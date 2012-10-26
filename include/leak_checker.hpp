//      leak_checker.hpp
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

#include <exception.hpp>
#include <UnixCommand.hpp>
#include <sys/utsname.h>
#include <algorithm>

using namespace std;

// A class which processes KEDR leak check output
class LeakChecker
{
public:
	LeakChecker(string log_file = ""):
		DebugFSPath(""),
		LogFilePath(log_file)
	{
		MountDebugFS();
	}
	bool ProcessLeakCheckerOutput()
	{
		try
		{
			ofstream of(LogFilePath.c_str(), ios_base::app);
			of << "<Module Name=\"leak_checker\">";
			// First process the possible leaks
			ifstream in_file((DebugFSPath + "/kedr_leak_check/possible_leaks").c_str());
						
			while (in_file.good())
			{
				string line;
				getline(in_file, line);
				
				if ( line.find("Block at") == string::npos )
					continue;
				string Data = line + "\n";
				while ( line.find("------") == string::npos )
				{
					cerr << "Line: " << line << endl;	
					getline(in_file, line);
					// Process the line to make it valid XML...
					size_t pos = 0;
					while ( ( pos = line.find("<", pos ) ) != string::npos )
					{
						line = line.replace(pos, 1, "&lt;");
					}
					
					Data += line + "\n";
				}
				cerr << "Leak checker. Adding item." << endl;
				
				of << "\t<Item Name=\"PossibleLeak\" Id=\"" << rand() << "\">\n\t\t<Status>Possibe Leak</Status><Output>" << Data << "</Output>\n\t</Item>";
			}
			in_file.close();
			
			
			// Then the unallocated frees will come
			in_file.open((DebugFSPath + "/kedr_leak_check/unallocated_frees").c_str());
						
			while (in_file.good())
			{
				string line;
				getline(in_file, line);
				
				if ( line.find("Block at") == string::npos )
					continue;
				string Data = line + "\n";
				while ( line.find("------") == string::npos )
				{
					cerr << "Line: " << line << endl;	
					getline(in_file, line);
					// Process the line to make it valid XML...
					size_t pos = 0;
					while ( ( pos = line.find("<", pos ) ) != string::npos )
					{
						line = line.replace(pos, 1, "&lt;");
					}
					
					Data += line + "\n";
				}
				cerr << "Leak checker. Adding item." << endl;
				
				of << "\t<Item Name=\"UnallocatedFrees\" Id=\"" << rand() << "\">\n\t\t<Status>Unallocated Free</Status><Output>" << Data << "</Output>\n\t</Item>";
			}
			in_file.close();
			
			of << "</Module>";
			of.close();
			return true;
		}
		catch(exception& err)
		{
			cerr << "Exception is thrown." << err.what() << endl;
			return false;
		}
	}
	
protected:
	string DebugFSPath;
	string LogFilePath;
	
	bool MountDebugFS()
	{
		// Is DebugFS already mounted?
		UnixCommand mount("mount");
		vector<string> args;
		args.push_back("-t");
		args.push_back("debugfs");
				
		ProcessResult * res = mount.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
			throw Exception("Error executing mount. " + (res ? res->GetOutput() : ""));
			
		string Output = res->GetOutput();
		if ( Output != "" )
		{
			// Read the path
			int SpacePos = Output.find(' ', 9);
			DebugFSPath = Output.substr(8, SpacePos - 8);
			cout << "DebugFS path : " << DebugFSPath << endl;
			return true;
		}
			
		// Try to mount...
		args.push_back(DebugFSPath);
		res = mount.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
			throw Exception("Error executing mount. " + (res ? res->GetOutput() : ""));
			
		return true;
	}
};
