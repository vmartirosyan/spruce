//      Mount_Umount.cpp
//      
// 		Copyright (C) 2011, Institute for System Programming	
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Karen Tsirunyan <ktsirunyan@gmail.com>
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


#include <Mount_Umount.hpp>
#include <stdlib.h>
#include <cstring>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/param.h>
#include <errno.h>
#include "File.hpp"
#include <unistd.h>
#include "Directory.hpp"

#include <fcntl.h>
#include "StatFile.hpp"


int Mount_Umount::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		if(DeviceName == NULL || MountPoint == NULL || FileSystemType == NULL)
		{
			cerr<< "Error in getting partition name, mount point or file system type";
			return Unres;
		}  
		   
		switch (_operation)
		{
			case MountUmountGeneral:			
				return MountUmountGeneralFunc();	
			case UmountErrNameTooLong :
				return UmountErrNameTooLongFunc();	
			case MountErrNameTooLong :
				return MountErrNameTooLongFunc();	
			case MountErrEbusy :
				return MountErrEbusyFunc();
			case MountErrEAcces :
				return MountErrEAccesFunc();
			case MountErrEFault :
				return 	MountErrEFaultFunc ();
			case MountErrELoop :
				return MountErrELoopFunc ();
			case MountErrEInval1 :
				return MountErrEInval1Func ();
				case MountErrEInval2 :
				return MountErrEInval2Func ();
			case MountErrENotblk:
				return 	MountErrENotblkFunc ();
			case MountErrEPerm:
				return 	MountErrEPermFunc ();
			case MountErrENoent:
				return 	MountErrENoentFunc ();
			case MountErrENodev:
				return MountErrENodevFunc ();
			case MountErrENotdir:
				return MountErrENotdirFunc ();
			
			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}


               
Status Mount_Umount::MountUmountGeneralFunc ()
{
	int ret_value;
	// changing directory for later unmounting the mount point
	if( chdir("/") != 0)
	{
		cerr<< "Cannot change directory" << strerror(errno);
		return Unres;		
	}	
	ret_value = umount(MountPoint);
	if ( ret_value == -1 )
	{
		cerr << "Error in unmounting partition: " << strerror(errno);		
		return Fail;
	}
	if(ret_value)
	// trying to reumount unmounted target
	ret_value = umount(MountPoint);
	if ( ret_value != -1 )
	{
		cerr << "Error: unmounted partition has been unmounted again: ";		
		return Fail;
	}
	if( errno != EINVAL )
	{
				
		cerr << "Incorrect error set in errno in case of incorrect mount point: "<<strerror(errno);
		return Fail;
	}
    
	return Success;

}
               


