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

enum ErrorCodes
{
	SUCCESS,
	NOCFGFILE,
	FAULT,
	NOMODULES
};

void OpenLogFiles(string browser, string logfolder, vector<string> XMLFilesToProcess);
void OpenDashboard(string browser, string logfolder, string fs);

int main(int argc, char ** argv)
{
	try
	{
		KedrIntegrator kedr;
		// The status of the whole process.
		// If any of the tests does not succeed then FAULT is returned!
		Status SpruceStatus = Success;
		Status ModuleStatus = Success;
		
		
		// Log files of the modules
		//vector<string> XMLFilesToProcess;
		vector<string> MountOptions;
		
		//Prepare the allowed modules list
		ModulesAvailable.push_back("syscall");
		//ModulesAvailable.push_back("benchmark");
		ModulesAvailable.push_back("fs-spec");
		ModulesAvailable.push_back("leak-check");
		ModulesAvailable.push_back("fault-sim");
		
		// Prepare the allowed FS list
		FSAvailable.push_back("ext4");
		FSAvailable.push_back("btrfs");
		FSAvailable.push_back("xfs");	
		FSAvailable.push_back("jfs");
		
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
			
		// Check if the modules key exists in the config file
		if ( configValues.find("modules") == configValues.end() )
		{
			cerr << "Cannot find modules list to be executed. Aborting." << endl;
			return NOMODULES;
		}
		// Split the module list into module names
		vector<string> Modules = SplitString(configValues["modules"], ';', ModulesAvailable);
		/*
		for ( int i = 0; i < Modules.size(); ++i )
			cerr << Modules[i] << " ";
		cerr << endl;*/
		
		// Search for the special modules ("fs-spec", "leak-check", "fault-sim" )
		//bool PerformFS_SpecificTests = false;
		bool PerformLeakCheck = false;
		bool PerformFaultSimulation = false;
		vector<string>::iterator it = Modules.end();
#if OS_32_BITS==0 && HAVE_MULTILIB
		if ( (it = find(Modules.begin(), Modules.end(), "fs-spec")) != Modules.end() )
		{
			cerr << "FS-specific module is enabled" << endl;
			Modules.erase(it);
			Modules.push_back("fs-spec_32");
			Modules.push_back("fs-spec_64");
		}
#endif
		if ( (it = find(Modules.begin(), Modules.end(), "leak-check")) != Modules.end() )
		{
			if ( KEDR_INSTALLED )
			{	
				cerr << "Leak checker module is enabled" << endl;
				PerformLeakCheck = true;
				Modules.erase(it);
			}
			else
			{
				cerr << "Leak checker module cannot be enabled because the KEDR framework is not available." << endl;
				PerformLeakCheck = false;
				Modules.erase(it);
			}
		}		
		if ( (it = find(Modules.begin(), Modules.end(), "fault-sim")) != Modules.end() )
		{
			if ( KEDR_INSTALLED )
			{	
				cerr << "Fault simulation module is enabled" << endl;
				PerformFaultSimulation = true;
				*it = "fault_sim";
			}
			else
			{
				cerr << "Fault simulation module cannot be enabled because the KEDR framework is not available." << endl;
				PerformFaultSimulation = false;
				Modules.erase(it);
			}
		}
#if OS_32_BITS==0 && HAVE_MULTILIB
		if ( (it = find(Modules.begin(), Modules.end(), "syscall")) != Modules.end() )
		{
			cerr << "Replacing syscall module with pair of <syscall_32, syscall_64> modules" << endl;
			Modules.erase(it);
			Modules.push_back("syscall_32");
			Modules.push_back("syscall_64");
		}
#endif

		
		
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
			cerr << "Error. No partition name provided." << endl;
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
			cerr << "Notice. No log folder specified. Using " << logfolder << "." << endl;
		}
				
		// making the logfolder directory
		if ( mkdir(logfolder.c_str(), 0777) )
		{
			if(errno != EEXIST)
			{
				cerr << "Cannot create logfolder: " << logfolder << ". " << strerror(errno) << endl;
				return FAULT;		
			}	
		} 
		//If the subfolders config value is specified by value=false, then we don't need subfolders, and we will overwrite log file each time.		
		if ( (configValues.find("subfolders") != configValues.end()) && (configValues["subfolders"] == "false" ))
		{
			cerr << "Notice. No subfolders will be created. The Log file will be overwritten." << endl;
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
				cerr << "Cannot create log subfolder: " << logfolder << ". " << strerror(errno) << endl;
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
			cerr << "Cannot obtain architecture." << endl;
		}
		else
			arch = res->GetOutput();
		delete res;
		
		args.clear();
		args.push_back("-r");
		res = uname.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			cerr << "Cannot obtain kernel version." << endl;
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
			cerr << "Cannot obtain distribution name." << endl;
		}
		else
			distro = res->GetOutput();
		delete res;
		
		args.clear();
		args.push_back("-sr");
		res = lsb_release.Execute(args);
		if ( res == NULL || res->GetStatus() )
		{
			cerr << "Cannot obtain distribution version." << endl;
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
			cerr << "Notice. No log level specified. Using `" << loglevel << "` as default." << endl;
		}
		LogLevel nLogLevel = Logger::Parse(loglevel);
		if ( LOG_None == nLogLevel)
			nLogLevel = LOG_Warn;
		cerr << "Logger::Init(" << logfolder + "/spruce.log" << "," << nLogLevel << ")" << endl; 
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
			cerr << "Notice. No browser specified. Switching to batch mode." << endl;
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
			cerr << "Cannot copy the transformation file. Error " << strerror(errno) << endl;
			return FAULT;
		}
		delete copy;
		
		// It is not allowed to provide exclude_tests and run_tests config values at once
		if ( configValues.find("exclude_tests") != configValues.end() &&
			 configValues.find("run_tests") != configValues.end())
		{
			cerr << "Please provide either exclude_tests or run_tests values, not both." << endl;
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
			cerr << "Cannot create folder " << MountAt << endl;
			cerr << "Error: " << strerror(errno) << endl;
			return errno;
		}
		bool ShowOutput = false;
		
		// Go through the modules, execute them
		// and collect the output
		//cerr << "Executing modules." << endl;
		for ( vector<string>::iterator fs = FileSystems.begin(); fs != FileSystems.end(); ++fs )
		{
			time_t FSStartTime = time(0);
			cerr << endl << "\033[1;32mFilesystem : " << *fs << "\033[0m" << endl;						
			
			// Before executing the modules prepare the environment
			setenv("MountAt", MountAt.c_str(), 1);
			setenv("Partition", partition.c_str(), 1);
			setenv("FileSystem", (*fs).c_str(), 1);
			setenv("INSTALL_PREFIX", INSTALL_PREFIX, 1);
			
			// Needed by the KEDR framework to be able to unload the target module
			// Unmount the MountAt folder first (just in case)		
			if ( umount( MountAt.c_str() ) != 0 && errno != EINVAL)
			{
				cerr << "Cannot unmount folder " << MountAt << endl;
				cerr << "Error: " << strerror(errno) << endl;
				continue;
			}			
			
			// 
			Item lcItem("Possible leaks, Unallocated frees");
			
			// Check if KEDR needs to be loaded			
			if ( PerformLeakCheck || PerformFaultSimulation )
			{
				try
				{
					cout << "Loading KEDR framework for module : " << *fs << endl;
					kedr.SetTargetModule(*fs);
					if ( PerformLeakCheck )
						kedr.EnableMemLeakCheck();
					if ( PerformFaultSimulation )
						kedr.EnableFaultSimulation();
					kedr.LoadKEDR();
					cout << "KEDR is successfully loaded." << endl;
					
				}
				catch (Exception e)
				{
					cerr << "KEDR cannot be loaded." << endl;
					cerr << "Exception is thrown. " << e.GetMessage() << endl;
					PerformFaultSimulation = false;
					PerformLeakCheck = false;
					lcItem.setStatus("Unresolved");
					lcItem.setOutput(string("KEDR cannot be loaded: ") + e.GetMessage());
				}
			}
			//PartitionManager * pm = ShmAllocator<PartitionManager>::GetInstance();
			PartitionManager pm(INSTALL_PREFIX"/share/spruce/config/PartitionManager.cfg", partition, MountAt, *fs, MountOpts);
			
			for (vector<string>::iterator module = Modules.begin(); module != Modules.end() && ModuleStatus < Fatal; ++module)
			{			
				string ModuleBin = (module->find("fs-spec") == string::npos ? *module : (*fs + module->substr(7, module->size())));
				if ( *module == "fault-sim" )
					ModuleBin = "fault_sim";
				if ( ModuleBin == "fault_sim" && !PerformFaultSimulation )
				{
					cerr << "KEDR was not loaded. Skipping fault simulation module." << endl;
					continue;
				}
								
				PartitionStatus PS = PS_Done;
				do
				{
					MountOptions.push_back(pm.GetCurrentOptions());
					
					vector<string> mkfsArgs;
					if((*fs).compare("ext4") == 0)
						mkfsArgs.push_back("-O mmp");
					
					PS = pm.PreparePartition();
					
					cerr << "Current options:  " << pm.GetCurrentOptions(false) << endl;
					
					if ( PS == PS_Fatal  )
					{
						cerr << "Fatal error raised while preparing partition." << endl;
						return FAULT;
					}
					if ( PS == PS_Skip )
					{
						cerr << "\t\tSkipping case." << endl;
						continue;
					}
					if ( PS == PS_Done  )
					{
						Logger::LogInfo("End of mount options.");
						break;
					}
					ShowOutput = true;
					
					
					
					// Check if the module should be executed on this FS with these mount options.
					bool ModuleShouldNotRun = (TestsToRun.size() > 0);
					for ( vector<string>::iterator it = TestsToRun.begin(); it != TestsToRun.end(); ++it )
					{
						string prefix = *fs + "." + pm.GetCurrentOptions() + "." + *module;
						if ( (*it).find(prefix) != string::npos )
						{							
							ModuleShouldNotRun = false;
						}
					}
					if ( ModuleShouldNotRun )
					{
						cerr << "Skipping module: " << *module << " (accoring to key run_tests in configuration file)."	<< endl;
						continue;
					}
					UnixCommand * command = new UnixCommand((INSTALL_PREFIX"/bin/" + ModuleBin).c_str());
					
					string FileName = logfolder + "/" + *fs + "_" + *module + "_" + pm.GetCurrentOptions() + "_log.xml";
					
					// Ensure the file is removed.
					unlink(FileName.c_str());
					
					vector<string> module_args;
					
					SpruceLog xmlSpruceLog(FileName, *fs, pm.GetCurrentOptions(), "", *module);
					xmlSpruceLog.openTags();
					ModuleLog moduleLog(FileName);
					
					
					module_args.push_back(FileName);
					//XMLFilesToProcess.push_back(FileName);
					
					// Find out which tests should be excluded in current module
					vector<string> ExcludeModuleTests;					
					for ( vector<string>::iterator it = TestsToExclude.begin(); it != TestsToExclude.end(); ++it )
					{
						string prefix = *fs + "." + pm.GetCurrentOptions() + "." + *module;
						if ( (*it).find(prefix) != string::npos )
						{
							ExcludeModuleTests.push_back(it->substr(prefix.size() + 1, it->size() - prefix.size()));
						}
					}
					
					if ( ExcludeModuleTests.size() > 0 )
					{
						module_args.push_back("-exclude");
						for ( vector<string>::iterator it = ExcludeModuleTests.begin(); it != ExcludeModuleTests.end(); ++it)
							module_args.push_back(*it);
					}
					
					// Find out which tests should run in current module
					vector<string> RunModuleTests;					
					for ( vector<string>::iterator it = TestsToRun.begin(); it != TestsToRun.end(); ++it )
					{
						string prefix = *fs + "." + pm.GetCurrentOptions() + "." + *module;
						
						if ( (*it).find(prefix) != string::npos )
						{							
							RunModuleTests.push_back(it->substr(prefix.size() + 1, it->size() - prefix.size()));							
						}
					}
					
					if ( RunModuleTests.size() > 0 )
					{
						module_args.push_back("-run");
						for ( vector<string>::iterator it = RunModuleTests.begin(); it != RunModuleTests.end(); ++it)
						{							
							module_args.push_back(*it);
						}
					}
					
					cerr << "Executing " << *module << " on " << *fs << " filesystem" << endl;
					
					time_t ItemStartTime = time(0);
					ProcessResult * result = command->Execute(module_args);
					delete command;
					
					size_t ItemDuration = time(0) - ItemStartTime;
					stringstream str;
					str << ItemDuration;
					
					xmlSpruceLog.setDuration(str.str());
					
					// Process the memory leak checker output
					if ( PerformLeakCheck && kedr.IsRunning() )
					{						
						string relPartOutput = "";
						string kedrUnloadModOutput = "";
						if(PartitionManager:: ReleasePartition(MountAt, &relPartOutput) && kedr.UnloadModule(*fs, &kedrUnloadModOutput)) 
						{
							LeakChecker leak_check(FileName);
							lcItem = leak_check.ProcessLeakCheckerOutput(*fs);
						}
						else
						{
							lcItem.setStatus("Unresolved");
							if(relPartOutput != "")
							{
								lcItem.appendOutput(relPartOutput + "\n");
							}
							if(kedrUnloadModOutput != "")
							{
								lcItem.appendOutput(kedrUnloadModOutput + "\n");
							}
						}
						
					}
					
					moduleLog.addItem(lcItem);
					xmlSpruceLog.closeTags();
					UnixCommand xslt("xsltproc");
					vector<string> xslt_args;
					
					xslt_args.push_back("-o");
					xslt_args.push_back(FileName.substr(0, FileName.size() - 3) + "html");
					xslt_args.push_back("-novalid");			
					xslt_args.push_back(logfolder + "/xslt/processor.xslt");
					xslt_args.push_back(FileName);
										
					ProcessResult *res = xslt.Execute(xslt_args);

					if ( res == NULL )
					{
						cerr << "Error executing xsltproc. Error: " << strerror(errno) << endl;						
					}
					
					if ( res->GetStatus() != Success )
					{
						cerr << res->GetOutput() << endl;
					}			
					
					ModuleStatus = result->GetStatus();					
					
					SpruceStatus = static_cast<Status>(ModuleStatus | SpruceStatus);
					cerr << "Module " << *module << " exits with status `" << StatusMessages[ModuleStatus] << "`" << endl;
					if ( ModuleStatus >= Fatal )
					{
						cerr << "\033[1;31mSpruce: Fatal error has rised. Stopping system execution.\033[0m" << endl;						
						break;
					}
					cout << endl;
					
				}
				while ( PS != PS_Done );
				
				
				
				/*if ( !pm.ReleasePartition() )
				{
					cerr << "Cannot release the partition: " << strerror(errno) << endl;
					break;
				}*/
				
				//ShmAllocator<PartitionManager>::Free(pm);
				pm.ClearCurrentOptions();
			}

			// Unload the KEDR framework
			try
			{
				if ( ( PerformFaultSimulation || PerformLeakCheck ) && kedr.IsRunning() )
				{
					if(kedr.UnloadKEDR())
						cout << "KEDR is successfully unloaded." << endl;
				}
			}
			catch(Exception e)
			{
				cerr << "Error unloading KEDR. " << e.GetMessage() << endl;
			}
			if ( !ShowOutput )
			{
				cerr << "\033[1;31mNo log file is generated for " << *fs << ".\033[0m" << endl;
				continue;
			}
			// Produce the <FS>.xml to pass to the dashboard generator
			// The file contains information about mount options and modules
			size_t FSDuration = time(0) - FSStartTime;
			stringstream str;
			str << FSDuration;
			string strFSStartTime = ctime(&FSStartTime);
			ofstream fs_xml((logfolder + "/" + *fs + ".xml").c_str());
			fs_xml << "<SpruceDashboard FS=\"" + *fs + "\">\n";
			fs_xml << "\t<Start>" + strFSStartTime + "</Start>\n";
			fs_xml << "\t<Duration>" + str.str() + "</Duration>\n";
			fs_xml << "\t<Rev>" HG_REV "</Rev>\n";
			fs_xml << "\t<MountOptions>\n";
			for ( unsigned int i = 0; i < MountOptions.size(); ++i )
				fs_xml << "\t\t<Option>" + MountOptions[i] + "</Option>\n";
			fs_xml << "\t</MountOptions>";
			
			fs_xml << "\t<Modules>\n";
			for ( unsigned int i = 0; i < Modules.size(); ++i )
				fs_xml << "\t\t<Module>" + Modules[i] + "</Module>\n";
			fs_xml << "\t</Modules>\n";
			fs_xml << "</SpruceDashboard>";
			
			fs_xml.close();
			MountOptions.erase(MountOptions.begin(), MountOptions.end());
			
			// Open the log files in the selected browser
			if ( browser != "" )
			{
				OpenDashboard(browser, logfolder, *fs);
				//OpenLogFiles(browser, logfolder, XMLFilesToProcess);
				//XMLFilesToProcess.erase(XMLFilesToProcess.begin(), XMLFilesToProcess.end());
			}
			if (  ModuleStatus >= Fatal )
			{
				break;
			}
		}
		return ( SpruceStatus == 0 ) ? SUCCESS : FAULT;
	}
	catch (Exception e)
	{
		cerr << "Exception is thrown: " << e.GetMessage() << endl;
		return FAULT;
	}
	//return 0;
}

