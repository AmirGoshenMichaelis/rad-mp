
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

//! \file util.cc
//  \brief Utilities mainly for accessing the directory entry and more.

#include <string>
#include <vector>
#include <limits.h>
 #include <stdlib.h>
#include <glob.h>
#include "util.h"

std::vector<std::string> Create_Check_Point_list(const std::string chk_dir, const std::string  chk_prefix) {
    std::vector<std::string> check_point_list;
    glob_t globbuf;
    char * resolved_path;
    // Upon successful return, pglob->gl_pathc contains the number of matched pathnames and pglob->gl_pathv contains a pointer to the list of pointers to matched pathnames.  The list of pointers is terminated by a null pointer.
    glob((chk_dir+"/"+chk_prefix+"*").c_str(), GLOB_PERIOD|GLOB_MARK|GLOB_TILDE, nullptr, &globbuf);
    for(unsigned int i=0; i<globbuf.gl_pathc; ++i) {
        resolved_path = canonicalize_file_name(globbuf.gl_pathv[i]);
        check_point_list.push_back(resolved_path);
        free(resolved_path);
    }
    globfree(&globbuf);

    return check_point_list;
}

std::vector<std::string> ReArrange_chk_list(const std::vector<std::string> &check_point_list, const int world_size)
{
    std::vector<std::string> chk_jobs(world_size);
    unsigned int rank = 0;

    if( check_point_list.empty() )
        return chk_jobs;

    chk_jobs[0] = check_point_list[0];
    for(unsigned int i = 1; i<check_point_list.size(); ++i) {
        chk_jobs[rank].append(":").append(check_point_list[i]); 
        if( (i+1)%world_size && rank<(world_size-1) )
            rank++;
    }
    return(chk_jobs);
}