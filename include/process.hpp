#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "common.hpp"
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

class ProcessResult
{
public:
	ProcessResult(int s, string output):
		_status(s),
		_output(output) {}	
	virtual ~ProcessResult();
	int GetStatus()
	{
		return _status;
	}
	string GetOutput()
	{
		return _output;
	}		
protected:
	int _status;
	string _output;
};

class Process
{
public:	
	virtual ProcessResult * Execute(vector<string> args = vector<string>());
	
	virtual ~Process() {}
protected:
	virtual int Main(vector<string> args) = 0;	
};

#endif /* PROCESS_HPP */

