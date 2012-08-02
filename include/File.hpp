//      File.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
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
#ifndef FILE_H
#define FILE_H

#include "exception.hpp"
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <iostream>

using std::string;	
using std::cerr;
using std::endl;

// Class representing a linux file system file
// Example usage:
//		File file("newfile");
//		int fd = file.GetFileDescriptor();
class File
{	
	public:
		File() {}
		explicit File(string pathname, mode_t mode = (mode_t)(S_IRUSR | S_IWUSR), int flags = O_RDWR | O_CREAT) : 
		_pathname(pathname),
		_mode(mode),
		_flags(flags)
		{
			
			Open(pathname, mode, flags);		
			
		}
		int Open(string pathname, mode_t mode = (mode_t)(S_IRUSR | S_IWUSR), int flags = O_RDWR | O_CREAT)
		{
			_pathname = pathname;
			_mode = mode;
			_flags = flags;
			// Remove the file first
			if ( flags & O_CREAT )
			{
				if ( access(pathname.c_str(), F_OK ) == 0 )
				{
					if ( unlink(pathname.c_str())  == -1 )
					{
						throw Exception("File " + pathname + " exists but cannot be removed. Error : " + strerror(errno) + "\n");
					}
				}
				else //clear the errno variable...
				{
					errno = 0;
				}
			}
				
			_fd = open(pathname.c_str(), flags, mode);
			
			if (_fd == -1)
			{								
				throw Exception("Cannot create file " + _pathname + ". Error : " + (string)strerror(errno) + (string)"\n");
			}
			return _fd;
		}
		~File()
		{
			try
			{
				close(_fd);
				
				if ( (_flags & O_CREAT) &&  (unlink(_pathname.c_str()) != 0) )
				{
					throw Exception("Cannot delete file " + _pathname + ". Error : " + (string)strerror(errno) + (string)"\n");
				}
				
			}
			catch (Exception e)
			{
				cerr << "Cannot remove file " << _pathname << ". Error : " << e.GetMessage() << endl;				
			}
		}
		string GetPathname() const 
		{ 
			return _pathname; 
		}
		int GetFileDescriptor() const
		{
			return _fd;
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

#endif /* FILE_H */
