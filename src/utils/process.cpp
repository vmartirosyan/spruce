//      process.cpp
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

#include <KedrIntegrator.hpp>
#include <Logger.hpp>
#include <Process.hpp>
#include <signal.h>
#include <fstream>
using namespace std;

string StatusMessages[] = {	
	"Success",
	"Shallow",
	"Skipped",
	"Unsupported",
	"Unresolved",
	"Failed",	
	"Timeout",
	"Signaled",
	"FSimSuccess",
	"FSimFail",
	"Fatal",		
	"Oops",
	"Bug",
	"Panic",
		
	"Unknown" // Must be the last status
	
};
	
string ProcessResult::StatusToString()
{ 
	if ( _status >= Success && _status <= Unknown )
		return static_cast<string>(StatusMessages[_status]);
	else
		return static_cast<string>(StatusMessages[Unknown]);
}

int Process::Level = 0;
string Logger::_LogFile;
LogLevel Logger::_LogLevel;

ProcessResult::~ProcessResult()
{
}

ProcessResult * Process::Execute(vector<string> args)
{
	return Execute(&Process::Main, args);
}

bool ProcessAlarmed = false;

void ProcessSignalHandler(int signum)
{
	cerr << "Processing signal: " << signum << endl;
	switch (signum)
	{
		case SIGALRM:
			ProcessAlarmed = true;
			break;
		case SIGSEGV:
			cerr << "Segmentation fault!";
		default:
			_exit(Signaled);
	}	
	
}

ProcessResult * Process::Execute(int (Process::*func) (vector<string>) , vector<string> args)
{
	int fds[2];
	if ( pipe(fds) == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create pipe. " + static_cast<string>(strerror(errno)));
	}
	char * MountPoint = NULL;
	if ( getenv("MountAt") )
	{
		MountPoint = getenv("MountAt");    
	}
	//changing directory to mounting point 
	chdir(MountPoint);
	// Create the child process
	pid_t ChildId = fork();
	
	if ( ChildId == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create child process. " + static_cast<string>(strerror(errno)));
	}
	
	if ( ChildId == 0 ) // Child process. Run the Main method
	{
		// Set up the handler for segmentation fault 
		
		struct sigaction sa;
		bzero(&sa, sizeof(sa));
		
		sa.sa_handler = ProcessSignalHandler;
		if ( sigaction(SIGSEGV, &sa, NULL) == -1 )
		{	
			cerr << "Cannot set signal handler. " << strerror(errno);
			_exit(Unresolved);
		}
		
		close(1);
		close(2);
		close(fds[0]);
		int _stdout = -1, _stderr = -1;
		if ( (_stdout = dup(fds[1])) != 1 )
		{
			cerr << "Child: cannot open pipe for writing. Error: " << strerror(errno);
			_exit(Unresolved);
		}
		if ( (_stderr = dup(1)) != 2 )
		{
			cerr << "Child: cannot create error stream. Error: " << strerror(errno);
			close(1);
			_exit(Unresolved);
		}
		//cerr << " ";
		int status = (*this.*func)(args);
		close(1);
		close(2);
		close(fds[1]);
		_exit(status);
	}
	//Freeing the mount point (have to set the CWD back after)
	if(MountPoint != NULL)
		chdir("/");
	// Parent process...
	close(fds[1]);
	
	if (EnableAlarm)
	{
		struct sigaction sa;
		bzero(&sa, sizeof(sa));
		
		sa.sa_handler = ProcessSignalHandler;
		if ( sigaction(SIGALRM, &sa, NULL) == -1 )
		{	
			return new ProcessResult(Unresolved, "Cannot set signal handler. " + static_cast<string>(strerror(errno)));
		}
	
		alarm(TEST_TIMEOUT);
	}
	
	int status;
	int wait_res = waitpid(ChildId, &status, WUNTRACED);
	
	alarm(0);
	
	if ( wait_res == -1 )
	{
		cerr << "wait_res=" << wait_res << endl;
		cerr << "Error: " << strerror(errno) << endl;
	
		kill(ChildId, SIGKILL);
		if ( ProcessAlarmed )
			return new ProcessResult(Timeout, "Child process has timed out.");
		else
			return new ProcessResult(Signaled, "Child process has been signalled.");
	}
	
	// In case of normal end of process. Let's collect the result;
	string Output = "";
	if ( wait_res != -1 )
	{
				
		char buf[1000];
		int bytes;
		while ( true )
		{
			bytes = read( fds[0], buf, 999 );

			if ( bytes == -1 )
				break;
			buf[bytes] = 0;
		
			Output += static_cast<string>(buf);
			
			if (bytes < 999)
				break;
		}
	}
	// Kill the child. Just in case.
	kill(ChildId, SIGKILL);
	
	
	close(fds[0]);
	return new ProcessResult(static_cast<Status>(WEXITSTATUS(status)), Output);
}

ProcessResult * BackgroundProcess::Execute(vector<string> args)
{
	// Create the child process
	pid_t ChildId = fork();
	
	if ( ChildId == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create child process. " + static_cast<string>(strerror(errno)));
	}
	
	if ( ChildId == 0 ) // Child process. Run the Main method
	{
		// Close the original descriptors
		close(0);
		close(1);
		close(2);
		
		int in_stream = open("/dev/zero", O_RDONLY);		
		int out_stream = open("/dev/null", O_WRONLY);		
		int err_stream = open("/dev/null", O_WRONLY);
		if ( in_stream == -1 || out_stream == -1 || err_stream == -1 )
		{
			cerr << "Error opening dummy streams. Error : " << strerror(errno);			
		}
		
		_exit(Main(args));
	}
	
	int status = 0;
	
	// This is a background process. No need to wait for the child :)
	return new ProcessResult(static_cast<Status>(WEXITSTATUS(status)), "Child process is executed.");
}
