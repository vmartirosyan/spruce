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
			int fd = -1;
			if((fd = open(LogFilePath.c_str(), O_RDWR)) == -1)
			{
				throw Exception(string("open log file failed"));
			}
			
			struct stat sb;
			if(fstat(fd, &sb) == -1)
			{
				throw Exception(string("stat failed"));
			}
			
			if(ftruncate(fd, sb.st_size - 9) == -1)
			{
				throw Exception(string("ftruncate failed"));
			}
			
			close(fd);
			
			ofstream of(LogFilePath.c_str(), ios_base::app);
			// First process the possible leaks
			ifstream in_file((DebugFSPath + "/kedr_leak_check/possible_leaks").c_str());
			string line;
			getline(in_file, line);
			while (in_file.good() && !in_file.eof())
			{					
				if ( line.find("Address:") == string::npos )
				{
					getline(in_file, line);
					continue;
				}

				cerr << "Leak checker. Adding item." << endl;
				of << "\t<Item Name=\"PossibleLeak\" Id=\"" << rand() << "\">\n\t\t<Status>Failed</Status><Output>";

				string Data;
				do
				{
					//process line to valid xml
					//cerr << "Line: " << line << endl;
					size_t pos = 0;
					while ( ( pos = line.find("<", pos ) ) != string::npos )
					{
						line = line.replace(pos, 1, "&lt;");
					}
					
					Data += line + "\n";
					
					getline(in_file, line);
				}
				while(line.find("Address:") == string::npos && !in_file.eof());
				
				// Write data
				of << Data << "</Output>\n\t</Item>";				
			}
			in_file.close();	
			
			// Then the unallocated frees will come
			in_file.open((DebugFSPath + "/kedr_leak_check/unallocated_frees").c_str());
			getline(in_file, line);
			while (in_file.good() && !in_file.eof())
			{					
				if ( line.find("Address:") == string::npos )
				{
					getline(in_file, line);
					continue;
				}

				cerr << "Leak checker. Adding item." << endl;
				of << "\t<Item Name=\"UnallocatedFree\" Id=\"" << rand() << "\">\n\t\t<Status>Failed</Status><Output>";

				string Data;
				do
				{
					//process line to valid xml
					//cerr << "Line: " << line << endl;
					size_t pos = 0;
					while ( ( pos = line.find("<", pos ) ) != string::npos )
					{
						line = line.replace(pos, 1, "&lt;");
					}
					
					Data += line + "\n";
					
					getline(in_file, line);
				}
				while(line.find("Address:") == string::npos && !in_file.eof());
				
				// Write data
				of << Data << "</Output>\n\t</Item>";
			}
			in_file.close();
			
			of << "</Module>";
			of.close();
			
		}
		catch(exception& err)
		{
			cerr << "Exception is thrown." << err.what() << endl;
			return false;
		}
		return true;
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
