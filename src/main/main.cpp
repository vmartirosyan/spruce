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

#include <iterator>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <getopt.h>
#include <UnixCommand.hpp>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>

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
using std::ostream_iterator;
using std::back_insert_iterator;


typedef map<string,string> ConfigValues;

ConfigValues ParseOptions(int argc, char ** argv);
ConfigValues ParseConfigFile(string FilePath);


vector<string> ModulesAvailable;
vector<string> FSAvailable;

vector<string> SplitString(string str, char delim, vector<string> AllowedValues );

enum ErrorCodes
{
	SUCCESS,
	NOCFGFILE,
	FAULT,
	NOMODULES
};

int main(int argc, char ** argv)
{
	//Prepare the allowed modules list
	ModulesAvailable.push_back("syscall");
	ModulesAvailable.push_back("benchmark");
	
	// Prepare the allowed FS list
	FSAvailable.push_back("ext4");
	FSAvailable.push_back("btrfs");
	FSAvailable.push_back("xfs");	
	
	// Parse the arguments
	ConfigValues options = ParseOptions(argc, argv);
	ConfigValues configValues;
	
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
	vector<string> Modules = SplitString(configValues["modules"], ';', ModulesAvailable);
	
	// Get the filesystem names to be tested
	vector<string> FileSystems;
	if ( configValues.find("fs") != configValues.end() )
	{
		FileSystems = SplitString(configValues["fs"], ';', FSAvailable);
	}
	else
		FileSystems.push_back("current");
	
	// Find out the partition to be tested on. If there is no partition provided
	// Spruce will use the current partition. 
	// By default tests will be executed in the /tmp folder
	string partition = "current";
	string MountAt = "/tmp/spruce_test";
	vector<string> MountOpts;
	if ( configValues.find("partition") != configValues.end() )
	{
		partition = configValues["partition"];
		if ( configValues.find("mount_at") != configValues.end() )
			MountAt = configValues["mount_at"];
		if ( configValues.find("mount_opts") != configValues.end() )
			MountOpts = SplitString(configValues["mount_opts"], ';', vector<string>());
	}
	else
	{
		cerr << "Warning. No partition name provided. Using /tmp folder." << endl;
		if ( chdir("/tmp") )
		{
			cerr << "Cannot change to /tmp folder. Aborting." << endl;
			return FAULT;
		}
	}
		
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
	args.push_back("${CMAKE_INSTALL_PREFIX}/share/spruce/config/processor.xslt");
	args.push_back(logfolder);
	
	UnixCommand copy("cp");
	if ( copy.Execute(args) == NULL )
	{
		cerr << "Cannot copy the transformation file. Error " << strerror(errno) << endl;
		return FAULT;
	}
		
	// Create the mount point	
	if ( mkdir(MountAt.c_str(), S_IRUSR | S_IWUSR ) == -1 && errno != EEXIST )
	{
		cerr << "Cannot create folder " << MountAt << endl;
		cerr << "Error: " << strerror(errno) << endl;
		return errno;
	}
	
	// Go through the modules, execute them
	// and collect the output
	
	for ( vector<string>::iterator fs = FileSystems.begin(); fs != FileSystems.end(); ++fs )
	{
		stringstream str;
		str << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\
			<?xml-stylesheet type=\"application/xml\" href=\"" << logfolder << "/processor.xslt\"?>\n\
			<SpruceLog>";
			
		
		
		// Unmount the MountAt folder first (just in case)		
		if ( umount( MountAt.c_str() ) != 0 && errno != EINVAL)
		{
			cerr << "Cannot unmount folder " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
			
		// If there are any filesystems mentioned, then let's create the filesystem
		UnixCommand mkfs("mkfs." + *fs);
		vector<string> args;		
		args.push_back(partition);		
		if ( *fs == "xfs" ) //Force if necessary
			args.push_back("-f");
		ProcessResult * res;
		res = mkfs.Execute(args);
		if ( res->GetStatus() != Success )
		{
			cerr << "Cannot create " << *fs << " filesystem on device " << partition << endl;
			cerr << "Error: " << res->GetOutput() << endl;
			continue;
		}
		
		
		
		// Then mount the filesystem
		UnixCommand mnt("mount");
		vector<string> mnt_args;		
		mnt_args.push_back(partition);
		mnt_args.push_back(MountAt);
		
		if ( !MountOpts.empty() )
			mnt_args.insert(mnt_args.end(), MountOpts.begin(), MountOpts.end());
		
		res = mnt.Execute(mnt_args);
		if ( res->GetStatus() != Success )
		{
			cerr << "Cannot mount " << partition << " at folder " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
		/*
		if ( mount(partition.c_str(), MountAt.c_str(), fs->c_str(), 0, 0) != 0 )
		{
			cerr << "Cannot mount " << partition << " at folder " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
		*/
		// Now change current dir to the newly mounted partition folder
		if ( chdir(MountAt.c_str()) != 0 )
		{
			cerr << "Cannot change current dir to " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
		
		
		for (vector<string>::iterator module = Modules.begin(); module != Modules.end(); ++module)
		{
			cerr << "Executing " << *module << " on " << *fs << " filesystem" << endl;
			UnixCommand command(( (string)("${CMAKE_INSTALL_PREFIX}/bin/" + (*module)).c_str()));
			//auto_ptr<ProcessResult> result(command.Execute());
			ProcessResult * result(command.Execute());
			str << "<FS Name=\"" << *fs << "\" />" << result->GetOutput() << endl;
		}
		
		str << "</SpruceLog>";
		// Forward the output to the log file	
		ofstream of((configValues["logfolder"] + "/spruce_log_" + *fs + ".xml").c_str());
		of << str.str();
		of.close();
		
		// Now change current dir to log folder to free the MountAt folder (to unmount later)
		if ( chdir(logfolder.c_str()) != 0 )
		{
			cerr << "Cannot change current dir to " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
		
		// Unmount the MountAt folder first (just in case)		
		if ( umount( MountAt.c_str() ) != 0 && errno != EINVAL)
		{
			cerr << "Cannot unmount folder " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			continue;
		}
		
		// Open the log file in the selected browser
		string browser_args = logfolder + "/spruce_log_" + *fs + ".xml";
		if ( browser == "chrome" )
		{
			browser = "chromium";
			browser_args += " --allow-file-access-from-files --user-data-dir /tmp";
		}
		string command = "bash -c '" + browser + " " + browser_args + " &'";
		system(command.c_str());
	}
	
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

vector<string> SplitString(string str, char delim, vector<string> AllowedValues )
{
	vector<string> pieces;
	size_t PrevPos = 0, CurPos;
	if ( str.find( delim, PrevPos ) == string::npos)
	{
		if ( !str.empty() )
			pieces.push_back(str);
		return pieces;
	}
	
	while ( ( CurPos = str.find( delim, PrevPos ) ) != string::npos )
	{
		string piece = str.substr(PrevPos, CurPos - PrevPos);		
		if ( AllowedValues.empty() || find(AllowedValues.begin(), AllowedValues.end(), piece) != AllowedValues.end() )
			pieces.push_back(piece);
		PrevPos = CurPos + 1;
	}
	// If the last symbol is not a delimiter, then take the remaining string also
	if ( str[str.size() - 1] != delim )
	{
		string piece = str.substr(PrevPos, string::npos);
		if ( !piece.empty() && (AllowedValues.empty() || find(AllowedValues.begin(), AllowedValues.end(), piece) != AllowedValues.end() ) )
			pieces.push_back(piece);
	}
	return pieces;
	
}
