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

#include <PlatformConfig.hpp>
#include <Logger.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <pwd.h>
#include <linux/limits.h>
#include <limits.h>
#include <dirent.h>
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

enum Check
{
	None = 0,
	Functional = 1,
	Stability = 2,
	MemoryLeak = 4,
	Dangerous = 8,
	All = Functional | Stability | MemoryLeak
};

const string strFuncional = "func";
const string strStability = "stability";
const string strMemoryLeak = "mem-leak";
const string strDangerous = "dangerous";

// A combination of flags from enum Check
typedef unsigned int Checks;

enum Status
{
	Success = 0, // 0 means Success!
	Shallow,
	Skipped,
	Unsupported,
	Unresolved,
	Fail,	
	Timeout,
	Signaled,
	FSimSuccess,
	FSimFail,
	Fatal,	// Any error coming after this one is counted as Fatal!
	Oops,
	Bug,
	Panic,
	
	Unknown	 // Must be the last status
};
extern string StatusMessages[];

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

#define EXIT_IF_SIGNALED if(terminate_process) { Logger::LogFatal("Exiting from Spruce..."); break; }

//#define MY_WEXITSTATUS(stat) (((*(static_cast<int *>( &(stat)))) >> 8) & 0xff)

#define Return(st) \
{\
	if ( _InFooter )\
	{\
		return st;\
	}\
	else\
	{\
		_TestStatus = st;\
		goto Footer;\
	}\
}\

// The only thing which the child process has to do
// is to manipulate the `pid` value.
// In fact setting `pid` to -1 means stop any kedr activity on the point
#define EnableFaultSim()\
{\
	KedrIntegrator::SetPid(obj->GetCurrentPoint());\
}\

#define DisableFaultSim()\
{\
	KedrIntegrator::ClearPid(obj->GetCurrentPoint());\
}\

 /*\
{\
	errno = 0;/\
	KedrIntegrator::ResetTimes( _fsim_point );\
	if (_fsim_enabled)\
	{\
		KedrIntegrator::SetIndicator(_fsim_point, "common", _fsim_expression);\
		Logger::LogInfo(static_cast<string>("Fault simulation is enabled for module ") + FileSystem + "(" +\
			_fsim_point + ", " + _fsim_expression + ")");\
	}\
}*/
	
/*#define DisableFaultSim() \
{\
	if (_fsim_enabled)\
	{\
		KedrIntegrator::ClearIndicator(_fsim_point);\
		Logger::LogInfo(static_cast<string>("Fault simulation is disabled for module ") + FileSystem + "(" +\
			_fsim_point + ", " + _fsim_expression + ")");\
	}\
}*/

#define SetFaultCount() \
	_fault_count = KedrIntegrator::GetTimes( _fsim_point); 
	
#define ERROR_3_ARGS(message, add_msg, status)\
{\
	int local_errno = errno;\
	string msg = static_cast<string>(message) + add_msg;\
	if ( status >= Unresolved )\
		Logger::LogError(msg);\
	else \
		Logger::LogInfo(msg);\
	if ( ((local_errno == ENOTSUP) || (local_errno == ENOTTY)) && (status != -1)) Return(Unsupported);\
	if ( (status != -1) && (status != Unsupported) ) Return(status);\
	if ( status == Unsupported ) Return(Unsupported);\
}\

#define ERROR_2_ARGS(message, status)\
	ERROR_3_ARGS(message, "", status)

#define ERROR_1_ARGS(message)\
	Logger::LogError(message);\

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define ERROR_MACRO_CHOOSER(...) \
    GET_4TH_ARG(__VA_ARGS__, ERROR_3_ARGS, ERROR_2_ARGS, ERROR_1_ARGS, )

#define Error(...) { ERROR_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) }

// The Fail macro should be called any time the target function is to be invoked.
// The macro invokates the target function and makes all the necessary checks.

// Fail macro should behaive differently when the fault simulation is in progress.
// In that case it is normal that target functions should fail.
// On the other side it is really strange that a function can success even though
// it cannot allocate memory or something like that
#define Fail(cond, message)\
{\
	if ( obj->GetEffectiveChecks() & Stability )\
		EnableFaultSim();\
	bool res = (cond);\
	if ( obj->GetEffectiveChecks() & Stability )\
		DisableFaultSim();\
	if ( res )\
		{ Error(message, Fail) }\
}\

// The Check macro should be called when a real functional check is being done
// For example: comparison of set and obtained values in case of get/set tests.
#define Check(cond, message)\
{\
	if ( obj->GetEffectiveChecks() & Functional )\
	{\
		if ( (cond) )\
			Error(message, Fail);\
	}\
}\
	


#define Skip(cond, message)\
	if ( (cond) )\
		{ Error(message, Skipped) }\

#define Fatal(cond, message)\
	if ( (cond) )\
		{ Error(message, Fatal) }\

#define Unres(cond, message)\
	if ( (cond) )\
	{ Error(message, Unresolved) }
	
#define Unsupp(message)\
	{ Error(message, Unsupported) }

