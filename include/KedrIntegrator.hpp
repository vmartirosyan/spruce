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

using namespace std;

const string DebugFSPath = "/sys/kernel/debug";

// A class which encapsulates access to the KEDR framework
class KedrIntegrator
{
public:
	KedrIntegrator():		
		TargetModule(""),
		MemLeakCheckEnabled(false),
		FaultSimulationEnabled(false),
		KEDRProfiles()	
		//_IsRunning(false)
	{
		if ( KEDR_INSTALLED )
			if ( !IsKEDRInstalled() )
				throw (Exception("KEDR framework is not installed on the system."));
	}
	KedrIntegrator(string module):		
		TargetModule(""),
		MemLeakCheckEnabled(false),
		FaultSimulationEnabled(false),
		KEDRProfiles()
		//_IsRunning(false)
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
	bool SetTargetModule(string module)
	{
		DoesModuleExist(module);
		
		if ( IsModuleLoaded(module) )
			UnloadModule(module);
			
		TargetModule = module;
		return true;
	}
	bool EnableMemLeakCheck()
	{
		KEDRProfiles.push_back("leak_check.conf");
		return true;
	}
	
	bool EnableFaultSimulation()
	{
		KEDRProfiles.push_back("fsim.conf");
		return true;
	}
	bool EnableCallMonitoring()
	{
		KEDRProfiles.push_back("callm.conf");
		return true;
	}
	// The process ID is set to the curent PID
	static bool SetIndicator(string point, string indicator, string expression)
	{
		if ( !KEDR_INSTALLED )
			return false;
		try
		{
			ofstream of((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/current_indicator").c_str());
			of << indicator;
			of.close();
			cerr << "Indicator is set in " << DebugFSPath + "/kedr_fault_simulation/points/" + point + "/current_indicator" << endl;
			
			of.open((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/expression").c_str());
			of << expression;
			of.close();
			cerr << "Expression is set" << endl;
			
			of.open((DebugFSPath + "/kedr_fault_simulation/points/" + point + "/pid").c_str());
			of << getpid();
			of.close();
			cerr << "PID is set" << endl;
			return true;
		}
		catch (...)
		{
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
			cerr << "Indicator is cleared." << endl;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	
	static int GetTimes( string point )
	{
		string _time_file = DebugFSPath + "/kedr_fault_simulation/points/"+ point + "/times";
		cerr << "time file path: "<<_time_file.c_str() << endl;
		char buffer[257];
		int _time_val;
		ifstream str( _time_file.c_str(), ifstream::in );
		cerr << "Error in GetTimes(): "<<strerror(errno) << endl;
		str >> buffer;
		_time_val = atoi(buffer);
		str.close();
	
		
		return _time_val;
	}
	static void  ResetTimes( string point )
	{
		string _time_file = DebugFSPath + "/kedr_fault_simulation/points/"+ point + "/times";
		ofstream of(_time_file.c_str());
		of << "0";
		of.close();
	} 
	bool LoadKEDR()
	{
		if ( !KEDR_INSTALLED )
			return false;
		// Try to unload KEDR first
		if(IsRunning())
		{
			cerr << "is running !" << endl;
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
	
	bool UnloadKEDR()
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
			cerr << "Not all of the components were successfully unloaded. " << e.GetMessage() << endl;
			return false;
		}
		return true;
	}
	
	bool IsRunning()
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
			cerr << "failed to get kedr status " + (res ? res->GetOutput() : "") << endl;
			return false;
		}
		string StrRes = res->GetOutput();
		if(StrRes.find("KEDR is running") != string:: npos)
			return true;
		
		return false;
	}
	
	bool UnloadModule(string module)
	{
		UnixCommand rmmod("rmmod");
		vector<string> args;
		args.push_back(module);
		
		ProcessResult * res = 
		rmmod.Execute(args);
		
		if ( res == NULL || res->GetStatus() != Success )
		{
			//throw(Exception("Error executing rmmod. " + (res ? res->GetOutput() : "")));
			cerr << "Error executing rmmod. " << (res ? res->GetOutput() : "") << endl;
			return false;
		}
		return true;
	}
	
protected:
	string TargetModule;
	bool MemLeakCheckEnabled;
	bool FaultSimulationEnabled;
	vector<string> KEDRProfiles;
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
	bool IsKEDRInstalled()
	{
		return (access(KEDR_PATH, F_OK) == 0);
	}
	
	void DoesModuleExist(string module)
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
		if (str == "")
			throw(Exception("Cannot find module " + module + "."));
		
	}
	
	bool IsModuleLoaded(string module)
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
	
	// Loads the common, kmalloc and capable indicators into the kernel
	void LoadIndicators()
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
	
	void UnloadIndicators()
	{
		UnixCommand rmmod("rmmod");
		vector<string> args;
		args.push_back("kedr_fsim_indicator_common");
		args.push_back("kedr_fsim_indicator_kmalloc");
		args.push_back("kedr_fsim_indicator_capable");
		
		ProcessResult * res = 
		rmmod.Execute(args);
		
		cerr << res->GetOutput() << endl;
	}
};
#endif // #ifndef KEDR_INTEGRATOR
