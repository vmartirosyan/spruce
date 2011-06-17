//      process.hpp
//      
//      Copyright 2011 Vahram Martirosyan <vmartirosyan@gmail.com>
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

#include "common.hpp"
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

class ProcessResult
{
public:
	ProcessResult(int s, string output):
		_status(s),	_output(output) {}
								
	ProcessResult(ProcessResult const & pr) :
		_status(pr._status), _output(pr._output) {}
	
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
