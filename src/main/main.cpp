//      main.cpp
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


// The main executable of the Spruce system. 
// Parses the configuration passed to it, 
// executes the tests depending on the configuration values
// collects the output and visualizes it.

#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <UnixCommand.hpp>
#include <string.h>
#include <errno.h>

using std::ifstream;		
using std::ofstream; 		
using std::map;			
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::auto_ptr;
using std::stringstream;


typedef map<string,string> ConfigValues;

ConfigValues ParseOptions(int argc, char ** argv);
ConfigValues ParseConfigFile(string FilePath);

const int ModuleCount = 2;
string ModulesAvailable[] = {"benchmark", "syscall"};

enum ErrorCodes
{
	SUCCESS,
	NOCFGFILE,
	FAULT,
	NOMODULES
};

int main(int argc, char ** argv)
{
	// Parse the arguments
	ConfigValues options = ParseOptions(argc, argv);
	ConfigValues configValues;
	for ( ConfigValues::const_iterator i = options.begin(); i != options.end(); ++i )
		cout << i->first << "=" << i->second << endl;;
	
	// Parse the configuration file
	ConfigValues::const_iterator configFile;
	if ( ( configFile = options.find("c")) != options.end() )
	{
		configValues = ParseConfigFile(configFile->second);
	}
	else
	{
		cerr << "No configuration file is given. Aborting" << endl;
		return NOCFGFILE;
	}
	
	if ( configValues.empty() )
	{
		cerr << "Cannot parse the configuration file. Aborting" << endl;
		return FAULT;
	}	
	// Execute the tests
	// Check if the modules key exists in the config file
	if ( configValues.find("modules") == configValues.end() )
	{
		cerr << "Cannot find modules list to be executed. Aborting." << endl;
		return NOMODULES;
	}
	// Split the module list into module names
	size_t SplitterPos = 0, OldPos = 0;
	vector<string> Modules;
	while (true)	
	{
		SplitterPos = configValues["modules"].find(';', OldPos);
		if ( SplitterPos == string::npos )
		{
			break;
		}
		string ModuleName = configValues["modules"].substr(OldPos, SplitterPos - OldPos );
		cout << "Module name: " << ModuleName << endl;
		OldPos = SplitterPos + 1;
		if ( find(&ModulesAvailable[0], &ModulesAvailable[ModuleCount], ModuleName) == ModulesAvailable + ModuleCount)
		{
			cerr << "Warning. Unknown module " << ModuleName << endl;
			continue;
		}
		Modules.push_back(ModuleName);
	}
	string ModuleName = configValues["modules"].substr(OldPos, configValues["modules"].size() - 1 );
	cout << "Module name: " << ModuleName << endl;	
	if ( find(ModulesAvailable, ModulesAvailable + ModuleCount, ModuleName) == ModulesAvailable + ModuleCount)
	{
		cerr << "Warning. Unknown module " << ModuleName << endl;		
	}
	else	
		Modules.push_back(ModuleName);
		
	// Find out the partition to be tested on. If there is no partition provided
	// Spruce will use the current partition. 
	// By default tests will be executed in the /tmp folder
	/*if ( configValues.find("partition") != configValues.end() )
	{
		string partition = configValues.find("partition");
	}
	else
	{
		cerr << "Warning. No partition name provided. Using /tmp folder."
		if ( chdir("/tmp") )
		{
			cerr << "Cannot change to /tmp folder. Aborting." << endl;
			return FAULT;
		}
	}*/
		
	// Where the output must be stored?
	string logfolder = "/tmp";
	if ( configValues.find("logfolder") != configValues.end() )
	{
		logfolder = configValues["logfolder"];
	}
	else
	{
		cerr << "Notice. No log folder specified. Using " << logfolder << "." << endl;
	}

	// Find out which browser must be used to view the log file
	string browser = "firefox";
	if ( configValues.find("browser") != configValues.end() )
	{
		browser = configValues["browser"];
	}
	else
	{
		cerr << "Notice. No browser specified. Using " << browser << " as default." << endl;
	}
	
	// A small hack for firefox to overcome a security problem
	// Copy the transformation file to the log folder	
	vector<string> args;
	
	args.push_back("cp");
	args.push_back("${CMAKE_INSTALL_PREFIX}config/processor.xslt");
	args.push_back(logfolder);
	
	UnixCommand copy("cp");
	if ( copy.Execute(args) == NULL )
	{
		cerr << "Cannot copy the transformation file. Error " << strerror(errno) << endl;
		return FAULT;
	}
			
	
	// Go through the modules, execute them
	// and collect the output
	stringstream str;
	str << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\
			<?xml-stylesheet type=\"application/xml\" href=\"" << logfolder << "/processor.xslt\"?>\n\
			<SpruceLog>";
			
	for (vector<string>::iterator i = Modules.begin(); i != Modules.end(); ++i)
	{
		UnixCommand command(( (string)("${CMAKE_INSTALL_PREFIX}bin/" + (*i)).c_str()));
		//auto_ptr<ProcessResult> result(command.Execute());
		ProcessResult * result(command.Execute());
		str << result->GetOutput() << endl;
	}
	str << "</SpruceLog>";
	// Forward the output to the log file	
	ofstream of((configValues["logfolder"] + "/spruce_log.xml").c_str());
	of << str.str();
	of.close();
	
	// Open the log file in the selected browser
	string browser_args = logfolder + "/spruce_log.xml";
	if ( browser == "chrome" )
	{
		browser = "chromium-browser";
		browser_args += " --allow-file-access-from-files --user-data-dir /tmp";
	}
	string command = "bash -c '" + browser + " " + browser_args + " &'";
	system(command.c_str());
	
	
	return 0;
}

ConfigValues ParseOptions(int argc, char ** argv)
{
	ConfigValues vals;
	static const char *optString = "f:c:vh?";
	int opt;
	do
	{
		opt = getopt(argc, argv, optString);
		if ( opt != -1 )
		{
			string optname;
			optname.insert(0, 1, (char)opt);
			vals[optname] = optarg ? optarg : "";
		}
	} 
	while (opt != -1);
	return vals;
}

ConfigValues ParseConfigFile(string FilePath)
{
	ConfigValues vals;
	cout << "Parsing configuration file " << FilePath << endl;
	try
	{
		ifstream FileReader(FilePath.c_str());
		if ( FileReader.fail() )
		{
			cerr << "Cannot open configuration file " << FilePath << endl;				
			return vals;
		}
		while ( !FileReader.eof() && FileReader.good() )
		{
			string CurrentLine;
			FileReader >> CurrentLine;
			size_t EqPos = CurrentLine.find('=');
			if ( EqPos == string::npos ) // wrong formatted line?
				continue;
			string name = CurrentLine.substr(0, EqPos);				
			string val = CurrentLine.substr(EqPos + 1, -1);
			vals[name] = val;
		}
	}
	catch (...)
	{
		cerr << "Error occured while reading configuration file " << FilePath << endl;
	}
	return vals;
}

vector<string> SplitString(string str, char delim)
{
	vector<string> pieces;
	size_t PrevPos = 0, CurPos;
	while ( ( CurPos = str.find( delim, PrevPos ) ) != string::npos )
	{
		pieces.push_back(str.substr(PrevPos, CurPos - PrevPos));
		PrevPos = CurPos;
	}
	// If the last symbol is not a delimiter, then take the remaining string also
	if ( str[str.size() - 1] != delim )
		pieces.push_back(str.substr(PrevPos, string::npos));
	return pieces;
	
}

