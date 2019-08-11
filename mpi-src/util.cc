
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
#include <glob.h>
#include "util.h"

std::vector<std::string> Create_Check_Point_list(const std::string chk_dir, const std::string  chk_prefix) {
    std::vector<std::string> check_point_list;
    glob_t globbuf;
    //       Upon successful return, pglob->gl_pathc contains the number of matched pathnames and pglob->gl_pathv contains a pointer to the list of pointers to matched pathnames.  The list of pointers is terminated by a null pointer.
    glob((chk_dir+"/"+chk_prefix+"*").c_str(), GLOB_PERIOD|GLOB_MARK|GLOB_TILDE, nullptr, &globbuf);
    for(unsigned int i=0; i<globbuf.gl_pathc; ++i)
        check_point_list.push_back(globbuf.gl_pathv[i]);
    globfree(&globbuf);

    return check_point_list;
}
