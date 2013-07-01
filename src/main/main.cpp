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
#include <stdlib.h>
#include <PlatformConfig.hpp>
#include <pwd.h>
#include <KedrIntegrator.hpp>
#include <LeakChecker.hpp>
#include <PartitionManager.hpp>
//#include <ShmAllocator.hpp>

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
bool CreateFilesystem(string fs, string partition);

vector<string> ModulesAvailable;
vector<string> FSAvailable;

vector<string> SplitString(string str, char delim, vector<string> AllowedValues );

bool terminate_process = false;

enum ErrorCodes
{
	SUCCESS,
	NOCFGFILE,
	FAULT,
	NOMODULES
};
bool GenerateHtml(string logfolder, string fs);
void OpenDashboard(string browser, string logfolder, string fs);
void SignalHandler(int signum);

int main(int argc, char ** argv)
{
	struct sigaction sa;
	sa.sa_handler = SignalHandler;
	if ((sigaction(SIGINT, &sa, 0) == -1) || (sigaction(SIGQUIT, &sa, 0) == -1) ) // Ctrl+C, Ctrl+4
	{	
		Logger::LogError("Cannot set signal handler.");
		return FAULT;
	}

	try
	{
		KedrIntegrator kedr;
		// The status of the whole process.
		// If any of the tests does not succeed then FAULT is returned!
		Status SpruceStatus = Success;
		Status ModuleStatus = Success;
		
		
		// Log files of the modules
		//vector<string> XMLFilesToProcess;		
		vector<pair<string, string> > MountOptions;
		
		//Prepare the allowed modules list
		//ModulesAvailable.push_back("syscall");
		//ModulesAvailable.push_back("benchmark");
		//ModulesAvailable.push_back("fs-spec");
		//ModulesAvailable.push_back("leak-check");
		//ModulesAvailable.push_back("fault-sim");
		
		// Prepare the allowed FS list
		//FSAvailable.push_back("ext4");
		//FSAvailable.push_back("btrfs");
		//FSAvailable.push_back("xfs");	
		//FSAvailable.push_back("jfs");
		
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
			Logger::LogError("No configuration file is given. Aborting.");
			return NOCFGFILE;
		}
		
		if ( configValues.empty() )
		{
			Logger::LogError("Cannot parse the configuration file. Aborting.");
			return FAULT;
		}

		// Get the filesystem names to be tested
		vector<string> FileSystems;
		if ( configValues.find("fs") != configValues.end() )
		{
			FileSystems = SplitString(configValues["fs"], ';', FSAvailable);
		}
		else
			FileSystems.push_back("current");
		for ( unsigned int i = 0; i < FileSystems.size(); ++i )
			cout << "FS : " << FileSystems[i] << endl;
			
		// Check if the 'packages' key exists in the config file
		vector<string> Packages;
		string strPackages;
		if ( configValues.find("packages") == configValues.end() )
		{
			Logger::LogWarn("No `packages` are provided. Executing both common and fs-specific packages.");
			strPackages = "common;fs-spec";
		}
		else		
		{
			strPackages = configValues["packages"];
		}
		Packages = SplitString(strPackages, ';');
		
		// Get the `checks` value
		vector<string> Checks;
		string strChecks;
		if ( configValues.find("checks") == configValues.end() )
		{
			Logger::LogWarn("No `checks` are provided. Perfoming functional check only.");
			strChecks = "func";
		}
		else		
		{
			strChecks = configValues["checks"];
		}
		Checks = SplitString(strChecks, ';');
		
		string partition = "current";
		string MountAt = "/tmp/spruce_test";
		string MountOpts;
		if ( configValues.find("partition") != configValues.end() )
		{
			partition = configValues["partition"];
			if ( configValues.find("mount_at") != configValues.end() )
				MountAt = configValues["mount_at"];
			if ( configValues.find("mount_opts") != configValues.end() )
			{
				MountOpts = configValues["mount_opts"];
				cout << "Mount options : " << MountOpts << endl;
			}				
		}
		else
		{
			Logger::LogError("No partition name provided. Aborting");
			return FAULT;
		}

		// Where the output must be stored?
		string logfolder = "/tmp/spruce_log/";
		if ( configValues.find("logfolder") != configValues.end() )
		{
			logfolder = configValues["logfolder"];			
		}
		else
		{
			Logger::LogInfo("No log folder specified. Using " + logfolder + ".");
		}
				
		// making the logfolder directory
		if ( mkdir(logfolder.c_str(), 0777) )
		{
			if(errno != EEXIST)
			{
				Logger::LogError("Cannot create logfolder: " + logfolder + ".");
				return FAULT;		
			}	
		} 
		//If the subfolders config value is specified by value=false, then we don't need subfolders, and we will overwrite log file each time.		
		if ( (configValues.find("subfolders") != configValues.end()) && (configValues["subfolders"] == "false" ))
		{
			Logger::LogWarn("No subfolders will be created. The Log file will be overwritten.");
			if(system(("rm -f " + logfolder + "/* > /dev/null 2>&1").c_str()) == -1)
			{
				Logger::LogWarn("Couldn't remove xml files from logfolder. The report may be incomplete.");
			}
		}
		else // by default subfolders are to be created
		{
			// Add the date and time to the log subfolder
			time_t t = time(NULL);
			struct tm * tm = localtime(&t);
			char buf[25];
			size_t bytes = strftime(buf, 25, "%F_%T", tm);
			buf[bytes] = 0;
			logfolder = logfolder + "/" + buf;
			if ( mkdir(logfolder.c_str(), 0777) )
			{
				Logger::LogError("Cannot create log subfolder: " + logfolder + ".");
				return FAULT;
			}
		}	
		
		
		// Generate the main log file. 
		// It should contain the distribution name, version and the kernel version.
		ofstream main_xml((logfolder + "/main.xml").c_str());
		
		string arch = "Unknown";
		string distro = "Unknown";
		string distro_ver = "Unknown";
		string kernel = "Unknown";
		
		UnixCommand uname("uname");
		vector<string> args;
		args.push_back("-m");
		ProcessResult * res = uname.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			Logger::LogWarn("Cannot obtain architecture.");
		}
		else
			arch = res->GetOutput();
		delete res;
		
		args.clear();
		args.push_back("-r");
		res = uname.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			Logger::LogWarn("Cannot obtain kernel version.");
		}
		else
			kernel = res->GetOutput();
		delete res;
		
		UnixCommand lsb_release("lsb_release");		
		args.clear();
		args.push_back("-si");
		res = lsb_release.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			Logger::LogWarn("Cannot obtain distribution name.");
		}
		else
			distro = res->GetOutput();
		delete res;
		
		args.clear();
		args.push_back("-sr");
		res = lsb_release.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			Logger::LogWarn("Cannot obtain distribution version.");
		}
		else
			distro_ver = res->GetOutput();
		delete res;
				
		
		main_xml << "<Main>\n" 
				<< "\t<Distribution>" + distro + "</Distribution>\n"
				<< "\t<DistroVersion>" + distro_ver + "</DistroVersion>\n"
				<< "\t<Arch>" + arch + "</Arch>\n"
				<< "\t<Kernel>" + kernel + "</Kernel>\n"
				<< "</Main>";
				
		main_xml.close();
		
		// Determine the log level
		string loglevel = "warning";
		if ( configValues.find("loglevel") != configValues.end() )
		{
			loglevel = configValues["loglevel"];			
		}
		else
		{
			Logger::LogInfo("No log level specified. Using `" + loglevel + "` as default.");
		}
		LogLevel nLogLevel = Logger::Parse(loglevel);
		if ( LOG_None == nLogLevel)
			nLogLevel = LOG_Warn;
		
		Logger::Init(logfolder + "/spruce.log", nLogLevel);

		// Find out which browser must be used to view the log file
		// If no browser name is specified, then the system will be executed in batch mode.
		string browser = "";
		if ( configValues.find("browser") != configValues.end() )
		{
			browser = configValues["browser"];
		}
		else
		{
			Logger::LogInfo("Notice. No browser specified. Switching to batch mode.");
		}
		
		// A small hack for firefox to overcome a security problem
		// Copy the transformation file to the log folder
		args.clear();
		
		args.push_back("cp");
		args.push_back("-r");
		args.push_back(INSTALL_PREFIX"/share/spruce/config/xslt/");
		args.push_back(logfolder);
		
		UnixCommand * copy = new UnixCommand("cp");
		if ( copy->Execute(args) == NULL )
		{
			Logger::LogError("Cannot copy the transformation file.");
			return FAULT;
		}
		delete copy;
		
		// It is not allowed to provide exclude_tests and run_tests config values at once
		if ( configValues.find("exclude_tests") != configValues.end() &&
			 configValues.find("run_tests") != configValues.end())
		{
			Logger::LogError("Please provide either exclude_tests or run_tests values, not both.");
			return FAULT;
		}
		
		// Get the test names to be excluded
		vector<string> TestsToExclude;		
		if ( configValues.find("exclude_tests") != configValues.end() )
		{			
			TestsToExclude = SplitString(configValues["exclude_tests"], ';', vector<string>());
		}
		
		// Get the test names to run
		vector<string> TestsToRun;		
		if ( configValues.find("run_tests") != configValues.end() )
		{
			TestsToRun = SplitString(configValues["run_tests"], ';', vector<string>());
		}
		
			
		// Create the mount point	
		if ( mkdir(MountAt.c_str(), S_IRUSR | S_IWUSR ) == -1 && errno != EEXIST )
		{
			Logger::LogError("Cannot create folder " + MountAt + ".");
			return errno;
		}
		bool ShowOutput = false;
		
		for ( vector<string>::iterator fs = FileSystems.begin(); fs != FileSystems.end(); ++fs )
		{
			EXIT_IF_SIGNALED;
						
			time_t FSStartTime = time(0);
			cerr << endl << "\033[1;32mFilesystem : " << *fs << "\033[0m" << endl;
			Logger::LogInfo("Filesystem : " + *fs + ".");
			
			// Before executing the modules prepare the environment
			setenv("MountAt", MountAt.c_str(), 1);
			setenv("Partition", partition.c_str(), 1);
			setenv("FileSystem", (*fs).c_str(), 1);
			setenv("INSTALL_PREFIX", INSTALL_PREFIX, 1);
			
			// Needed by the KEDR framework to be able to unload the target module
			// Unmount the MountAt folder first (just in case)		
			if ( umount( MountAt.c_str() ) != 0 && errno != EINVAL)
			{
				Logger::LogError("Cannot unmount folder " + MountAt + ".");
				continue;
			}
			
			vector<string> doer_args;
			
			doer_args.push_back("-f"); // FileSystem
			doer_args.push_back(*fs);
			
			doer_args.push_back("-l"); //Log folder
			doer_args.push_back(logfolder);
			
			doer_args.push_back("-p"); // Packages
			doer_args.push_back(strPackages);
			
			doer_args.push_back("-c"); // Checks
			doer_args.push_back(strChecks);
			
			// Check if some certain tests should be executed
			if ( configValues.find("run_tests") != configValues.end() )
			{
				doer_args.push_back("-r"); // Tests to run
				doer_args.push_back(configValues["run_tests"]);
			}
			else if ( configValues.find("exclude_tests") != configValues.end() )
			{
				doer_args.push_back("-e"); // Tests to exclude
				doer_args.push_back(configValues["exclude_tests"]);
			}
			
			UnixCommand * command = new UnixCommand(INSTALL_PREFIX"/bin/doer", ProcessNoCaptureOutput);			
			
			// Produce the <FS>.xml to pass to the dashboard generator
			// The file contains information about mount options and packages
			string strFSStartTime = ctime(&FSStartTime);
			ofstream fs_xml((logfolder + "/" + *fs + ".xml").c_str());
			fs_xml << "<SpruceDashboard FS=\"" + (string)*fs + "\">\n";
			fs_xml << "\t<Start>" + strFSStartTime + "</Start>\n";			
			fs_xml << "\t<Rev>" HG_REV "</Rev>\n";
			fs_xml.close();
			
			ProcessResult * result = command->Execute(doer_args);
			delete command;
			
			ShowOutput = (result->GetStatus() == Success);
			
			int status = result->GetStatus();
			
			if ( !WIFSIGNALED(status) && (status < Fatal) && !OS_32_BITS && HAVE_MULTILIB )
			{
				UnixCommand * command = new UnixCommand(INSTALL_PREFIX"/bin/doer_32", ProcessNoCaptureOutput);
				
				ProcessResult * result = command->Execute(doer_args);
				delete command;
				
				ShowOutput = (result->GetStatus() == Success);
			}
			
			size_t FSDuration = time(0) - FSStartTime;
			stringstream str;
			str << FSDuration;
			
			fs_xml.open((logfolder + "/" + *fs + ".xml").c_str(), ios_base::app);
			fs_xml << "\t<Duration>" + str.str() + "</Duration>\n";
			
			fs_xml << "\t<Packages>\n";
			for ( size_t i = 0; i < Packages.size(); ++i )
			{
				string PackageName = Packages[i];
				if ( PackageName.find("fs-spec") != string::npos )
					PackageName.replace(0, 7, *fs);
				if ( !OS_32_BITS && HAVE_MULTILIB )
				{
					fs_xml << "\t\t<Package>" + PackageName + "_32</Package>\n";
				}
				fs_xml << "\t\t<Package>" + PackageName + "</Package>\n";		
			}
			// Add the memory leak test package
			if ( strChecks.find(strMemoryLeak) != string::npos )
			{
				if ( !OS_32_BITS && HAVE_MULTILIB )
				{
					fs_xml << "\t\t<Package>MemoryLeaks_32</Package>\n";
				}
				fs_xml << "\t\t<Package>MemoryLeaks</Package>\n";
			}
			fs_xml << "\t</Packages>\n";
			fs_xml << "</SpruceDashboard>";
			
			fs_xml.close();
			
			
			if ( !ShowOutput )
			{
				Logger::LogWarn("\033[1;31mNo log file is generated for " + *fs + ".\033[0m");
				continue;
			}
			
			//if we need htmls, then generating them
			if ( ! ((configValues.find("genhtml") != configValues.end()) && (configValues["genhtml"] == "false")))
			{	
				// Find all the generated XMLs for the current FS
				UnixCommand find("find");
				vector<string> find_args;
				find_args.push_back(logfolder);
				find_args.push_back("-maxdepth"); 
				find_args.push_back("1"); 
				find_args.push_back("-name");
				find_args.push_back( *fs + "*.xml");
				
				
				ProcessResult *res = find.Execute(find_args);
				
				if ( res->GetStatus() != Success || res->GetOutput() == "" )
				{
					Logger::LogError("No XML files were found to process.");
				}
				else
				{
					vector<string> xmls = SplitString(res->GetOutput(), '\n', vector<string>());
				
					
					for (size_t i = 0; i < xmls.size(); ++i)
					{
						string FileName = xmls[i];
					
						UnixCommand xslt("xsltproc");
						vector<string> xslt_args;
						
						xslt_args.push_back("-o");
						xslt_args.push_back(FileName.substr(0, FileName.size() - 3) + "html");
						xslt_args.push_back("-novalid");			
						xslt_args.push_back(logfolder + "/xslt/processor.xslt");
						xslt_args.push_back(FileName);
											
						res = xslt.Execute(xslt_args);

						if ( res == NULL || res->GetStatus() != Success)
						{
							Logger::LogError("Error executing xsltproc." + res->GetOutput());
							continue;
						}
						
					}
				}
			}						
			
			
			// we generate html's if it is in need, also if the browser is specified, open the log files in the selected browser.
			if ( !( (configValues.find("genhtml") != configValues.end() ) && ( configValues["genhtml"] == "false") ) )
			{
				if ( GenerateHtml(logfolder, *fs) && ( browser != "" ) && !terminate_process)
				{
					OpenDashboard(browser, logfolder, *fs);
				}
			}
			//XMLFilesToProcess.erase(XMLFilesToProcess.begin(), XMLFilesToProcess.end());
			if (  ModuleStatus >= Fatal )
			{
				Logger::LogError("Fatal error has rised. Quiting.");
				break;
			}
		}
		return ( SpruceStatus == 0 ) ? SUCCESS : FAULT;
	}
	catch (Exception e)
	{
		Logger::LogError("Exception is thrown: " + e.GetMessage());
		return FAULT;
	}
	//return 0;
}


