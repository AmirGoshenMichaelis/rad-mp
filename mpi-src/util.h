#pragma once

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

//! \file util.h
//  \brief Utilities header and datatypes (mainly for accessing the directory entry).

#include <string>
#include <vector>

std::vector<std::string> Create_Check_Point_list(const std::string& chk_dir, const std::string& chk_prefix);
std::vector<std::string> ReArrange_Chk_List(const std::vector<std::string>& check_point_list, const int world_size);
std::vector<std::string> Split(const std::string& s, char delimiter);
bool Create_Directory(const std::string& dir_name);
std::string Get_Base_Name(const std::string& path);