void OpenDashboard(string browser, string logfolder, string fs)
{
	// The new process must be created to be able to execute the browser as non-privileged user.
	if ( fork() == 0 )
	{
		cout << "Opening logfile in browser " << browser << endl;
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
		xslt_args.push_back(logfolder + +"/" + fs + ".xml");
		
		res = xslt.Execute(xslt_args);

		if ( res == NULL )
		{
			cerr << "Error executing xsltproc. Error: " << strerror(errno) << endl;
			_exit(1);
		}
		
		if ( res->GetStatus() != Success )
		{
			cerr << res->GetOutput() << endl;
			_exit(1);
		}			
		cout << endl;
	
		
		// Change the effective user id to real user id... browsers don't like ronning as root.
		
		char * UserName = getenv("SUDO_USER");
		//char * UserName = "nobody";
		
		if ( UserName == NULL )
		{
			cerr << "Cannot obtain user name. " << strerror(errno) << endl;
			return;
		}
		
		setenv("HOME", (static_cast<string>("/home/") + UserName).c_str(), 1);
		
		struct passwd * nobody = getpwnam(UserName);
		if ( nobody == NULL )
		{
			cerr << "Cannot switch to user `" << UserName << "`. Browser won't start. " << strerror(errno) << endl;
			return;
		}
			
		if ( setuid(nobody->pw_uid) == -1 )
		{
			cerr << "Cannot switch to user `" << UserName << "`. Browser won't start. " << strerror(errno) << endl;
			return;
		}
		
		UnixCommand * browser_cmd = new UnixCommand(browser, ProcessBackground);
		vector<string> browser_args;
		browser_args.push_back(logfolder + "/" + fs + ".html");
		
		res = browser_cmd->Execute(browser_args);
		delete browser_cmd;
		
		if ( res == NULL )
		{
			cerr << "Cannot execute the browser: " << browser << endl;
			_exit(1);
		}
		if ( res->GetStatus() != Success )
		{
			cerr << "Error executing " << browser << ". " << strerror(errno) << "\n Output: " << res->GetOutput() << endl;
			_exit(1);
		}
		_exit(0);
	}
	else
	{
		wait(0);
	}
}

