//      doer.cpp
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

// A component of the Engine which executes the test packages

#include <TestResult.hpp>
#include <XmlSpruceLog.hpp>
#include <PartitionManager.hpp>
#include <KedrIntegrator.hpp>
#include <LeakChecker.hpp>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

//extern TestPackage btrfsTestPackage;
//extern TestPackage ext4TestPackage;

char * DeviceName = NULL;
char * MountPoint = NULL;
char * FileSystem = NULL;

TestPackage Initcommon();
TestPackage Initext4();
TestPackage Initjfs();
TestPackage Initxfs();
TestPackage Initbtrfs();


map<string, int> FileSystemTypesMap;



map<string, TestPackage*> InitPackages(string fs, vector<string> AllowedPackages)
{
	static bool Initialized = false;
	static map<string, TestPackage> packages;
	if ( !Initialized )
	{
		packages["common"] = Initcommon();
		packages["btrfs"] = Initbtrfs();
		packages["ext4"] = Initext4();
		packages["jfs"] = Initjfs();
		packages["xfs"] = Initxfs();
		Initialized = true;
	}
	
	map<string, TestPackage*> result;
	
	if ( find(AllowedPackages.begin(), AllowedPackages.end(), (string)"common") != AllowedPackages.end() )
	{
		result["common"] = &packages["common"];
	}
	
	if ( find(AllowedPackages.begin(), AllowedPackages.end(), (string)"fs-spec") != AllowedPackages.end() )
	{
		if ( packages.find(fs) != packages.end() )
		{
			result[packages[fs].GetName()] = &packages[fs];
		}
	}
	
	return result;
}

