//      ioctl.cpp
//
//      Copyright 2011 Suren Grigoryan <suren.grigoryan@gmail.com>
//
//      This program is free software; you can redistrisbute it and/or modify
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
#include <sys/capability.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "IoctlTest.hpp"
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
            case IOCTL_FS_SYNC_FL:
                return SetGetFlags(IOCTL_FS_SYNC_FL);
            case IOCTL_FS_SECRM_FL:
                return SetGetFlags(FS_SECRM_FL);
            case IOCTL_FS_JOURNAL_DATA_FL:
                return SetGetFlags(FS_JOURNAL_DATA_FL);
            case IOCTL_FS_IMAGIC_FL:
                return SetGetFlags(FS_IMAGIC_FL);
            case IOCTL_FS_INDEX_FL:
                return SetGetFlags(FS_INDEX_FL);
            case IOCTL_FS_BTREE_FL:
                return SetGetFlags(FS_BTREE_FL);
            case IOCTL_FS_RESERVED_FL:
                return SetGetFlags(FS_RESERVED_FL);
            case IOCTL_FS_DIRECTIO_FL:
                return SetGetFlags(IOCTL_FS_DIRECTIO_FL);
            case IOCTL_FS_EXTENT_FL:
                return SetGetFlags(FS_EXTENT_FL);
            case IOCTL_FS_TOPDIR_FL:
                return SetGetFlags(FS_TOPDIR_FL);
            case IOCTL_FS_NOTAIL_FL:
                return SetGetFlags(FS_NOTAIL_FL);
            case IOCTL_FS_DIRSYNC_FL:
                return SetGetFlags(FS_DIRSYNC_FL);
            case IOCTL_FS_COMPRBLK_FL:
                return SetGetFlags(FS_COMPRBLK_FL);
            case IOCTL_FS_DIRTY_FL:
                return SetGetFlags(FS_DIRTY_FL);
            case IOCTL_FS_NOCOMP_FL:
                return SetGetFlags(FS_NOCOMP_FL);
            case IOCTL_FS_NOATIME_FL:
                return SetGetFlags(FS_NOATIME_FL);
            case IOCTL_FS_NODUMP_FL:
                return SetGetFlags(FS_NODUMP_FL);
            case IOCTL_FS_APPEND_FL:
                return SetGetFlags(FS_APPEND_FL);
            case IOCTL_FS_ECOMPR_FL:
                return SetGetFlags(FS_ECOMPR_FL);
            case IOCTL_FS_IMMUTABLE_FL:
                return SetGetFlags(FS_IMMUTABLE_FL);
            case IOCTL_FS_COMPR_FL:
                return SetGetFlags(FS_COMPR_FL);
            case IOCTL_FS_UNRM_FL:
                return SetGetFlags(FS_UNRM_FL);
                
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
        int old_flags = 0;
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

// Tests different ioctl flags
Status IoctlTest::SetGetFlags(int flag) {
    try 
	{		
        File file("newfile");
		int fd = file.GetFileDescriptor();		
        int set_flags = flag; 
        int get_flags = 0;
        int old_flags = 0;
        
		if (ioctl(fd, FS_IOC_GETFLAGS, &old_flags) == -1 ) {
            cerr << "Error backing up old values. " << strerror(errno);
            return Unres;
        }
        
		if (ioctl(fd, FS_IOC_SETFLAGS, &set_flags) == -1 ) {
            cerr << "Error setting flag." << strerror(errno);
            return Unres;
        }
        if (ioctl(fd, FS_IOC_GETFLAGS, &get_flags) == -1 ) {
            cerr << "Error getting newly set flags." << strerror(errno);
            return Unres;
        }
        
        // restore old flags
        if (ioctl(fd, FS_IOC_SETFLAGS, &old_flags) == -1 ) {
            cerr << "Error restoring old values." << strerror(errno);
            return Unres;
        }
                                
        if((get_flags & set_flags) == 0) {
            cerr << "Set get flags does not match. " << strerror(errno);
            return Fail;
        }
        
        return Success;
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
	return Success;
}

