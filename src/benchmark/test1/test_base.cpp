#include "test_base.hpp"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

TestResultCollection TestBase::Run()
{
	TestResultCollection Results;
		
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
		Results.AddResult(Unres, strerror(errno), "pipe creation", "");
		return Results;
	}
	
	int error_stream = dup(fds[1]);
	
	for ( unsigned int index = 0 ; index < _operations.size(); ++index)
	{
		// Create the child process
		pid_t ChildId = fork();
		
		if ( ChildId == -1 )
		{
			Results.AddResult(Unres, strerror(errno), "fork", "");
			continue;
		}
		
		if ( ChildId == 0 ) // Child process. Run the real test
		{
			_exit(RealRun(_operations[index].first, _operations[index].second));
		}
		else // Parent process. Wait for the child
		{
			int status;
			if ( waitpid(ChildId, &status, 0) == -1)
			{
				Results.AddResult(Unres, strerror(errno), "waitpid", "");
				continue;
			}
			
			// Probably normal end of test. Let's collect the result;
			char buf[1024];
			int bytes = read(fds[0], buf, 1024);
			buf[bytes] = 0;
			
			
			// Return what we have got from the child process
			Results.AddResult((Status)status, (string)buf, "test operation", _operations[index].second);
		}
	}
	// Restore the stdin, stdout and stderr descriptors
	close(fds[0]);
	close(fds[1]);
	close(error_stream);
	dup(_stdin);
	dup(_stdout);
	dup(_stderr);
	
	return Results;
}

string TestResult::ToXML()
{
	return _error + " : " + _operation + " : " + _arguments;
}
