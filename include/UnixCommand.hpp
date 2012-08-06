//      UnixCommand.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
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
#ifndef UNIX_COMMAND_H
#define UNIX_COMMAND_H

#include <process.hpp>
#include <string>
#include <vector>
#include <unistd.h>
 
// Class representing a UNIX command.  
//
// Example usage:	
// 		UnixCommand uc("ls");
// 		ProcessResult* pr = uc.Execute();
// 		cout << pr->GetOutput()<< endl;
class UnixCommand : public BackgroundProcess
{
public:	
	UnixCommand(string name, ProcessMode mode = ProcessForeground) : _name(name), _mode(mode) 
	{
		
	}
	
	virtual ProcessResult * Execute(vector<string> args = vector<string>())
	{
		ofstream of("/tmp/test", ios_base::app);
		
		ProcessResult * res = ( ( _mode == ProcessBackground ) ? BackgroundProcess::Execute(args) : Process::Execute(args) );
		//ProcessResult * res = Process::Execute(args);
		
		
		
		of << res << "\tOutput \"" << res->GetOutput().substr(0,30) << "\"" << endl;
		
		of.close();		
		
		return res;
	}
	~UnixCommand() 
	{
		
	}
	
protected:
	
	int Main(vector<string> args) 
	{
		char** argv = new char*[args.size() + 2];
		argv[0] = (char*)_name.c_str();
		
		for (unsigned int i = 0; i < args.size(); i++)
		{
			argv[i + 1] = (char*)args[i].c_str();
		}
		
		argv[args.size() + 1] = (char*)0;
		
		execvp(argv[0], argv);
		cerr << "Cannot execute unix command: " << argv[0] << " Error: " << strerror(errno);
		return static_cast<int>(Unresolved);
	}
protected:
	string _name;
	ProcessMode _mode;
};

#endif /* UNIX_COMMAND_H */
