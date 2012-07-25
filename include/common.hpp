//      common.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
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

#include <platform_config.hpp>
#include <string>
#include <iostream>
#include <vector>
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::pair;


enum Mode
{
	Normal,
	FaultSimulation
};

enum Status
{
	Success,
	Shallow,
	Fail,
	Unresolved,
	Fatal,
	Timeout,
	Signaled,
	Unsupported,
	Unknown
};

#define ERROR_3_ARGS(message, add_msg, status)\
	cerr << message << add_msg;\
	if ( errno ) cerr << "\nError: " << strerror(errno) << endl;\
	if (status != -1) return status;

#define ERROR_1_ARGS(message)\
	ERROR_2_ARGS(message, -1)
		
#define ERROR_2_ARGS(message, status)\
	ERROR_3_ARGS(message, "", status)
	

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define ERROR_MACRO_CHOOSER(...) \
    GET_4TH_ARG(__VA_ARGS__, ERROR_3_ARGS, ERROR_2_ARGS, ERROR_1_ARGS )

#define Error(...) ERROR_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define Fail(message, cond)\
	if ( (cond) )\
		{ Error(message, Fail) }\
	else return Success;
	
#define Unres(cond, message)\
	if ( (cond) )\
	{ Error(message, Unresolved) }

#endif /* COMMON_HPP */
