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

//! \file opt_data.h
//  \brief Data types and declerations for reding command line options into the code.

#include <getopt.h>
#include <map>
#include <string>

class OptData
{
    static const struct option longopts[];
    //static const char * shortopts;
    std::map<std::string, std::string> opt;

public:
    OptData(const int argc, char *const *argv);
    void Print_Usage(void) const;
    const std::string operator[](const std::string &) const;
    const std::string operator[](const char *) const;
};
