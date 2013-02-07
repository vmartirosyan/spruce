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
#include <XmlSpruceLog.hpp>

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

	Item ProcessLeakCheckerOutput(string fs)
	{
		Item item("Possible leaks, Unallocated frees", "");
		try
		{			
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

				item.setStatus("Failed");
				
				//str += "\t<Item Name=\"PossibleLeak\" Id=\"" + 123 + "\">\n\t\t<Status>Failed</Status><Output>";
			}
			else
			{
				item.setStatus("Success");
				//of << "\t<Item Name=\"PossibleLeak\" Id=\"" << rand() << "\">\n\t\t<Status>Success</Status><Output>";
			}

			UnixCommand cat("cat");
			vector<string> catArgs;
			catArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/info");
			ProcessResult * res = cat.Execute(catArgs);
			if (res->GetStatus() == Success)
			{
				item.appendOutput(res->GetOutput());
			}
			//string str = res.GetOutput();
			//string num = str.substr(string("Possible leaks: ").length(), str.length() - string("Possible leaks: ").length());

			if(resPosLeaks->GetStatus() == 0) //
			{
				catArgs.clear();
				catArgs.push_back(DebugFSPath + string("/kedr_leak_check/") + fs + "/possible_leaks");
				delete res;
				res = cat.Execute(catArgs);
				
				if(res->GetStatus() == Success)
				{
					item.appendOutput(res->GetOutput());
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
					item.appendOutput(resUnallocFrees->GetOutput());
				}
				else
				{
				}
			}
			//of << "</Output></Item></Module>" << endl;
			
			//of.close();
			
		}
		catch(exception& err)
		{
			Logger::LogError(static_cast<string>("Exception is thrown. ") + err.what());
			item.setStatus("Unresolved");
			item.setOutput(err.what());
			return item;
		}
		
		return item;
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
