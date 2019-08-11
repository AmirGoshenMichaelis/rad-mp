
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

    std::string check_point_to_process;

    if (world_rank == 0)
    {
        // if not created then create the main radmc directory which contains all the check_point_radmc_dir
        // Create list of flash check point to process per rank
        std::vector<std::string> check_point_list = Create_Check_Point_list(opt_data["chk_dir"], opt_data["chk_prefix"]);

        // rearrange the check_point_list the match the world_rank size and add colon between entries
        check_point_list = ReArrange_chk_list(check_point_list, world_size);
        // Send using point to point communication the data to the other ranks
        // for(unsigned int i_rank=1; i_rank<world_size; ++i_rank)
        //    MPI_Send();

        // Initialize rank 0 check_point_to_process variable
        // check_point_to_process = check_point_list[0];
    }
    else
    {
        //MPI_Recv();
        //check_point_to_process;
    }

    // go over all check point (seperated by colon :)
    // create check_point_radmc_dir
    // run setup_radmc_sed.py
    // copy additional files ['dustkappa_silicate.inp', 'dustopac.inp', 'radmc3d.inp']
    // run radmc3d
    // run plot_radmc_sed


    // MPI_Barrier();
    // if (world_rank == 0)
    // {
    //     //run gen_light_curve
    //     //run plot_light_curve
    // }

    // Finalize the MPI environment.
    MPI_Finalize();
}

