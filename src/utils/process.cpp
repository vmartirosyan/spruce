#include "process.hpp"

ProcessResult::~ProcessResult()
{
}


ProcessResult * Process::Execute()
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
		_exit(Main());
	}
	
	// Parent process...
	int status;
	if ( waitpid(ChildId, &status, 0) == -1)
	{
		return new ProcessResult(Unres, "Cannot wait for child process. " + (string)strerror(errno));
	}
	
	// Probably normal end of test. Let's collect the result;
	char buf[1024];
	int bytes = read(fds[0], buf, 1024);
	buf[bytes] = 0;
	
	// Restore the stdin, stdout and stderr descriptors
	close(fds[0]);
	close(fds[1]);
	close(error_stream);
	dup(_stdin);
	dup(_stdout);
	dup(_stderr);
	
	return new ProcessResult(status, (string)buf);
}
