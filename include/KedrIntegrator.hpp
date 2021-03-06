//      kedr_integrator.hpp
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

// TODO: Add configuration for the KEDR path!

#ifndef KEDR_INTEGRATOR
#define KEDR_INTEGRATOR

#include <PlatformConfig.hpp>
#include <Exception.hpp>
#include <UnixCommand.hpp>
#include <sys/utsname.h>
#include <algorithm>
#include <map>

const string LastFaultMsgNone = "none";
const string LastFaultTraceStart = "[KEDR: Start last fault call trace]";
const string LastFaultTraceEnd = "[KEDR: End last fault call trace]";

using namespace std;

const string DebugFSPath = "/sys/kernel/debug";

// A class which encapsulates access to the KEDR framework
class KedrIntegrator
{
public:
	KedrIntegrator()
	{
		if ( KEDR_INSTALLED )
			if ( !IsKEDRInstalled() )
				throw (Exception("KEDR framework is not installed on the system."));
	}
	KedrIntegrator(string module)
	{
		if ( KEDR_INSTALLED )
			if ( !IsKEDRInstalled() )
				throw (Exception("KEDR framework is not installed on the system."));
			
		if ( module != "" )
			if ( !SetTargetModule(module) )
			{
				throw (Exception("Cannot set target module."));
			}
		
	}
	static bool SetTargetModule(string module)
	{
		DoesModuleExist(module);
		
		if ( IsModuleLoaded(module) )
			UnloadModule(module);
			
		TargetModule = module;
		return true;
	}
	// Add KEDR configuration file to be processed while load KEDR.
	static void AddPayloads(const string& confFile)
	{
		KEDRProfiles.push_back(confFile);
	}
	
	static void EnableMemLeakCheck()
	{
		MemLeakCheckEnabled = true;
		AddPayloads("leak_check.conf");
	}
	
	void EnableFaultSimulation()
	{
		FaultSimulationEnabled = true;
		AddPayloads("fsim.conf");
		AddPayloads(FAULT_SIMULATION_CUSTOM_PAYLOADS);
	}
	
	static void SetAllIndicators(string indicator = "common")
	{
		DIR * dir = opendir(  (DebugFSPath + "/kedr_fault_simulation/points/").c_str() );
		if ( dir == NULL )
		{
			Logger::LogError("Cannot open kedr fault simulation points directory.");
			return;
		}
		
		while ( struct dirent * file = readdir(dir) )
		{
			if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") )
				continue;
				
			SetIndicator( file->d_name, indicator );
			ClearPid(file->d_name);
			ClearTimes(file->d_name);
		}
		
		closedir(dir);
	}
	
