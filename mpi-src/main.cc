
/**
 *  rad-mp. Envelope for running RADMC3D on multi FLASH simulation checkpoints.
 *  Copyright (C) 2019 Amir Michaelis <amirmi@ariel.ac.il>
 *
 *  This file is part of rad-mp. rad-mp is free software: you can 
 *  redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, version 3.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/

//! \file main.cc
//  \brief The rad-mp MPI main.


#include <mpi.h>
#include <iostream>
#include <string>
#include <sstream>
#include "opt_data.h"
#include "util.h"

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Get command line options
    OptData opt_data(argc, argv);

    // Print wellcome message
    std::cout<<"Wellcome to rad-mp from processor "<<processor_name<<
               ", rank "<<world_rank<<
               " out of "<<world_size<<
               " processors"<<std::endl;

    std::string dir_name = opt_data["output_main_dir"]+"/radmc3d_db-incl-"+opt_data["incl"]+"-phi-"+opt_data["phi"];
    std::string check_point_to_process;

    if (world_rank == 0)
    {
        // if not created then create the main radmc directory which contains all the check_point_radmc_dir
        Create_Directory(dir_name);

        // Create list of flash check point to process per rank
        std::vector<std::string> check_point_list = Create_Check_Point_list(opt_data["chk_dir"], opt_data["chk_prefix"]);

        // rearrange the check_point_list the match the world_rank size and add colon between entries
        check_point_list = ReArrange_Chk_List(check_point_list, world_size);

        // Send using point to point communication the data to the other ranks
        for(unsigned int i_rank=1; i_rank<world_size; ++i_rank) 
            MPI_Send(check_point_list[i_rank].c_str(), check_point_list[i_rank].size()+1, MPI_CHAR, i_rank, 0 /* MPI_TAG */, MPI_COMM_WORLD);

        // Initialize rank 0 check_point_to_process variable
        check_point_to_process = check_point_list[0];
    }
    else
    {
        // receive from rank 0 the require job list (separate by colon :).
        MPI_Status status;
        char * chk_job;
        int chk_job_size;
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &chk_job_size);
        chk_job = new char[chk_job_size];
        MPI_Recv(chk_job, chk_job_size, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        check_point_to_process = chk_job;
        delete[] chk_job;
    }

    // go over all check point (seperated by colon :)
    std::vector<std::string> chk_job_list = Split(check_point_to_process, ':');

    for(auto chk_file:chk_job_list) {
        // create check_point_radmc_dir
        std::string job_dir_name = dir_name+"/"+Get_Base_Name(chk_file);
        Create_Directory(job_dir_name);
        std::cout<<"process job "<<world_rank<<" --- "<<chk_file<<std::endl;
        // run setup_radmc_sed.py
        std::ostringstream cmd;
        cmd<<opt_data["python_yt"]<<" "<<opt_data["py_radmc_dir"]<<"/setup_radmc_sed.py"<<" --dir "<<job_dir_name<< " --flash_chk "<<chk_file<<" --level "<<opt_data["refinement_level"];
        Run_Cmd(cmd.str().c_str());
        // copy additional files
        std::vector<std::string> file_to_copy {"dustkappa_silicate.inp", "dustopac.inp", "radmc3d.inp"};
        for(auto file_name: file_to_copy) {
            cmd.str("");
            cmd.clear();
            cmd<<"cp "<<opt_data["py_radmc_dir"]<<"/"<<file_name<<" "<<job_dir_name<<"/";
            Run_Cmd(cmd.str().c_str());
        }
        // run radmc3d mctherm
        cmd.str("");
        cmd.clear();
        cmd<<"cd "<<job_dir_name<<"; radmc3d mctherm";
        Run_Cmd(cmd.str().c_str());
        // run radmc3d sed
        cmd.str("");
        cmd.clear();
        cmd<<"cd "<<job_dir_name<<"; radmc3d sed"<<" incl "<<opt_data["incl"]<<" phi "<<opt_data["phi"];
        Run_Cmd(cmd.str().c_str());
        // run plot_radmc_sed
        cmd.str("");
        cmd.clear();
        cmd<<opt_data["python_yt"]<<" "<<opt_data["py_radmc_dir"]<<"/plot_radmc_sed.py"<<" --dir "<<job_dir_name;
        Run_Cmd(cmd.str().c_str());
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) {
        std::cout<<"run the following commands:"<<std::endl;
        std::cout<<opt_data["python_yt"]<<" "<<opt_data["py_radmc_dir"]<<"/gen_light_curve.py"<<" --chk_dir "<<dir_name<<" --chk_db_prefix "<<opt_data["chk_prefix"]<<" --verbose"<<std::endl;
        std::cout<<opt_data["python_yt"]<<" "<<opt_data["py_radmc_dir"]<<"/plot_light_curve.py"<<" --file "<<dir_name<<"/light_curve.out"<<" --not_interactive"<<std::endl;
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}

