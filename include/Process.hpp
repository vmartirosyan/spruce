//      Process.hpp
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

#ifndef PROCESS_HPP
#define PROCESS_HPP

enum ProcessMode
{
	ProcessForeground = 0,
	ProcessBackground = 1,
	ProcessNoCaptureOutput = 2
};

#include "Common.hpp"
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <signal.h>
using namespace std;

class ProcessResult
{
public:
	ProcessResult(Status s, string output):
		_status(s),	_output(output) 
	{
		
	}
								
	ProcessResult(const ProcessResult & pr) :
		_status(pr._status), _output(pr._output) 
	{
		
	}
	
	virtual ~ProcessResult() {}
	
	Status GetStatus() const
	{
		return _status;
	}
	
	void SetStatus(Status st)
	{
		if (st < 0 || st > Unknown)
			_status = Unknown;
		else
			_status = st;
	}
	void ModOutput( string s )
	{
		_output +=s;
	}
	void SetOutput( string s )
	{
		_output = s;
	}
	string GetOutput() const
	{
		return _output;
	}
		
	string StatusToString();
	string CheckToString(Checks);
protected:
	Status _status;
	string _output;
	
	
};

class Process;

typedef int (*ProcessFunc) (Process const *, vector<string>);


class Process
{
public:

	virtual ProcessResult * Execute(ProcessFunc = NULL, vector<string> args = vector<string>() );
	virtual ProcessResult * ExecuteNoCaptureOutput(ProcessFunc = NULL, vector<string> args = vector<string>() );
	
	int SetBlockSignalMask(int signum) { return sigaddset (&BlockSignalMask, signum);};
	
	Process():
		EnableAlarm(false),
		_Timeout(0),
		_mode(ProcessForeground)		
	{
		sigemptyset (&BlockSignalMask);
	}
	
	Process(int timeout):
		EnableAlarm(true),
		_Timeout(timeout),
		_mode(ProcessForeground)
	{
		sigemptyset (&BlockSignalMask);
	}
	
	virtual ~Process() 
	{
		
	}
protected:
	bool EnableAlarm;
	static int Level;
	int _Timeout;
	sigset_t BlockSignalMask;
	ProcessMode _mode;
	virtual int Main(vector<string>) { Logger::LogError("Main not implemented."); return Unsupported; }
};

class BackgroundProcess : public Process
{
public:	
	virtual ProcessResult * Execute(ProcessFunc = NULL, vector<string> args = vector<string>());
	
	virtual ~BackgroundProcess() {}
protected:
	virtual int Main(vector<string> args) = 0;
};

#endif /* PROCESS_HPP */