void OpenLogFiles(string browser, string logfolder, vector<string> XMLFilesToProcess)
{
	// The new process must be created to be able to execute the browser as non-privileged user.
	if ( fork() == 0 )
	{
		ProcessResult * res = NULL;
		// Process the module log files
		for ( unsigned int i = 0; i < XMLFilesToProcess.size(); ++i )
		{
			cout << "Processing file " << XMLFilesToProcess[i] << "... ";
			UnixCommand xslt("xsltproc");
			vector<string> xslt_args;
			
			xslt_args.push_back("-o");
			xslt_args.push_back(XMLFilesToProcess[i].substr(0, XMLFilesToProcess[i].size() - 3) + "html");
			xslt_args.push_back("-novalid");			
			xslt_args.push_back(logfolder + "/xslt/processor.xslt");
			xslt_args.push_back(XMLFilesToProcess[i]);
			
			res = xslt.Execute(xslt_args);

			if ( res == NULL )
			{
				cerr << "Error executing xsltproc. Error: " << strerror(errno) << endl;
				continue;
			}
			
			if ( res->GetStatus() != Success )
			{
				cerr << res->GetOutput() << endl;
				continue;
			}			
			cout << "Done" << endl;
		}
		
		// Change the effective user id to real user id... browsers don't like ronning as root.
		
		char * UserName = getenv("SUDO_USER");
		//char * UserName = "nobody";
		
		if ( UserName == NULL )
		{
			cerr << "Cannot obtain user name. " << strerror(errno) << endl;
			return;
		}
		
		cout << "Switching to user `" << UserName << "`" << endl;
		
		setenv("HOME", (static_cast<string>("/home/") + UserName).c_str(), 1);
		
		struct passwd * nobody = getpwnam(UserName);
		if ( nobody == NULL )
		{
			cerr << "Cannot switch to user `" << UserName << "`. Browser won't start. " << strerror(errno) << endl;
			return;
		}
			
		if ( setuid(nobody->pw_uid) == -1 )
		{
			cerr << "Cannot switch to user `" << UserName << "`. Browser won't start. " << strerror(errno) << endl;
			return;
		}
		
		cerr << "UserId: " << getuid() << endl;
		
		
		
		cerr << "$HOME: " << getenv("HOME") << endl;
		
		for ( unsigned int i = 0; i < XMLFilesToProcess.size(); ++i )
		{
			// Hold on a while...
			sleep(1);
			
			UnixCommand * browser_cmd = new UnixCommand(browser, ProcessBackground);
			vector<string> browser_args;
			browser_args.push_back(XMLFilesToProcess[i].substr(0, XMLFilesToProcess[i].size() - 3) + "html");
			/*if ( browser.find("chrom") != string::npos )
			{
				//browser = "chromium";
				browser_args.push_back("--allow-file-access-from-files");
				browser_args.push_back("--user-data-dir");
				browser_args.push_back("/tmp");
			}*/
			res = browser_cmd->Execute(browser_args);
			delete browser_cmd;
			
			if ( res == NULL )
			{
				cerr << "Cannot execute the browser: " << browser << endl;
				continue;
			}
			if ( res->GetStatus() != Success )
			{
				cerr << "Error executing " << browser << ". " << strerror(errno) << "\n Output: " << res->GetOutput() << endl;
				continue;
			}
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
			cerr << "Cannot open configuration file " << FilePath << endl;				
			return vals;
		}
		while ( !FileReader.eof() && FileReader.good() )
		{			
			string CurrentLine;
			FileReader >> CurrentLine;
			//cerr << "Config line: " << CurrentLine << endl;
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



