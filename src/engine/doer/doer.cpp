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
		TestPackage pkgCommon = Initcommon();
		packages[pkgCommon.GetName()] = pkgCommon;
		TestPackage pkgBtrfs = Initbtrfs();
		packages[pkgBtrfs.GetName()] = pkgBtrfs;
		TestPackage pkgExt4 = Initext4();
		packages[pkgExt4.GetName()] = pkgExt4;
		TestPackage pkgJfs = Initjfs();
		packages[pkgJfs.GetName()] = pkgJfs;
		TestPackage pkgXfs = Initxfs();
		packages[pkgXfs.GetName()] = pkgXfs;
		Initialized = true;
	}
	
	map<string, TestPackage*> result;
	
	
#ifdef COMPAT
	if ( find(AllowedPackages.begin(), AllowedPackages.end(), (string)"common_32") != AllowedPackages.end() )
	{
		result["common_32"] = &packages["common_32"];
	}
#else
	if ( find(AllowedPackages.begin(), AllowedPackages.end(), (string)"common") != AllowedPackages.end() )
	{
		result["common"] = &packages["common"];
	}
#endif
	
	if ( find(AllowedPackages.begin(), AllowedPackages.end(), (string)"fs-spec") != AllowedPackages.end() )
	{
#ifdef COMPAT
		if ( packages.find(fs+"_32") != packages.end() )
		{
			result[packages[fs+"_32"].GetName()] = &packages[fs+"_32"];
		}
#else
		if ( packages.find(fs) != packages.end() )
		{
			result[packages[fs].GetName()] = &packages[fs];
		}
#endif
	}
	
	return result;
}

string GetOptionsToEnable(vector<string> TestsToRun);


vector<string> TestsToRun;
vector<string> TestsToExclude;
string OptionsToEnable;
string Path; // Represents the path of the current running FS.(Mkfs+Mount)Option.TestPackge.TestSet.Test

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
	// r - Run tests
	// e - Exclude tests
	const char* optstring = "f:l:p:c:r:e:"; 
	char c;
	while ( (c = getopt(argc, argv, optstring)) != -1 )
	{
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
				// Add the compat versions of the mentioned packages
#ifdef COMPAT
				{
					size_t PackagesSize = Packages.size();
					for ( size_t i = 0; i < PackagesSize; ++i )
						Packages.push_back(Packages[i] + "_32");
				}
#endif
				break;
			case 'f':
				FileSystem = optarg;
				break;
			case 'l':
				LogFolder = optarg;
				break;
			case 'r':
				TestsToRun = SplitString(optarg, ';');
				break;
			case 'e':
				TestsToExclude = SplitString(optarg, ';');
				break;
			default:
				Logger::LogError((string)"Unknown option: " + c);
				return 1;
		}
		
	}
	
	Path = FileSystem;
		
	if ( SkipTestPath(Path) )
		return 1;
	
	
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
		}
	}
	
	PartitionManager pm(
		INSTALL_PREFIX"/share/spruce/config/PartitionManager.cfg",
		DeviceName,
		MountPoint,
		FileSystem,
		( TestsToRun.size() > 0 ? GetOptionsToEnable(TestsToRun) : ""));
	
	PartitionStatus PS = PS_Success;
	
	TestPackage tp((string)"MemoryLeaks" + ( ( strstr(argv[0], "_32") != 0 ) ? "_32" : "" ));
	
	string PMCurrentOptionsStripped;
	string PMCurrentOptions;
	
	do
	{
		string PMCurrentOptionsStripped = pm.GetCurrentOptions(true);
		string PMCurrentOptions = pm.GetCurrentOptions();
		
		if ( TestsToExclude.size() > 0 )
		{
			Path = (string)FileSystem + "." + PMCurrentOptionsStripped;
			
			if ( SkipTestPath(Path) )
			{
				pm.AdvanceOptionsIndex();
				continue;
			}
		}
		
		MountOptions.push_back(pair<string,string>(PMCurrentOptionsStripped, PMCurrentOptions));
		
		PS = pm.PreparePartition();
		
		if ( PS == PS_Fatal  )
		{
			Logger::LogFatal("Fatal error raised while preparing partition.");
			return 1;
		}
		if ( PS == PS_Skip )
		{
			Logger::LogError("Skipping case.");
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
			Path = (string)FileSystem + "." + PMCurrentOptions + "." + i->first;
			
			Logger::LogInfo("Executing packages. Path: " + Path);

			if ( SkipTestPath(Path) )
				continue;

			Status res = i->second->Run(PerformChecks);
			
			size_t ItemDuration = time(0) - ItemStartTime;
			stringstream str;
			str << ItemDuration;
			
			string LogFile = 
				LogFolder + "/" // LogFolder
				+ FileSystem + "_"  // FS
				+ i->first + "_"  // Package name
				// Add the '_32' suffix to the name in case of compatibility checks
				// + ( ( strstr(argv[0], "_32") != 0 ) ? "32_" : "" )
				+ PMCurrentOptionsStripped // Current options
				+ "_log.xml";
			
			XMLGenerator::GenerateLog(LogFile , *i->second, FileSystem, "", str.str());
		}

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
			+ PMCurrentOptionsStripped // Current options
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
	
	ofstream fs_xml((LogFolder + "/" + FileSystem + ".xml").c_str(), ios_base::app);
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
	fs_xml.close();
	
	MountOptions.erase(MountOptions.begin(), MountOptions.end());
		
	cerr << "Doer complete." << endl;
	
	return 0;
}

bool SkipTestPath(string Path)
{
	if ( TestsToRun.size() != 0 )
	{
		for ( size_t i = 0; i < TestsToRun.size(); ++i)
		{
			vector<string> TestsToRunComponents = SplitString(TestsToRun[i], '.');
			vector<string> PathComponents = SplitString(Path, '.');
			
			for ( size_t j = 0; j < PathComponents.size() && j < TestsToRunComponents.size(); ++j )
			{
				if ( TestsToRunComponents[j] != PathComponents[j] )
				{
					Logger::LogInfo("Path `" + Path + "` is not included in run_tests value. Skipping.");
					return true;
				}
			}
		}
		Logger::LogInfo("\t\tPath `" + Path + "` is included in run_tests value.");
		return false;
	}
	else if ( TestsToExclude.size() != 0 )
	{
		for ( size_t i = 0; i < TestsToExclude.size(); ++i )
		{
			if ( Path == TestsToExclude[i] )
			{
				Logger::LogInfo("Path `" + Path + "` is included in exclude_tests value. Skipping.");
				return true;
			}
		}
		return false;
	}
	return false;
}

string GetOptionsToEnable(vector<string> TestsToRun)
{
	string res = "";
	for ( size_t i = 0; i < TestsToRun.size(); ++i )
	{
		size_t pos1 = TestsToRun[i].find('.');
		if ( pos1 == string::npos )
			continue;
			
		size_t pos2 = TestsToRun[i].find('.', pos1 + 1 );
		if ( pos2 == string::npos )
			continue;
		
		res += TestsToRun[i].substr(pos1 + 1, pos2 - pos1 - 1) + ";";
	}
	return res;
}
