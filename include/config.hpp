//      config.hpp
//      
//      Copyright 2011 Vahram Martirosyan <vmartirosyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "test.hpp"
#include "exception.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <fstream>

using std::string;
using std::ifstream;

struct RawConfigData // A single line of configuration file
{
	string ClassName;
	string Operation;
	string Arguments;
};

ifstream & operator >> (ifstream &, RawConfigData);

class Configuration
{
public:
	Configuration(const char * config_file_name)
	{
		_inf.open(config_file_name) ;
			//throw new Exception("Cannot open config file");
				
	}
	
private:
	ifstream _inf;
};

#endif /* CONFIG_HPP */