bool GenerateHtml(string logfolder, string fs)
{
	if ( access((logfolder + +"/" + fs + ".xml").c_str(), F_OK) != 0 )
	{
		Logger::LogError("No Fs xml file is generated.");
		return false;
	}
	ProcessResult * res = NULL;
	// Process the dashboard file
	// cout << "Processing file " << fs << ".xml ...";
	UnixCommand xslt("xsltproc");
	vector<string> xslt_args;
	
	xslt_args.push_back("-o");
	xslt_args.push_back(logfolder + "/" + fs + ".html");
	xslt_args.push_back("-novalid");			
	xslt_args.push_back("--stringparam");
	xslt_args.push_back("LogFolder");
	xslt_args.push_back(logfolder + "/");
	xslt_args.push_back(logfolder + "/xslt/dashboard.xslt");
	xslt_args.push_back(logfolder + "/" + fs + ".xml");
	
	res = xslt.Execute(xslt_args);

	if ( res->GetStatus() != Success )
	{
		Logger::LogError( "Cannot open browser: " + res->GetOutput());
		return false;
	}
	return true;
}

void OpenDashboard(string browser, string logfolder, string fs)
{
					
	// The new process must be created to be able to execute the browser as non-privileged user.
	int fork_ret = fork();
	if ( fork_ret == -1 )
	{
		Logger::LogError("Cannot create process for opening Dashboard in browser.");
		return;
	}				
	if(fork_ret == 0)
	{	
		
		// Change the effective user id to real user id... browsers don't like running as root.
		char * UserName = getenv("SUDO_USER");
		//char * UserName = "nobody";
		
		if ( UserName == NULL )
		{
			Logger::LogError("Cannot obtain user name.");
			return;
		}
		
		setenv("HOME", (static_cast<string>("/home/") + UserName).c_str(), 1);
		
		struct passwd * nobody = getpwnam(UserName);
		if ( nobody == NULL )
		{
			Logger::LogError("Cannot switch to user `" + (string)UserName + "`. Browser won't start.");
			return;
		}
			
		if ( setuid(nobody->pw_uid) == -1 )
		{
			Logger::LogError("Cannot switch to user `" + (string)UserName + "`. Browser won't start.");
			return;
		}
		
		UnixCommand * browser_cmd = new UnixCommand(browser, ProcessBackground);
		vector<string> browser_args;
		browser_args.push_back(logfolder + "/" + fs + ".html");
		
		ProcessResult * res = browser_cmd->Execute(browser_args);
		delete browser_cmd;
		
		if ( res == NULL )
		{
			Logger::LogError("Cannot execute the browser: " + browser + ".");
			_exit(1);
		}
		if ( res->GetStatus() != Success )
		{
			Logger::LogError("Cannot execute the browser: " + browser + ". " + res->GetOutput() + ".");
			_exit(1);
		}
		_exit(0);
	}
	else
	{
		wait(0);
	}
	
	
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
			optname.insert(0, 1, static_cast<char>(opt));
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
			Logger::LogError("Cannot open configuration file " + FilePath + ".");
			return vals;
		}
		while ( !FileReader.eof() && FileReader.good() )
		{			
			
			char buf[256];
			//FileReader >> CurrentLine;
			FileReader.getline(buf, 256);
			string CurrentLine(buf);
			
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
		Logger::LogError("Error occured while reading configuration file " + FilePath);
	}
	return vals;
}


void SignalHandler(int signum)
{
	if(!terminate_process)
		Logger::LogError("Spruce receive INTERRUPT signal. Preparing operations, please wait...");// Spruce PID = " << getpid()<<endl;
	terminate_process = true;
}
