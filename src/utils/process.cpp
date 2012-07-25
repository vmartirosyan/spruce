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

#include <process.hpp>
#include <signal.h>
#include <fstream>
using namespace std;

char * StatusMessages[] = {
	(char * )"Success",
	(char * )"Shallow",
	(char * )"Failed",
	(char * )"Unresolved",
	(char * )"Fatal",
	(char * )"Timeout",
	(char * )"Signaled",
	(char * )"Unsupported",
	(char * )"Unknown"
	};
	
string ProcessResult::StatusToString()
{ 
	if ( _status >= Success && _status <= Unknown )
		return (string)StatusMessages[_status];
	else
		return (string)StatusMessages[Unknown];
}

int Process::Level = 0;

ProcessResult::~ProcessResult()
{
}

ProcessResult * Process::Execute(vector<string> args)
{
	return Execute(&Process::Main, args);
}

ProcessResult * Process::Execute(int (Process::*func) (vector<string>) , vector<string> args)
{
	int fds[2];
	if ( pipe(fds) == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create pipe. " + (string)strerror(errno));		
	}
	
	// Create the child process
	pid_t ChildId = fork();
	
	if ( ChildId == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create child process. " + (string)strerror(errno));		
	}
	
	if ( ChildId == 0 ) // Child process. Run the Main method
	{
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
		cerr << " ";
		int status = (*this.*func)(args);
		close(1);
		close(2);
		close(fds[1]);
		_exit(status);
	}
	
	// Parent process...
	close(fds[1]);
	
	int status;
	int wait_res = waitpid(ChildId, &status, 0);
	//cerr << "wait_res=" << wait_res << endl;
	//cerr << "Error: " << strerror(errno) << endl;
			
	// In case of normal end of process. Let's collect the result;
	string Output = "";
	if ( wait_res != -1 )
	{
				
		char buf[1000];
		int bytes;
		while ( true )
		{
			bytes = read( fds[0], buf, 999 );
			//cerr << "bytes = " << bytes << endl;
			if ( bytes == -1 )
				break;
			buf[bytes] = 0;
			//cerr << Output.size() << "+" << bytes << ">=" << Output.max_size() << endl;
			/*if ( Output.size() + bytes >= Output.max_size() )
			{
				//cerr << "Waiting for child" << endl;
				return new ProcessResult(WEXITSTATUS(status), "Overflow! ");
			}*/
			
			Output += (string)buf;
			
			
			
			if (bytes < 999)
				break;
		}
	}
	else
	{
		// Kill the child. Just in case.
		kill(ChildId, SIGKILL);
	}
	
	
	close(fds[1]);
	
	/*ofstream of("/tmp/tests", ios_base::app);
	
			//of << "Bytes read: " << bytes << endl;
	of << Output << endl; 
	of.close();
	*/
	//cerr << "Output:  " << Output.substr(0,10) << endl;
	
	return new ProcessResult(WEXITSTATUS(status), Output);
}

ProcessResult * BackgroundProcess::Execute(vector<string> args)
{
	// Create the child process
	pid_t ChildId = fork();
	
	if ( ChildId == -1 )
	{
		return new ProcessResult(Unresolved, "Cannot create child process. " + (string)strerror(errno));		
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
	return new ProcessResult(WEXITSTATUS(status), "Child process is executed.");
}
