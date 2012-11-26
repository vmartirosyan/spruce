//      logger.hpp
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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>
using std::string;
using std::ofstream;
using std::cerr;
using std::endl;

enum LogLevel
{
	LOG_None,
	LOG_Info,
	LOG_Debug,
	LOG_Warn,
	LOG_Error,
	LOG_Fatal,
	LOG_All
};

class Logger
{
private:
	static string _LogFile;
	static LogLevel _LogLevel;
	static void Log(LogLevel level, const string & msg)
	{
		// Check if we should print the log
		if ( _LogLevel < level )
			return;
			
		// Well the log level is ok. Print it!
		try
		{
			ofstream of(_LogFile.c_str(), ios_base::app);
			of << msg << endl;
			of.close();
		}
		catch (...)
		{
			cerr << "Cannot write to log file." << endl;
		}
	}
	
public:
	static void LogInfo(const string & msg)
	{
		Log(LOG_Info, msg);
	}
	static void LogDebug(const string & msg)
	{
		Log(LOG_Debug, msg);
	}	
	static void LogWarn(const string & msg)
	{
		Log(LOG_Warn, msg);
	}
	static void LogError(const string & msg)
	{
		Log(LOG_Error, msg);
	}
	static void LogFatal(const string & msg)
	{
		Log(LOG_Fatal, msg);
	}
	static void Init(const string & file, LogLevel level)
	{
		_LogFile = file;
		_LogLevel = level;
	}
	
};

#endif /* LOGGER_HPP */
