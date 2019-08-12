
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

//! \file opt_data.cc
//  \brief Implemnitaton of opt_data.h for reading command line options.

#include <getopt.h>
#include <unistd.h>
#include <map>
#include <string>
#include <iostream>
#include "opt_data.h"

// const char * OptData::shortopts = {"d:p:i:s:a:b:h"};
const struct option OptData::longopts[] = {
    {"chk_dir", required_argument, 0, 'd'},
    {"chk_prefix", required_argument, 0, 'p'},
    {"python_yt", required_argument, 0, 'i'},
    {"py_radmc_dir", required_argument, 0, 's'},
    {"output_main_dir", required_argument, 0, 'o'},
    {"incl", required_argument, 0, 'a'},
    {"phi", required_argument, 0, 'b'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
    };

OptData::OptData(const int argc, char * const *argv) {
    int c;
    int index;
    char * cwd = get_current_dir_name();

    opt["chk_dir"] = "";
    opt["chk_prefix"] = "";
    opt["python_yt"] = "/usr/bin/python";
    opt["py_radmc_dir"] = cwd;
    opt["output_main_dir"] = cwd;
    opt["incl"] = "0.0";
    opt["phi"] = "0.0";
    opt["refinement_level"] = "2";

    free(cwd);

    while( (c = getopt_long_only(argc, argv, ""/*shortopts*/, longopts, &index)) != -1 ) {
        if(c=='h') {
        Print_Usage();
        exit(EXIT_SUCCESS);
        };
        if(c=='?' || c==':') {
        Print_Usage();
        exit(EXIT_FAILURE);
        };
        opt[longopts[index].name] = optarg;
    };
    if( opt["chk_dir"].empty() || opt["chk_prefix"].empty() ) {
        std::cerr<<"You must provid --chk_dir and --chk_prefix !!! "<<std::endl;
        Print_Usage();
        exit(EXIT_FAILURE);
    };
}

void OptData::Print_Usage(void) const
{
    std::cout <<"Usage: rad-mp \n\t"<<
                "--chk_dir directory containing the check points \n\t"<<
                "--chk_prefix the pattern for the flash check file to process \n\t"<<
                "--python_yt command to run python interperter that support yt  \n\t"<<
                "--output_main_dir \n\t"<<
                "--py_radmc_dir dirctory containing the python scripts and *.inp files for radmc3d \n\t"<<
                "--incl inclination angle angle pass into radmc3d \n\t"<<
                "--phi phi angle pass into radmc3d \n\t"<<
                "--help print this message \n\t"<<
                std::endl;
}

const std::string OptData::operator [](const std::string & key) const {
    return opt.at(key);
}

const std::string OptData::operator [](const char * key) const {
    return opt.at(key);
}
