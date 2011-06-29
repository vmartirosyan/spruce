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

#include "process.hpp"

ProcessResult::~ProcessResult()
{
}


ProcessResult * Process::Execute(vector<string> args)
{
	// Duplicate the stdin/stdout/stderr descriptors
	int _stdin = dup(0);
	int _stdout = dup(1);
	int _stderr = dup(2);
	
	// Close the original descriptors
	close(0);
	close(1);
	close(2);
	
	// Create the pipe to be used between the parent and child processes
	int fds[2];
	// if the pipe could not be created then it's a fatal!
	if ( pipe(fds) == -1 )
	{
		return new ProcessResult(Unres, "Cannot create pipe. " + (string)strerror(errno));
	}
	
	// Create the error stream
	int error_stream = dup(fds[1]);
	if ( error_stream == -1 )
	{
		return new ProcessResult(Unres, "Cannot create error stream. " + (string)strerror(errno));		
	}
	
	// Create the child process
	pid_t ChildId = fork();
	
	if ( ChildId == -1 )
	{
		return new ProcessResult(Unres, "Cannot create child process. " + (string)strerror(errno));		
	}
	
	if ( ChildId == 0 ) // Child process. Run the main method
	{
		cerr << " ";
		_exit(Main(args));
	}
	
	// Parent process...
	int status;
	if ( waitpid(ChildId, &status, 0) == -1)
	{
		return new ProcessResult(Unres, "Cannot wait for child process. " + (string)strerror(errno));
	}
	
	// Probably normal end of test. Let's collect the result;
	char buf[10240];
	int bytes = read(fds[0], buf, 10240);
	buf[bytes] = 0;
	
	// Restore the stdin, stdout and stderr descriptors
	close(fds[0]);
	close(fds[1]);
	close(error_stream);
	dup(_stdin);
	dup(_stdout);
	dup(_stderr);
	
	return new ProcessResult(WEXITSTATUS(status), (string)buf);
}
