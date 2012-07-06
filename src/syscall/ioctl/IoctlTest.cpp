//      IoctlTest.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Grigoryan <suren.grigoryan@gmail.com>
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

#include <linux/fs.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <IoctlTest.hpp>
#include "File.hpp"




int IoctlTest::Main(vector<string> args)
{
    if ( geteuid() != 0 )
	{
		// The tests should be executed by the root user;
		cerr << "Only root can execute these tests..." << endl;
		return Unres;
	}
    
	if ( _mode == Normal )
	{
		switch (_operation)
		{
            case IOCTL_SETGET_VERSION:
				return SetGetVersion();
            case IOCTL_INVALID_FD:
                return InvalidFD();
            case IOCTL_INVALID_ARGP:
                return InvalidArgp();


            default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}

// Tests if the ioctl retruns EBADF when fd is not a valid file descriptor.
Status IoctlTest::InvalidFD() 
{
	try 
	{		
        int old_flags = 0;
        int fd = 0;
        
        // open and close file so the fd gets invalid
		{
            File file("newfile");
            fd = file.GetFileDescriptor();
        }
        
		if (ioctl(fd, FS_IOC_GETFLAGS, &old_flags) != -1 ) {
            cerr << "ioctl should return -1 value for invalid file "
                "descriptor but it doesn't";                
            return Fail;
        } else if (errno != EBADF) {
            cerr << "ioctl should set errno to EBADF, but errno is set to " 
                <<"\"" <<strerror(errno) << "\"";
            return Fail;
        }
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
	return Success;
}

// Tests if the ioctl retruns EFAULT when the argp is NULL.
Status IoctlTest::InvalidArgp() 
{
	try 
	{		
        int fd;
        File file("newfile");
        fd = file.GetFileDescriptor();
        
		if (ioctl(fd, FS_IOC_GETFLAGS, NULL) != -1 ) {
            cerr << "ioctl should return -1 value for inaccessible argp but it "
                "doesn't";                
            return Fail;
        } else if (errno != EFAULT) {
            cerr << "ioctl should set errno to EFAULT, but errno is set to " 
                <<"\"" <<strerror(errno) << "\"";
            return Fail;
        }
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
	return Success;
}


// Tests FS_IOC_GETVERSION, FS_IOC_SETVERSION for ioctl 
Status IoctlTest::SetGetVersion() {
    try 
	{		
        File file("newfile");
		int fd = file.GetFileDescriptor();		
        int set_version = 15; 
        int get_version = 0;
        int old_version = 0;
        
		if (ioctl(fd, FS_IOC_GETVERSION, &old_version) == -1 ) {
            cerr << "Error backing up old values. " << strerror(errno);
            return Unres;
        }
        
		if (ioctl(fd, FS_IOC_SETVERSION, &set_version) == -1 ) {
            cerr << "Error setting flag." << strerror(errno);
            return Unres;
        }
        if (ioctl(fd, FS_IOC_GETVERSION, &get_version) == -1 ) {
            cerr << "Error getting newly set flags." << strerror(errno);
            return Unres;
        }
        
        // restore old flags
        if (ioctl(fd, FS_IOC_SETVERSION, &old_version) == -1 ) {
            cerr << "Error restoring old values." << strerror(errno);
            return Unres;
        }
                                
        if(get_version != set_version) {
            cerr << "Set get flags does not match. set version to  = " << 
				set_version << " get version = " << get_version << " old = " << old_version;
            return Fail;
        }
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
	return Success;
}

