//      Directory.hpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Eduard Bagrov <ebagrov@gmail.com>
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

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "exception.hpp"
#include "UnixCommand.hpp"
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
using std::cerr;

using std::string;	

// Class representing a linux directory
// Example usage:
//		Directory directory("newfile");
//		int fd = directory.GetdirectoryDescriptor();

class Directory
{	
	public:
		Directory() {}
		Directory(string pathname, mode_t mode = (mode_t)(S_IRUSR | S_IWUSR), int flags = O_DIRECTORY ) : 
		_pathname(pathname),
		_mode(mode),
		_flags(flags)
		{
			Open(pathname, mode, flags);
		}
		int Open(string pathname, mode_t mode = (mode_t)(S_IRUSR | S_IWUSR), int flags = O_DIRECTORY )
		{
			_pathname = pathname;
			_mode = mode;
			_flags = flags;
			if (mkdir(_pathname.c_str(), _mode) == -1 && errno != EEXIST)
			{
				throw Exception("Cannot create directory " + _pathname + 
				": error = " + static_cast<string>(strerror(errno)));
			}
			errno = 0;
			_fd = open( _pathname.c_str(), _flags );
			if ( _fd == -1 )
			{
				throw Exception("Cannot open directory " + _pathname + 
				": error = " + static_cast<string>(strerror(errno)));
			}
			return _fd;
		}
		~Directory()
		{			
			try
			{
				close(_fd);
				
				//cerr<<"Removing directory "<<_pathname;
				
				if ( system(("rm -rf " + _pathname).c_str()) == -1)
				{
					cerr << "Cannot remove directory " << _pathname << ". Error: " << strerror(errno);					
				}
			
								
			}
			catch (Exception ex)
			{
				//cerr<<ex.GetMessage();
			}
		}
		
		string GetPathname() const 
		{ 
			return _pathname;
		}
		
		int GetDirectoryDescriptor() const
		{
			return _fd;
		}
		
		int GetFlags() const
		{
			return _flags;
		}
		
		mode_t GetMode() const
		{
			return _mode;
		}
	private:
		string _pathname;
		int _fd;
		mode_t _mode;
		int _flags;
};

#endif