int main(int argc, char ** argv)
{
	cerr << "Running " << argv[0] << "." << endl;
	
	if ( argc < 4 )
	{
		cerr << "Usage: " << argv[0] << " <FS> <LogFile> <MountOptions>" << endl;
		return 1;
	}
	
	string LogFolder;
	vector<string> Packages;	
	Checks PerformChecks = None;
	
	// p - Packages,
	// c - Checks
	// f - FS
	// l - Log Folder
	const char* optstring = "f:l:p:c:"; 
	for ( int i = 0; i < 4; ++i )
	{
		char c = getopt(argc, argv, optstring);
		
		switch ( c )
		{
			case 'c':
			{
				vector<string> ch;
				ch = SplitString(optarg, ';');
				for ( vector<string>::iterator j = ch.begin(); j != ch.end(); ++j )
				{
					if ( *j == strFuncional )
						PerformChecks |= Functional;
					else if (*j == strStability)
						PerformChecks |= Stability;
					else if (*j == strMemoryLeak)
						PerformChecks |= MemoryLeak;					
				}
				break;
			}
			case 'p':
				Packages = SplitString(optarg, ';');
				break;
			case 'f':
				FileSystem = optarg;
				break;
			case 'l':
				LogFolder = optarg;
				break;
			default:
				Logger::LogError((string)"Unknown option: " + c);
				return 1;
		}
		
	}
	
	
	if ( getenv("Partition") )
		DeviceName = getenv("Partition");

	if ( getenv("MountAt") )
		MountPoint = getenv("MountAt");
		
	FileSystemTypesMap["ext4"] = 0xEF53; //EXT4_SUPER_MAGIC;
	FileSystemTypesMap["btrfs"] = -1; // does not support?
	FileSystemTypesMap["xfs"] = 0x58465342; //XFS_SUPER_MAGIC;
	FileSystemTypesMap["jfs"] = 0x3153464a; //JFS_SUPER_MAGIC
	
	map<string, TestPackage*> Tests = InitPackages(FileSystem, Packages);
	
	//cerr << "Package count: " << Tests.size() << endl;
	
	PartitionManager pm(INSTALL_PREFIX"/share/spruce/config/PartitionManager.cfg", DeviceName, MountPoint, FileSystem, "");
	
	PartitionStatus PS = PS_Done;
	
	vector<pair<string, string> > MountOptions;
	
	bool PerformLeakCheck = PerformChecks & MemoryLeak;
	bool PerformFaultSimulation = PerformChecks & Stability;
	
	KedrIntegrator kedr;
	
	TestSet leakTestSet("Memory Leaks, Unallocated frees");
	
	Test MemoryLeakTest("Memory Leak", "Checks if there are any possible memory leaks in the filesystem driver.");
			
	// Check if KEDR needs to be loaded			
	if ( PerformLeakCheck || PerformFaultSimulation )
	{
		try
		{
			cout << "Loading KEDR framework for module : " << FileSystem << endl;
			kedr.SetTargetModule(FileSystem);
			if ( PerformLeakCheck )
				kedr.EnableMemLeakCheck();
			if ( PerformFaultSimulation )
				kedr.EnableFaultSimulation();
			// Process possible additional KEDR payload.
			// Used in testing of Spruce itself.
			const char* KEDRAdditionalPayloads = getenv("KEDR_ADDITIONAL_PAYLOADS");
			if(KEDRAdditionalPayloads && KEDRAdditionalPayloads[0] != '\0')
			{
				kedr.AddPayloads(KEDRAdditionalPayloads);
			}
			
			kedr.LoadKEDR();
			cout << "KEDR is successfully loaded." << endl;
			
		}
		catch (Exception e)
		{
			cerr << "KEDR cannot be loaded." << endl;
			cerr << "Exception is thrown. " << e.GetMessage() << endl;
			if ( PerformLeakCheck )
			{
				MemoryLeakTest.AddResult(MemoryLeak, ProcessResult(Unresolved, "Cannot load KEDR framework."));
				leakTestSet.AddTest(MemoryLeakTest);
			}
			PerformFaultSimulation = false;
			PerformLeakCheck = false;
		}
	}
	
	
	time_t FSStartTime = time(0);
	
	TestPackage tp("MemoryLeaks");
	
	do
	{
		MountOptions.push_back(pair<string,string>(pm.GetCurrentOptions(), pm.GetCurrentOptions(false)));
		
		PS = pm.PreparePartition();
		
		if ( PS == PS_Fatal  )
		{
			cerr << "Fatal error raised while preparing partition." << endl;
			return 1;
		}
		if ( PS == PS_Skip )
		{
			cerr << "Skipping case." << endl;
			continue;
		}
		if ( PS == PS_Done  )
		{
			Logger::LogInfo("End of mount options.");
			break;
		}
		
		time_t ItemStartTime = time(0);
		
		for ( map<string, TestPackage*>::iterator i = Tests.begin(); i != Tests.end(); ++i )
		{
			Status res = i->second->Run(PerformChecks);
			
			size_t ItemDuration = time(0) - ItemStartTime;
			stringstream str;
			str << ItemDuration;
			
			cerr << "Current options: " << pm.GetCurrentOptions(true) << endl;
			
			string LogFile = 
				LogFolder + "/" // LogFolder
				+ FileSystem + "_"  // FS
				+ i->first + "_"  // Package name
				// Add the '_32' suffix to the name in case of compatibility checks
				+ ( ( strstr(argv[0], "_32") != 0 ) ? "32_" : "" )
				+ pm.GetCurrentOptions(true) // Current options
				+ "_log.xml";
			
			XMLGenerator::GenerateLog(LogFile , *i->second, FileSystem, "", str.str());
		}
		
		// Prepare the pseudo-package for memory leak check 
		// Process the memory leak checker output
		
	}
	while ( PS != PS_Done );
	
	string relPartOutput = "";
	PartitionManager::ReleasePartition(MountPoint, &relPartOutput);
	
	if ( PerformLeakCheck && kedr.IsRunning() )
	{						
		string kedrUnloadModOutput = "";
		if( kedr.UnloadModule(FileSystem, &kedrUnloadModOutput)) 
		{
			LeakChecker leak_check;
			leakTestSet = leak_check.ProcessLeakCheckerOutput(FileSystem);
		}
		else
		{
			string Output = "";
			
			if(relPartOutput != "")
			{
				Output += relPartOutput + "\n";
			}
			if(kedrUnloadModOutput != "")
			{
				Output += kedrUnloadModOutput + "\n";
			}
			MemoryLeakTest.AddResult(MemoryLeak, ProcessResult(Unresolved, Output));
			leakTestSet.AddTest(MemoryLeakTest);
		}
		
	}
	
   if ( PerformLeakCheck )
   {
     tp.AddTestSet(leakTestSet);
	
     string LogFile = 
		LogFolder + "/" // LogFolder
		+ FileSystem + "_"  // FS
		+ tp.GetName() + "_"  // Package name
		// Add the '_32' suffix to the name in case of compatibility checks			
		+ pm.GetCurrentOptions(true) // Current options
		+ "_log.xml";
		
     XMLGenerator::GenerateLog(LogFile , tp, FileSystem, "", "0");
   }
	
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
	
	// Produce the <FS>.xml to pass to the dashboard generator
	// The file contains information about mount options and packages
	size_t FSDuration = time(0) - FSStartTime;
	stringstream str;
	str << FSDuration;
	string strFSStartTime = ctime(&FSStartTime);
	ofstream fs_xml((LogFolder + "/" + FileSystem + ".xml").c_str());
	fs_xml << "<SpruceDashboard FS=\"" + (string)FileSystem + "\">\n";
	fs_xml << "\t<Start>" + strFSStartTime + "</Start>\n";
	fs_xml << "\t<Duration>" + str.str() + "</Duration>\n";
	fs_xml << "\t<Rev>" HG_REV "</Rev>\n";
	fs_xml << "\t<Options>\n";
	
	for ( unsigned int i = 0; i < MountOptions.size(); ++i )
	{
		string option = MountOptions[i].second;
		string mkfs_opt = "";
		string mount_opt = "";
		if ( option.find(":") != string::npos )
		{
			mkfs_opt = option.substr(0, option.find(":"));
			mount_opt = option.substr( option.find(":") + 1, option.length() - option.find(":") );
		}
		else
			mount_opt = option;
		fs_xml << "\t\t<Option Mkfs=\"" + mkfs_opt + "\" Mount=\"" + mount_opt + 
			"\" Raw=\"" + MountOptions[i].first + "\"/>\n";
	}
	fs_xml << "\t</Options>";
	
	fs_xml << "\t<Packages>\n";
	for ( map<string, TestPackage*>::iterator i = Tests.begin(); i != Tests.end(); ++i )
	{
		if (  strstr(argv[0], "_32") != 0 )
		{
			fs_xml << "\t\t<Package>" + i->first + "_32</Package>\n";
		}
		fs_xml << "\t\t<Package>" + i->first + "</Package>\n";		
	}
	// Add the memory leak test package
	if ( PerformLeakCheck )
	{
		fs_xml << "\t\t<Package>" + tp.GetName() + "</Package>\n";
	}
	fs_xml << "\t</Packages>\n";
	fs_xml << "</SpruceDashboard>";
	
	fs_xml.close();
	MountOptions.erase(MountOptions.begin(), MountOptions.end());
		
	cerr << "Doer complete." << endl;
	
	return 0;
}