	static void ClearAllIndicators()
	{
		DIR * dir = opendir(  (DebugFSPath + "/kedr_fault_simulation/points/").c_str() );
		if ( dir == NULL )
		{
			Logger::LogError("Cannot open kedr fault simulation points directory.");
			return;
		}
		
		while ( struct dirent * file = readdir(dir) )
		{
			if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") )
				continue;
				
			ClearIndicator( file->d_name );
		}
		
		closedir(dir);
	}
	
	
	// The process ID is set to the curent PID
	static bool SetIndicator(string point, string indicator)
	{
		if ( !KEDR_INSTALLED )
			return false;
		try
		{
			ofstream of((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/current_indicator").c_str());
			of << indicator;
			of.close();
			//Logger::LogInfo("Indicator is set in " + DebugFSPath + "/kedr_fault_simulation/points/" + point + "/current_indicator");
			
			
			return true;
		}
		catch (...)
		{
			Logger::LogError("Cannot set indicator " + indicator + " for point " + point);
			return false;
		}
	}
	
	static bool SetExpression(string point, string expression)
	{
		try
		{
			ofstream of;
			of.open((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/expression").c_str());
			of << expression;
			of.close();
			Logger::LogInfo("Expression `" + expression + "` is set.");
			return true;
		}
		catch (...)
		{
			Logger::LogError("Cannot set expression " + expression + " for point " + point);
			return false;
		}
	}		
	
	static bool ClearIndicator(string point)
	{
		if ( !KEDR_INSTALLED )
			return false;
		try
		{
			ofstream of((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/current_indicator").c_str());
			of << "none";
			of.close();
			//Logger::LogInfo("Indicator is cleared.");
			return true;
		}
		catch (...)
		{
			Logger::LogError("Cannot clear indicator for point " + point);
			return false;
		}
	}
	
	static int GetTimes( string point )
	{
		string _time_file = DebugFSPath + "/kedr_fault_simulation/points/"+ point + "/times";
		char buffer[257];
		int _time_val;
		try
		{
			ifstream str( _time_file.c_str(), ifstream::in );
			str >> buffer;
			_time_val = atoi(buffer);
			str.close();
		
			return _time_val;
		}
		catch (...)
		{
			Logger::LogError("Cannot obtain `times` value for point " + point);
			return -1;
		}
	}
	
	static void SetPid(string point = "", pid_t pid = 0 )
	{
		// Recurse the function to set the same process ID in all the points
		if ( point == "" )
		{
			DIR * dir = opendir(  (DebugFSPath + "/kedr_fault_simulation/points/").c_str() );
			if ( dir == NULL )
			{
				Logger::LogError("Cannot open kedr fault simulation points directory.");
				return;
			}
			
			while ( struct dirent * file = readdir(dir) )
			{
				if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") )
					continue;
				SetPid( file->d_name, pid );
			}
			
			closedir(dir);
			return;
		}
		
		/*if ( pid != -1 )
			Logger::LogInfo("Setting pid for point " + point );*/
		
		string _pid_file = DebugFSPath + "/kedr_fault_simulation/points/"+ point + "/pid";
		
		if ( pid == 0 )
			pid = getpid();
			
		try
		{
			ofstream str( _pid_file.c_str() );
			str << pid;
			str.close();
		}
		catch (...)
		{
			Logger::LogError("Cannot write to file " + _pid_file);
		}
	}
	
	static void ClearPid(string point)
	{
		//Logger::LogInfo("Clearing pid for point " + point );
		SetPid(point, -1);
	}
	
	static map<string, unsigned int> GetPointTimesMap()
	{
		map<string, unsigned int> PointTimes;
		
		DIR * dir = opendir(  (DebugFSPath + "/kedr_fault_simulation/points/").c_str() );
		if ( dir == NULL )
		{
			Logger::LogError("Cannot open kedr fault simulation points directory.");
			return PointTimes;
		}
		
		while ( struct dirent * file = readdir(dir) )
		{
			if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") )
					continue;
			
			PointTimes[file->d_name] = GetTimes(file->d_name);
		}
		
		closedir(dir);
		
		return PointTimes;
	}


	
	static void ClearTimes( string point )
	{
		string _time_file = DebugFSPath + "/kedr_fault_simulation/points/"+ point + "/times";
		ofstream of(_time_file.c_str());
		of << "0";
		of.close();
	}
	
	static bool LoadKEDR()
	{
		if ( !KEDR_INSTALLED )
			return false;
		// Try to unload KEDR first
		if(IsRunning())
		{
			UnloadKEDR();
		}
		
		MountDebugFS();
		
		UnixCommand kedr(KEDR_PATH);
		vector<string> args;
		args.push_back("start");
		args.push_back(TargetModule);
		for (unsigned int i = 0; i < KEDRProfiles.size(); ++i)
		{
			args.push_back("-f");
			args.push_back(KEDRProfiles[i]);
		}
		
		ProcessResult * res = kedr.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
			throw(Exception("Error loading KEDR. " + (res ? res->GetOutput() : "")));
		
		// Check if it is really loaded.
		args.erase(args.begin(), args.end());
		args.push_back("status");
		
		res = kedr.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
			throw(Exception("Error getting KEDR status. " + (res ? res->GetOutput() : "")));
		
		
		 // Indicators are loaded by the KEDR framework automatically.
		  //LoadIndicators();
		//_IsRunning = true;
		return true;
	}
	
	static bool UnloadKEDR()
	{
		if ( !KEDR_INSTALLED )
			return true;
		try
		{
			// No need to unload the indicators. They are not controlled by us.
			 //UnloadIndicators();
			UnloadModule(TargetModule);
			UnixCommand kedr(KEDR_PATH);
			vector<string> args;
			args.push_back("stop");
					
			ProcessResult * res = 
			kedr.Execute(args);
			
			if ( res == NULL || res->GetStatus() != Success )
				throw(Exception("Error unloading KEDR. " + (res ? res->GetOutput() : "")));
			//_IsRunning = false;
		}
		catch (Exception e)
		{
			Logger::LogError("Not all of the components were successfully unloaded. " + e.GetMessage());
			return false;
		}
		return true;
	}
	
	static bool IsRunning()
	{
		if ( !KEDR_INSTALLED )
			return false;
		//return _IsRunning;
		UnixCommand kedr(KEDR_PATH);
		vector<string> args;
		args.push_back("status");
		ProcessResult * res = kedr.Execute(args);
		if(res == NULL || res->GetStatus() != Success)
		{
			Logger::LogError("Failed to get kedr status " + (res ? res->GetOutput() : ""));
			return false;
		}
		string StrRes = res->GetOutput();
		if(StrRes.find("KEDR is running") != string:: npos)
			return true;
		
		return false;
	}
	
	static bool UnloadModule(string module, string* output = NULL)
	{
		UnixCommand rmmod("rmmod");
		vector<string> args;
		args.push_back(module);
		//args.push_back("-f");
		
		ProcessResult * res = 
		rmmod.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
		{
			//throw(Exception("Error executing rmmod. " + (res ? res->GetOutput() : "")));
			if(output != NULL)
			{
				*output = string("Error executing rmmod: ");
				*output += res->GetOutput();
			}
			Logger::LogWarn("Error executing rmmod. " + (res ? res->GetOutput() : ""));
			return false;
		}
		return true;
	}
	
	static bool IsModuleLoaded(string module)
	{
		ifstream modules("/proc/modules");
		
		string str;
		while ( modules.good() )
		{
			modules >> str;
			if (str == module)
			{
				modules.close();
				return true;
			}
		}
		
		modules.close();
		return false;
	}	

	static void ResetLastFaultMsg()
	{
		// Clear the last_fault file.
		ofstream of((DebugFSPath + "/kedr_fault_simulation/last_fault").c_str(), ifstream::out);
		of << LastFaultMsgNone;
		of.close();
	}
	
	static string GetLastFaultMsg()
	{
		// Check if the fault simulation is in progress.
		if ( !FaultSimulationEnabled )
			return LastFaultMsgNone;
			
		char buf[5000];
		ifstream f((DebugFSPath + "/kedr_fault_simulation/last_fault").c_str(), ifstream::in);
		f.read(buf, 5000);
		f.close();
		
		string LastFaultFull = (string)buf;
		if ( LastFaultTrace != "" )
			LastFaultFull = LastFaultFull + "\n" + LastFaultTrace;
		
		return LastFaultFull;
	}
	
	// Try to get the call trace information from the kernel log.
	// This information is not populated by default KEDR installation.
	// Call to dump_stack() must be added in file 
	// sources/templates/payload_fsim.c/block/block.tpl somewhere near call to kedr_fsim_fault_message().
	
	// This method better be called from OopsChecker for each peace of log being analyzed.
	static string ParseLastFaultTrace(string log)
	{
		size_t start_pos, end_pos;
		start_pos = log.find(LastFaultTraceStart);
		if ( start_pos == string::npos )
			return "";
			
		end_pos = log.find(LastFaultTraceEnd);
		if ( end_pos == string::npos )
			return "";
			
		LastFaultTrace = log.substr(start_pos + LastFaultTraceStart.length(), end_pos - start_pos - LastFaultTraceStart.length());
			
		return LastFaultTrace;
	}
	
	static string GetLastFaultTrace()
	{
		return LastFaultTrace;
	}
	
	static bool HasLastFault()
	{
		// Check if the fault simulation is in progress.
		if ( !FaultSimulationEnabled )
			return false;
			
		char buf[5000];
		ifstream f((DebugFSPath + "/kedr_fault_simulation/last_fault").c_str(), ifstream::in);
		f.read(buf, 5000);
		f.close();
		
		return (((string)buf).find(LastFaultMsgNone) == string::npos);
	}
	
	static bool DoesModuleExist(string module)
	{
		struct utsname buf;
		if (uname(&buf) == -1)
			throw (Exception("Cannot get kernel version. " + static_cast<string>(strerror(errno))));
			
		string ModulesDir = "/lib/modules/" + static_cast<string>(buf.release) + "/kernel";
		UnixCommand find("find");
		vector<string> args;
		args.push_back(ModulesDir);
		args.push_back("-name");
		args.push_back(module + ".ko*");
		
		ProcessResult * res = find.Execute(args);
		
		if ( res == NULL )
			throw(Exception("Error executing find."));
		
		string str = res->GetOutput();
		// Trim
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		return !str.empty();
	}
	
	
protected:
	static string TargetModule;
	static bool MemLeakCheckEnabled;
	static bool FaultSimulationEnabled;
	static vector<string> KEDRProfiles;
	static string LastFaultTrace;
	//bool _IsRunning;
	static bool MountDebugFS()
	{
		// Is DebugFS already mounted?
		UnixCommand mount("mount");
		vector<string> args;
		args.push_back("none");
		args.push_back("-t");
		args.push_back("debugfs");		
		args.push_back(DebugFSPath);
		ProcessResult *res = mount.Execute(args);
		
		if ( res == NULL )
			throw Exception("Error executing mount. " + (res ? res->GetOutput() : ""));
			
		return true;
	}
	static bool IsKEDRInstalled()
	{
		return (access(KEDR_PATH, F_OK) == 0);
	}
	
	// Loads the common, kmalloc and capable indicators into the kernel
	static void LoadIndicators()
	{
		struct utsname buf;
		if (uname(&buf) == -1)
			throw (Exception("Cannot get kernel version. " + static_cast<string>(strerror(errno))));
			
		string ModulesDir = "/usr/local/lib/modules/" + static_cast<string>(buf.release) + "/misc/";
		
		vector<string> indicators;
		indicators.push_back(ModulesDir + "kedr_fsim_indicator_common.ko");
		indicators.push_back(ModulesDir + "kedr_fsim_indicator_kmalloc.ko");
		indicators.push_back(ModulesDir + "kedr_fsim_indicator_capable.ko");
		
		UnixCommand insmod("insmod");
		for ( unsigned int i = 0; i < indicators.size(); ++i )
		{
			vector<string> args;
			args.push_back(indicators[i]);
			
			ProcessResult * res = insmod.Execute(args);
			if ( res == NULL || res->GetStatus() != Success )
				throw(Exception("Error loading indicators. " + (res ? res->GetOutput() : "")));
		}
		
	}
	
	static void UnloadIndicators()
	{
		UnixCommand rmmod("rmmod");
		vector<string> args;
		args.push_back("kedr_fsim_indicator_common");
		args.push_back("kedr_fsim_indicator_kmalloc");
		args.push_back("kedr_fsim_indicator_capable");
		
		//ProcessResult * res = 
		rmmod.Execute(args);
	}
};
#endif // #ifndef KEDR_INTEGRATOR
