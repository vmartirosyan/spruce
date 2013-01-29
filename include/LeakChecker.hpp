//      leak_checker.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
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

#include <Exception.hpp>
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
	void ProcessLeakCheckerOutput(string fs)
	{
		//int status = Success;
		try
		{
			/*
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
			
			close(fd);*/
			
			ofstream of(LogFilePath.c_str(), ios_base::app);
			
			UnixCommand grep("grep");
			vector<string> grepArgs;
			
			grepArgs.push_back("-E");
			grepArgs.push_back("Possible leaks:[[:space:]]+[1-9][0-9]*");
			grepArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/info");

			ProcessResult * resPosLeaks = grep.Execute(grepArgs);
			
			grepArgs.clear();
			grepArgs.push_back("-E");
			grepArgs.push_back("Unallocated frees:[[:space:]]+[1-9][0-9]*");
			grepArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/info");
			ProcessResult * resUnallocFrees = grep.Execute(grepArgs);
			
			if(resPosLeaks->GetStatus() == 0 || resUnallocFrees->GetStatus() == 0)
			{
				of << "\t<Item Name=\"PossibleLeak\" Id=\"" << rand() << "\">\n\t\t<Status>Failed</Status><Output>";
			}
			else
			{
				of << "\t<Item Name=\"PossibleLeak\" Id=\"" << rand() << "\">\n\t\t<Status>Success</Status><Output>";		
			}

			UnixCommand cat("cat");
			vector<string> catArgs;
			catArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/info");
			ProcessResult * res = cat.Execute(catArgs);
			if (res->GetStatus() == Success)
			{
				string resOut = res->GetOutput();
				processXml(resOut);
				of << resOut << endl;
			}
			//string str = res.GetOutput();
			//string num = str.substr(string("Possible leaks: ").length(), str.length() - string("Possible leaks: ").length());

			if(resPosLeaks->GetStatus() == 0) //
			{
				catArgs.clear();
				catArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/possible_leaks");
				delete res;
				res = cat.Execute(catArgs);
				string resOut = res->GetOutput();
				processXml(resOut);
				of << resOut << endl;
				
				if(res->GetStatus() == Success)
				{
					string resOut = res->GetOutput();
					processXml(resOut);
					of << resOut << "\n";
				}
				else
				{
					
				}
			}
			

			if(resUnallocFrees->GetStatus() == 0)
			{
				catArgs.clear();
				catArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/unallocated_frees");
				delete res;
				res = cat.Execute(catArgs);
				if(resUnallocFrees->GetStatus() == Success)
				{
					string resOut = resUnallocFrees->GetOutput();
					processXml(resOut);
					of << resOut << "\n";
				}
				else
				{
				}
			}
			of << "</Output></Item></Module>" << endl;
			
			of.close();
			
		}
		catch(exception& err)
		{
			cerr << "Exception is thrown." << err.what() << endl;
			return;
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
	
	void replaceXmlEl(string &str, string val1, string val2)
	{
		size_t pos = -1;
		while ( true )
		{
			pos = str.find(val1.c_str(), pos + 1);
			if ( pos == string::npos )
				break;
				//cout<<"first  "<<pos<<endl;
			str.replace(pos, 1, val2.c_str());
		}	
	}
	void processXml(string &str)
	{
		replaceXmlEl(str, "&", "&amp;");
		replaceXmlEl(str, "<", "&lt;");
		replaceXmlEl(str, ">", "&gt;");
		replaceXmlEl(str, "'", "&apos;");
		replaceXmlEl(str, "\"", "&quot;");
		
	}
};
