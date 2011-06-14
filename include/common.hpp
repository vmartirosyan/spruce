//      common.hpp
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

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <iostream>
#include <vector>
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::pair;

typedef pair<int, string> Operation;

enum Mode
{
	Normal,
	FaultSimulation
};

enum Status
{
	Success,
	Fail,
	Unres,
	Timeout,
	Signaled,
	Unknown
};

#endif /* COMMON_HPP */
