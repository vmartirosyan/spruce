//      File.hpp
//      
//      Copyright 2011 Narek Saribekyan <narek.saribekyan@gmail.com>
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

using std::string;	

// Class representing a linux file system file
// Example usage:
//		File file("newfile");
//		int fd = file.GetFileDescriptor();
class File
{	
	public:
		File(string pathname, mode_t mode = (mode_t)(S_IRUSR | S_IWUSR)) : 
		_pathname(pathname),
		_mode(mode)
		{
			_fd = creat(pathname.c_str(), _mode);
			if (_fd == -1)
			{								
				throw Exception("Cannot create file " + _pathname + 
				": error = " + (string)strerror(errno));
			}
		}
		~File()
		{			
			try
			{
				if (unlink(_pathname.c_str()) != 0)
				{
					throw Exception("Cannot delete file " + _pathname + 
					": error = " + (string)strerror(errno));
				}
				if (close(_fd) != 0)
				{
					throw Exception("Cannot close file descriptor for " + _pathname + 
					": error = " + (string)strerror(errno));
				}
			}
			catch (...)
			{
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
};

#endif /* FILE_H */
