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
#include <pwd.h>
#include <linux/limits.h>
#include <limits.h>
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
	Skipped,
	Unknown
};

struct FSimInfo
{
	FSimInfo():
		Point(""),
		Count(1),
		Expression("") {}
	string Point;
	unsigned int Count;
	string Expression;
};

#define EnableFaultSim() \
{\
	if (_fsim_enabled)\
	{\
		KedrIntegrator::SetIndicator(_fsim_point, "common", _fsim_expression);\
		cerr << "Fault simulation is enabled for module " << FileSystem << "(" << _fsim_point << ", " << _fsim_expression << ")" << endl;\
	}\
}
	
#define DisableFaultSim() \
{\
	SetFaultCount();\
	if (_fsim_enabled)\
	{\
		KedrIntegrator::ClearIndicator(_fsim_point);\
		cerr << "Fault simulation is disabled for module " << FileSystem << "(" << _fsim_point << ", " << _fsim_expression << ")" << endl;\
	}\
}
//added
#define SetFaultCount() \
	_fault_count = KedrIntegrator::GetTimes( _fsim_point); 
	
#define ERROR_3_ARGS(message, add_msg, status)\
	cerr << message << add_msg;\
	if ( errno ) cerr << "\nError: " << strerror(errno) << endl;\
	if (status != -1) return status;

#define ERROR_2_ARGS(message, status)\
	ERROR_3_ARGS(message, "", status)

#define ERROR_1_ARGS(message)\
	ERROR_3_ARGS(message, "", -1)

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define ERROR_MACRO_CHOOSER(...) \
    GET_4TH_ARG(__VA_ARGS__, ERROR_3_ARGS, ERROR_2_ARGS, ERROR_1_ARGS, )

#define Error(...) ERROR_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define Fail(cond, message)\
	if ( (cond) )\
		{ Error(message, Fail) }\
		
#define Unres(cond, message)\
	if ( (cond) )\
	{ Error(message, Unresolved) }

#define ELoopTest(func_call, error_val)\
{\
	const char * Link = (FilePaths[0] + "_link").c_str();\
	Unres( symlink(FilePaths[0].c_str(), Link) == -1, "Cannot create symlink on old file.");\
	Unres( unlink(FilePaths[0].c_str()) == -1, "Cannot remove old_file. ");\
	Unres( symlink(Link, FilePaths[0].c_str()) == -1, "Cannot create symlink on new_file.");\
	errno = 0;\
	int res = func_call;\
	unlink(Link);\
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
	Unres (chdir(dirPath.c_str()) == -1, "Cannot change directory");\
	Unres (symlink (dirPath.c_str(), link1.c_str()) != 0, "symlink() can't create symlink.");\
	Unres (symlink (link1.c_str(), link2.c_str()) != 0, "symlink() can't create symlink.");\
	Unres (unlink (link1.c_str()) != 0, "remove() can't remove symlink.");\
	Unres (symlink (link2.c_str(), link1.c_str()) != 0, "symlink() can't create symlink.");\
	Unres (chdir("..") == -1, "Cannot change back to parent directory");\
	const char *path = (dirPath + link1).c_str();\
	ErrorTest(func_call, error_val, ELOOP);\
}\

#define ENameTooLongTest(func_call, error_val)\
{\
	char path[PATH_MAX+1];\
	for ( int i = 0; i < PATH_MAX + 1; ++i )\
		path[i] = 'a';\
	ErrorTest(func_call, error_val, ENAMETOOLONG);\
}\

#define ENotDirTest(func_call, error_val)\
{\
	char * path = (char*)(FilePaths[0] + "/some_file").c_str();\
	ErrorTest(func_call, error_val, ENOTDIR);\
}\

#define ENoAccessTest(func_call, error_val)\
{\
	pid_t pid = 0;\
	Unres( (pid = fork()) == -1, "Cannot fork!");\
	if ( pid == 0 )\
	{\
		struct passwd * noBody;\
		/* Change root user to nobody */\
		if((noBody = getpwnam("nobody")) == NULL)\
		{\
			cerr << "Can not get the 'nobody' user data.";\
			_exit(Unresolved);\
		}\
		if(setuid(noBody->pw_uid) != 0)\
		{\
			cerr << "Can not set uid";\
			_exit(Unresolved);\
		}\
		errno = 0;\
		if (func_call != error_val || ( ( errno != EACCES ) && ( errno != EPERM ) )) {\
			cerr << "Function should return '" + (string)strerror(EACCES) +  "' or '" + (string)strerror(EPERM) +  "' when permission was denied but it did not.";\
			if ( errno )\
				cerr << "Error: " << strerror(errno) << endl;\
			_exit( Fail );\
		}\
		_exit(Success);\
	}\
	int status;\
	wait(&status);\
	return WEXITSTATUS(status);\
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
			unlink(path);\
			char buf[3];\
			sprintf(buf, "%d", file_index);\
			Unres(true, "Cannot create file " + (string)buf);\
		}\
	}\
	errno = 0;\
	int ret_val = func_call;\
	for ( int i = FirstFileDesc; i < max_files_open - 1; ++i )\
		close(i);\
	unlink(path);\
	if ( ret_val != error_val || errno != EMFILE )\
	{\
		Error("Function should return '" + (string)strerror(EMFILE) +  "' error but it did not.", Fail);\
	}\
	return Success;\
}\

#define ErrorTest(func_call, error_val, error_code)\
{\
	errno = 0;\
	if ( func_call != error_val )\
	{\
		Error("Function should fail but it did not.", Fail);\
	}\
	if (error_code != 0)\
	{\
		if (  errno != error_code )\
		{\
			Error("Function should return '" + (string)strerror(error_code) +  "' error but it did not.", Fail);\
		}\
	}\
	return Success;\
}\

#define EmptyTestSet(module_name, test_set_name, status, message)\
class test_set_name##Tests : public Process\
{\
public:\
	void RunTest(string s) {}\
	void ExcludeTest(string s) {}\
	virtual TestResultCollection RunNormalTests()\
	{\
		cerr << "EmptyTestSet" << endl;\
		TestResultCollection res;\
		module_name##TestResult * tr = new module_name##TestResult(new ProcessResult(status, message), #test_set_name, "None");\
		res.AddResult(tr);\
		return res;\
	}\
	virtual TestResultCollection RunFaultyTests()\
	{\
		cerr << "EmptyTestSet" << endl;\
		TestResultCollection res;\
		module_name##TestResult * tr = new module_name##TestResult(new ProcessResult(status, message), #test_set_name, "None");\
		res.AddResult(tr);\
		return res;\
	}\
};

#endif /* COMMON_HPP */