Status Mount_Umount::UmountErrNameTooLongFunc ()
{
	int ret_umount;
	char longname[MAXPATHLEN + 2];
	for ( int i = 0; i <= MAXPATHLEN + 1; ++i ) 
	{
		longname[i] = 'a';
	}
	ret_umount = umount( longname );
	if ( ret_umount == 0 )
	{
		cerr << "unmount returns 0 in case of too long  path name";
	    return Fail;
	} 
  	if( errno != ENAMETOOLONG )
	{
				
		cerr << "Incorrect error set in errno in case of too long path name (Umount): "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

/*int mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data);
*/
Status Mount_Umount::MountErrNameTooLongFunc ()
{
	int ret_mount;
	char longname[MAXPATHLEN + 2];
	for ( int i = 0; i <= MAXPATHLEN + 1; ++i ) 
	{
		longname[i] = 'a';
	}
	ret_mount = mount( DeviceName, longname, FileSystemType, 0, 0);
	if ( ret_mount == 0 )
	{
		cerr << "mount returns 0 in case of too long  path name";
	    return Fail;
	} 
  	if( errno != ENAMETOOLONG )
	{
				
		cerr << "Incorrect error set in errno in case of too long path name (Mount): "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status Mount_Umount::MountErrEbusyFunc ()
{			
	int ret_mount;
	ret_mount = mount( DeviceName, MountPoint, FileSystemType, 0, 0);
	if ( ret_mount == 0 )
	{
		cerr << "mount returns 0 in case of already mounted partition";
	    return Fail;
	} 
  	if( errno != EBUSY )
	{
		cerr << "Incorrect error set in errno in case of mounting an already mounted partition: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}


Status Mount_Umount::MountErrEAccesFunc ()
{
	int ret_mount;
	int ret_chmod;
	string dirPath = (string)_cwd + "/mountTestDirectory";
	string path= dirPath  + "/mountTest.txt";
	
	try
	{
		Directory dir(dirPath);
		File file(path);
		ret_chmod = chmod(path.c_str(), 0666);
		if(ret_chmod != 0)
		{
			cerr << "Cannot change mode of file: "<<strerror(errno);
			return Unres;
		}
							
		ret_mount = mount( DeviceName, dirPath.c_str(), FileSystemType, 0, 0);
		if ( ret_mount == 0 )
		{
			cerr << "mount returns 0 in case of no access file. " ;
			return Fail;
		} 
		if( errno != EACCES )
		{				
			cerr << "Incorrect error set in errno in case of mounting an already mounted partition: "<< strerror(errno);
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	return Success;
}


Status Mount_Umount::MountErrEFaultFunc ()
{

	int ret_mount;		
	
    //(char*)-1 is outside of accessible address space
	ret_mount = mount (DeviceName, (char*)-1, FileSystemType, 0, 0);
	if(ret_mount != -1)
	{
			cerr << "Mount didn't return Fail in case of non acessible address. ";
			return Fail;
	}
	else
	{
		if(errno != EFAULT)
		{
				cerr << "Incorrect error set in errno in case of EFAULT: "<<strerror(errno);
				return Fail;
		}
	}
	return Success;
}

Status Mount_Umount::MountErrELoopFunc ()
{

	int ret_mount;	
	const char* source = "/";	
	ret_mount = mount(source, MountPoint, FileSystemType, MS_MOVE, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when move was attempted and target was a descendant of source ";
			return Fail;
	}
	if(errno != ELOOP)
	{
			cerr << "Incorrect error set in errno in case of ELOOP: "<<strerror(errno);
			return Fail;
	}

	return Success;
}

Status Mount_Umount::MountErrEInval1Func ()
{

	int ret_mount;	
	ret_mount = mount("dummy", MountPoint, FileSystemType, MS_REMOUNT, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when move was attepted and source was not mounted on target";
			return Fail;
	}
	if(errno != EINVAL)
	{
			cerr << "Incorrect error set in errno in case of EINVAL: " << strerror(errno);
			return Fail;
	}

	return Success;
}
Status Mount_Umount::MountErrEInval2Func ()
{

	int ret_mount;	
	const char* source = "/";	
	ret_mount = mount(source, MountPoint, FileSystemType, MS_MOVE, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when move was attepted and source was '/' ";
			return Fail;
	}
	if(errno != EINVAL)
	{
			cerr << "Incorrect error set in errno in case of EINVAL: " << strerror(errno);
			return Fail;
	}

	return Success;
}

Status Mount_Umount::MountErrENotblkFunc ()
{

	int ret_mount;	
	const char* source = "/";	
	ret_mount = mount(source, MountPoint, FileSystemType, 0, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when move was attepted and source was not mounted on target";
			return Fail;
	}
	if(errno != ENOTBLK)
	{
			cerr << "Incorrect error set in errno in case of ENOTBLK: " << strerror(errno);
			return Fail;
	}

	return Success;
}

Status Mount_Umount::MountErrENoentFunc ()
{

	int ret_mount;	
	ret_mount = mount("dummy", MountPoint, FileSystemType, 0, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when source was empty. ";
			return Fail;
	}
	if(errno != ENOENT)
	{
			cerr << "Incorrect error set in errno in case of ENOENT: "<<strerror(errno);
			return Fail;
	}

	return Success;
}

Status Mount_Umount::MountErrENodevFunc ()
{

	int ret_mount;	
	ret_mount = mount(DeviceName, MountPoint, "NonexistingFS", 0, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case of non existing FS type ";
			return Fail;
	}
	if(errno != ENODEV)
	{
			cerr << "Incorrect error set in errno in case of ENODEV: "<<strerror(errno);
			return Fail;
	}

	return Success;
}


Status Mount_Umount::MountErrENotdirFunc ()
{
	try
	{
		string path = "/mountNOTDIRTest.txt";
		File file(path);
		int ret_mount;	
		
		ret_mount = mount( DeviceName, path.c_str(), FileSystemType, 0, 0);
		if(ret_mount == 0)
		{
			cerr << "Mount returns Success in case when target is not a directory";
			return Fail;
		}
		if(errno != ENOTDIR)
		{
			cerr << "Incorrect error set in errno in case of ENOTDIR: "<<strerror(errno);
			return Fail;
		}
		
		return Success;
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}

	
}


Status Mount_Umount::MountErrEPermFunc ()
{
	struct passwd * noBody;
	if((noBody = getpwnam("nobody")) == NULL)
	{
		cerr<< "Cannot get struct nobody";
		return Unres;
	}
	if (setuid(noBody->pw_uid) != 0)
	{
        cerr<<"Cannot set uid";
        return Unres;
    }
	int ret_mount;	
	ret_mount = mount(DeviceName, MountPoint, FileSystemType, 0, 0);
	if(ret_mount == 0)
	{
			cerr << "Mount returns Success in case when the user is set to NOBODY";
			return Fail;
	}
	if(errno != EPERM)
	{
			cerr << "Incorrect error set in errno in case of EPERM: "<<strerror(errno);
			return Fail;
	}

	return Success;
}

