//      getdents.hpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Eduard Bagrov <ebagrov@gmail.com>
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

#ifndef GETDENTS_H
#define GETDENTS_H

#include "SyscallTest.hpp"
#include "UnixCommand.hpp"
#include "exception.hpp"
#include <memory>
#include <vector>

using namespace std;

// Operations
enum GetDentsSyscalls
{
	GetDentsEbadfError,
	GetDentsEfaultError,
	GetDentsEinvalError,
	GetDentsEnotdirError,
	GetDentsGeneral
};

class GetDentsTest : public SyscallTest
{			
public:	
	GetDentsTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "getdents"), 
		directory(randomName(6))
	{
		Status status = createDirWithFiles(directory, 3);
		cerr << "status = " << status ;
		if (status != Success)
		{
			cerr << "exception no no ";
			//throw Exception("Cannot create directory for testing, error = " +  
				//static_cast<string>(strerror(errno)));
		}
	}
	virtual ~GetDentsTest() 
	{
		vector<string> arguments;
		arguments.push_back(directory);
		arguments.push_back("-rf");
		
		UnixCommand command("rm");
		auto_ptr<ProcessResult> result(command.Execute(arguments));
	}
	
	Status GetDentsEbadfErrorTest();
	Status GetDentsEfaultErrorTest();
	Status GetDentsEinvalErrorTest();
	Status GetDentsEnotdirErrorTest();
	Status GetDentsGeneralTest();
	
protected:
	virtual int Main(vector<string> args);
	
private:
	string directory;
	Status createDirWithFiles(string dirName, int numOfFiles);
	Status deleteDir(string dirName);
	string randomName(int n);
};
#endif