#define ELoopTest(func_call, error_val)\
{\
	if ( PartitionManager::IsOptionEnabled("ro") )\
		Unsupp("Read-only file system.");\
	string sLink = FilePaths[0] + "_link";\
	const char * Link = sLink.c_str();\
	Unres( symlink(FilePaths[0].c_str(), Link) == -1, "Cannot create symlink on old file.");\
	Unres( unlink(FilePaths[0].c_str()) == -1, "Cannot remove old_file. ");\
	Unres( symlink(Link, FilePaths[0].c_str()) == -1, "Cannot create symlink on new_file.");\
	errno = 0;\
	int res = func_call;\
	unlink(Link);\
	if ( res !=  error_val || errno != ELOOP )\
	{\
		Error("Function should return '" + static_cast<string>(strerror(ELOOP)) +  "' error code but it did not.", Fail);\
	}\
	Return(Success);\
}\

#define ELoopDirTest(func_call, error_val)\
{\
	if ( PartitionManager::IsOptionEnabled("ro") )\
		Unsupp("Read-only file system.");\
    string dirPath = "TestDirectory/";\
    string link1 = "link1";\
    string link2 = "link2";\
	Directory dir(dirPath, 0777);\
	Unres (chdir(dirPath.c_str()) == -1, "Cannot change directory");\
	Unres (symlink (dirPath.c_str(), link1.c_str()) != 0, "symlink() can't create symlink1.");\
	Unres (symlink (link1.c_str(), link2.c_str()) != 0, "symlink() can't create symlink2.");\
	Unres (unlink (link1.c_str()) != 0, "remove() can't remove symlink.");\
	Unres (symlink (link2.c_str(), link1.c_str()) != 0, "symlink() can't create symlink3.");\
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
	string path1 = (FilePaths[0] + "/some_file");\
	const char * path = path1.c_str();\
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
			Error("Can not get the 'nobody' user data.");\
			_exit(Unresolved);\
		}\
		if(setuid(noBody->pw_uid) != 0)\
		{\
			Error("Can not set uid");\
			_exit(Unresolved);\
		}\
		errno = 0;\
		if (func_call != error_val || ( ( errno != EACCES ) && ( errno != EPERM ) )) {\
			if(errno == ENOTSUP || errno == ENOTTY) {\
				Error("Functionality is not supported\n");\
				_exit( Unsupported );\
				}\
			else {\
				Error("Function should return '" + static_cast<string>(strerror(EACCES)) +  "' or '" + static_cast<string>(strerror(EPERM)) +  "' when permission was denied but it did not.");\
				_exit( Fail );\
			}\
		}\
		_exit(Success);\
	}\
	int status;\
	wait(&status);\
	Return( static_cast<Status>(WEXITSTATUS(status)) );\
}\

#define EMaxFilesOpenTest(func_call, error_val)\
{\
	if ( PartitionManager::IsOptionEnabled("ro") )\
		Unsupp("Read-only file system.");\
	long max_files_open=sysconf(_SC_OPEN_MAX);\
	char path[50];\
	sprintf(path, "%s", "max_files_test");\
	int firstFd = open(path, O_CREAT | O_RDONLY, 0777);\
	if(firstFd == -1)\
	{\
		Unres(true, "Failed to open first file");\
	}\
	DIR * fdDir = opendir("/proc/self/fd");\
	Unres(fdDir == NULL, "Opendir /proc/self/fd failed");\
	dirent * dirEntry = NULL;\
	int busyFdCount = 0;\
	while((dirEntry = readdir(fdDir)) != NULL)\
	{\
		busyFdCount++;\
	}\
	busyFdCount -= 2;\
	for (int file_index = 0; file_index < max_files_open - busyFdCount; ++file_index)\
	{\
		sprintf(path, "max_files_test%d", file_index);\
		int ret_val = open(path, O_CREAT | O_RDONLY, 0777);\
		if ( ret_val == -1 )\
		{\
			unlink(path);\
			char buf[3];\
			sprintf(buf, "%d-th", file_index);\
			Unres(true, "Cannot create the " + static_cast<string>(buf) + "file");\
		}\
	}\
	errno = 0;\
	int ret_val = func_call;\
	unlink(path);\
	if ( ret_val != error_val || errno != EMFILE )\
	{\
		Error("Function should return '" + static_cast<string>(strerror(EMFILE)) +  "' error but it did not.", Fail);\
	}\
	closedir(fdDir);\
	Return(Success);\
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
			Error("Function should return '" + static_cast<string>(strerror(error_code)) + \
				"' error but returned '" + static_cast<string>(strerror(errno)) +  "'", Fail);\
		}\
	}\
	Return(Success);\
}\

#define EmptyTestSet(package_name, test_set_name, status, message)\
TestSet Init_##package_name##_##test_set_name()\
{\
	return TestSet(message);\
}

vector<string> SplitString(string str, char delim, vector<string> AllowedValues = vector<string>());
void StrReplace(string& str, string val1, string val2);

bool SkipTestPath(string Path);

#endif /* COMMON_HPP */

