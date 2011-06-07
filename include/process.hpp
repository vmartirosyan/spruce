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
protected:
	int _status;
	string _output;
};

class Process
{
public:	
	virtual ProcessResult * Execute();
	
	virtual ~Process() {}
protected:
	virtual Status Main() = 0;
	
};

#endif /* PROCESS_HPP */

