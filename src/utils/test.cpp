//      test.hpp
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

#include "Test.hpp"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <KedrIntegrator.hpp>
#include <PartitionManager.hpp>
#include <memory>

// Defined in doer.cpp
extern string Path; 
extern char * DeviceName;
extern char * MountPoint;
extern char * FileSystem;
					

ProcessResult * Test::Execute(vector<string> args)
{
	Logger::LogInfo("Test: " + _name);
	ProcessResult * p_res = Process::Execute(_func, args);
	Logger::LogInfo("Test: " + _name + " - OK");
	
	return p_res;
}

Status Test::OopsChecker(string& OutputLog)	
{
	//Logger::LogDebug("OOPS CHECKER ACTIVATED...Wait for output. \n");
	string mainMessage;
	vector<string> args;
	args.push_back("-c"); // Clear the ring
	//args.push_back("dmesg");
	UnixCommand* command = new UnixCommand("dmesg");
	std::auto_ptr<ProcessResult> result(command->Execute(args));
	if(result.get() == NULL || result->GetStatus() != Success)
	{
		OutputLog = "Unable to read system log";
		if ( result.get() != NULL )
			OutputLog += result->GetOutput();
		return Unresolved;
	}
	mainMessage = result->GetOutput();
	delete command;
	
	//searching points
	const string bug = "BUG";
	const string oops = "Oops";
	const string panic = "ernel panic";  //interested in both "Kernel panic" and "kernel panic"
	const char less = 60;  // 60 is the ASCII code of the operation 'less than'
	const char amp = 38;   // 38 is the ASCII code of the operation 'ampersand'
	size_t foundPos;
	
	//Logger::LogDebug(mainMessage);
	foundPos = mainMessage.find(bug); // searching "bug" in the kernel output
	if( foundPos != string::npos )
	{
		OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
		size_t pos = 0;
		while(true)
		{
			pos = (OutputLog.find(amp, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " "); 
		}
		while(true)
		{
			pos = (OutputLog.find(less, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " ");   
		}
		Logger::LogFatal("Oops checker: bug found." + OutputLog);
		return Bug;
	}
	foundPos = mainMessage.find(oops);			
	if( foundPos != string::npos )
	{
		OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
		size_t pos = 0;
		while(true)
		{
			pos = (OutputLog.find(amp, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " ");  
		}
		while(true)
		{
			pos = (OutputLog.find(less, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " ");  
		}
		Logger::LogFatal("Oops checker: oops found." + OutputLog);
		return Oops;
	}
	foundPos = mainMessage.find(panic);			
	if( foundPos != string::npos )
	{
		OutputLog.assign(mainMessage.begin() + foundPos, mainMessage.end());
		size_t pos = 0;
		while(true)
		{
			pos = (OutputLog.find(amp, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " ");  
		}
		while(true)
		{
			pos = (OutputLog.find(less, pos+1));
			if(pos == std::string::npos)
				break;
			OutputLog.replace(pos, 1, " ");  
		}
		Logger::LogFatal("Oops checker: Panic found." + OutputLog);
		return Panic;
	}
	return Success;
}

void TestSet::Merge(TestSet & tc)
{
	//for ( vector<Test *>::iterator i = tc._tests.begin(); i != tc._tests.end(); ++i)		
	for ( map<string, Test>::iterator i = tc._tests.begin(); i != tc._tests.end(); )
	{
		_tests.insert(*i);
		// Erase the original pointer so that the destructors do not overlap
		tc._tests.erase(i);
	}
}

Status TestSet::Run(Checks checks)
{
	
	string OrigPath = Path;
	
	Status result = Success;
	Status st = Success;
	
	// Clear the test results
	map<string, Test>::iterator i = _tests.begin();
	while ( i != _tests.end())
	{
		i->second.ClearResults();
		i++;
	}
	
	if ( _StartUpFunc )
		if ( (st = _StartUpFunc()) != Success )
			return st;
			
	i = _tests.begin();
	while ( i != _tests.end())
	{
		i->second.ClearResults();
		
		Path = OrigPath + "." + i->first;
		
		if ( SkipTestPath(Path) )
		{
			_tests.erase(i++);
			continue;
		}
		
		i->second.SetChecks(checks);
		
		// Skip the dangerous tests if it is not clearly mentioned in the configuration file
		if ( ( ( i->second.GetSupportedChecks() & Dangerous ) != None) && 
			 ( ( i->second.GetEffectiveChecks() & Dangerous ) == None))
		{
			Logger::LogInfo("Test " + i->first + " is dangerous. Skipping.");
			i++;
			continue;
		}
		
		if ( i->second.GetEffectiveChecks() == None )
		{
			Logger::LogInfo("Test " + i->first + " does not support necessary checks. Skipping.");
			i++;
			continue;
		}
		
		if ( i->second.GetEffectiveChecks() & Stability )
		{
			KedrIntegrator::SetAllIndicators();
		}
		
		TestResult res = *static_cast<TestResult *>(i->second.Execute());
		if ( i->second.GetEffectiveChecks() & Functional )
			i->second.AddResult(Functional, res );
			
		// Get the last fault information
		/*if ( i->second.GetEffectiveChecks() & Stability )
		{
			Logger::LogWarn("Last fault: " + KedrIntegrator::GetLastFaultMsg());
		}*/
		
		if ( result < res.GetStatus() )
			result = res.GetStatus();
		
		// Perform oops checking and handle it
		string log;
		Status oopsStatus = i->second.OopsChecker(log); // log is an output parameter
		
		if(oopsStatus != Success)
		{	
			//so we have an emergency situation...
			i->second.AddResult(Stability, ProcessResult(Fatal, "Status: " + StatusMessages[oopsStatus] + "Output: " + log));
			break;
		}
		
		// See if the stability check should be done
		
		if ( i->second.GetEffectiveChecks() & Stability )
		{
			i->second.SetChecks(Stability);
			// At this point it known that EnableFaultSim and DisbleFaultSim have been executed
			// Thus the indicators are set, the `times` values are still there.
			map<string, unsigned int> PointTimes = KedrIntegrator::GetPointTimesMap();
			
			KedrIntegrator::ClearAllIndicators();
			
			for ( map<string, unsigned int>::iterator j = PointTimes.begin(); j != PointTimes.end(); ++j )
			{
				if ( j->second )
				{
					stringstream str;
					str << "Test: " << i->first << ".\tProcessing point " 
						<< j->first << ". Count: " << j->second;
					Logger::LogInfo(str.str());
				}
				
				unsigned int Count = j->second;
				for ( unsigned int k = 1; k <= Count; ++k )
				{
					
					i->second.SetCurrentPoint(j->first);
					KedrIntegrator::SetIndicator(j->first, "common");
					KedrIntegrator::ClearPid(j->first);
					KedrIntegrator::ClearTimes(j->first);
					
					stringstream sExpression;
					
					sExpression << "times = ";
					
					sExpression << k;
					
					KedrIntegrator::SetExpression(j->first, sExpression.str() );
					
					Logger::LogInfo("Executing test " + i->first + " in fault simulated environment.");
					TestResult res = *static_cast<TestResult *>(i->second.Execute());
					
					// Get the last fault information
					//Logger::LogWarn("Last fault: " + KedrIntegrator::GetLastFaultMsg());
					
					oopsStatus = i->second.OopsChecker(log); // log is an output parameter
					
					if(oopsStatus != Success)
					{	
						//so we have an emergency situation...
						i->second.AddResult(Stability, ProcessResult(Fatal, "Status: " + StatusMessages[oopsStatus] + "Output: " + log));
						if(log.find(static_cast<string>("INFO:")) != std::string::npos)
						{
							Logger::LogWarn("OopsChecker has found non-fatal problem. Trying to continue execution. \n");
						}
						else
						{
							break;
						}
						
					}
					
					i->second.AddResult(Stability, res );
														
					if(PartitionManager::RestorePartition(DeviceName, MountPoint, FileSystem, true)  != PS_Success)
					{
						Logger::LogFatal("Could not restore partition. Terminating... \n");
						return Fatal;
					}	
									
					// Check if the partition has become read-only because of system failures.
					// In case of fault simulation such a situation is quite common.
					/*if ( PartitionManager::IsOptionEnabled("ro") )
					{
						Logger::LogWarn("Partition was re-mounted as read-only. Restoring partition.");
						PartitionManager::RestorePartition(DeviceName, MountPoint, FileSystem, true);
					}*/
				}
			}
		}
		++i;
	}	
	
	if ( _CleanUpFunc )
		_CleanUpFunc();
		
	Path = OrigPath;
	
	return result;
}

Status TestPackage::Run(Checks checks)
{
	string OrigPath = Path;
	Status result = Success;
	map<string, TestSet>::iterator i = _testsets.begin();
	while ( i != _testsets.end() )
	{
		Path = OrigPath + "." + i->first;
		
		if ( SkipTestPath(Path) )
		{			
			_testsets.erase(i++);
			continue;
		}
		
		Status st = i->second.Run(checks);		
		
		if ( st > result )
			result = st;

		// If Fatal error has rised quit!
		if ( st == Fatal )
			break;

		++i;
	}
	
	//Restore the path
	Path = OrigPath;	
	
	return result;
}
