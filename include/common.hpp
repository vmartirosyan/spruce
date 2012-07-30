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

#define ELoopTest(func_call, error_val)\
{\
	Unres( symlink(FilePaths[0].c_str(), "new_file") == -1, "Cannot create symlink on old file.");\
	Unres( unlink(FilePaths[0].c_str()) == -1, "Cannot remove old_file. ");\
	Unres( symlink("new_file", FilePaths[0].c_str()) == -1, "Cannot create symlink on new_file.");\
	int res = func_call;\
	unlink("new_file");\
	if ( res !=  error_val || errno != ELOOP )\
	{\
		Error("Function should return '" + (string)strerror(ELOOP) +  "' error code but it did not.", Fail);\
	}\
	return Success;\
}\

#define ELoopDirTest(func_call, error_val)\
{\
    string dirPath = "TestDirectory/";\
    string link1 = "link1";\
    string link2 = "link2";\
	Directory dir(dirPath, 0777);\
	chdir(dirPath.c_str());\
	Unres (symlink (dirPath.c_str(), link1.c_str()) != 0, "symlink() can't create symlink.");\
	Unres (symlink (link1.c_str(), link2.c_str()) != 0, "symlink() can't create symlink.");\
	Unres (unlink (link1.c_str()) != 0, "remove() can't remove symlink.");\
	Unres (symlink (link2.c_str(), link1.c_str()) != 0, "symlink() can't create symlink.");\
	chdir("..");\
	const char *path = (dirPath + link1).c_str();\
	int res = func_call;\
	if ( res !=  error_val || errno != ELOOP )\
	{\
		Error("Function should return '" + (string)strerror(ELOOP) +  "' error code but it did not.", Fail);\
	}\
	return Success;\
}\

#define ENameTooLongTest(func_call, error_val)\
{\
	char path[PATH_MAX+1];\
	for ( int i = 0; i < PATH_MAX + 1; ++i )\
		path[i] = 'a';\
	if ( func_call != error_val || errno != ENAMETOOLONG )\
	{\
		Error("Function should return '" + (string)strerror(ENAMETOOLONG) +  "'error  but it did not.", Fail);\
	}\
	return Success;\
}\

#define ENotDirTest(func_call, error_val)\
{\
	const char * path = (FilePaths[0] + "/some_file").c_str();\
	if ( func_call != error_val || errno != ENOTDIR )\
	{\
		Error("Function should return '" + (string)strerror(ENOTDIR) +  "' error but it did not.", Fail);\
	}\
	return Success;\
}\

#define ENoAccessTest(func_call, error_val)\
{\
	struct passwd * noBody;\
	/* Change root user to nobody */\
	Unres((noBody = getpwnam("nobody")) == NULL, "Can not get the 'nobody' user data.");\
	Unres(seteuid(noBody->pw_uid) != 0, "Can not set uid");\
	if (func_call != error_val || errno != EACCES) {\
		Error("Function should return '" + (string)strerror(EACCES) +  "' when permission was denied but it did not.", Fail);\
	}\
	return Success;\
}\

#define EMaxFilesOpenTest(func_call, error_val)\
{\
	long max_files_open=sysconf(_SC_OPEN_MAX);\
	int FirstFileDesc = -1;\
	const char * path = "max_files_test";\
	FirstFileDesc = open(path, O_CREAT | O_RDONLY, 0777);\
	Unres(FirstFileDesc == -1, "Cannot create the first file.");\
	\
	for (int file_index = FirstFileDesc + 1; file_index < max_files_open - 2; ++file_index)\
	{\
		int ret_val = open(path, O_RDONLY, 0777);\
		if ( ret_val == error_val )\
		{\
			for ( int i = FirstFileDesc; i <= file_index; ++i )\
				close(i);\
			char buf[3];\
			sprintf(buf, "%d", file_index);\
			Unres(true, "Cannot create file " + (string)buf);\
		}\
	}\
	int ret_val = func_call;\
	for ( int i = FirstFileDesc; i < max_files_open - 1; ++i )\
		close(i);\
	if ( ret_val != error_val || errno != EMFILE )\
	{\
		Error("Function should return '" + (string)strerror(EMFILE) +  "' error but it did not.", Fail);\
	}\
	return Success;\
}\

#define ErrorTest(func_call, error_val, error_code)\
{\
	if ( func_call != error_val || errno != error_code )\
	{\
		Error("Function should return '" + (string)strerror(error_code) +  "' error but it did not.", Fail);\
	}\
	return Success;\
}\

#endif /* COMMON_HPP */
