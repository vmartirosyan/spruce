//      Mount_Umount.hpp
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

#ifndef MOUNT_H
#define MOUNT_H

#include "SyscallTest.hpp"

// Operations
/*enum MountOperations
{
	UmountErrNameTooLong, //done
	MountErrNameTooLong, //done
	MountErrEbusy,    //done
	MountErrEFault,  //done
	MountErrELoop,   //done
	MountErrEInval1, //done
	MountErrEInval2, 
	MountErrENotblk, //done
	MountErrEPerm,   //done
	MountErrENoent,  //done
	MountErrENodev,  //done
	MountErrENotdir, //done
	
};*/

class Mount_Umount : public SyscallTest
{			
public:	
	Mount_Umount(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Mount_Umount")
	{	
		
		long size;
		_cwd = NULL;
		size = pathconf(".", _PC_PATH_MAX);

		if ((_cwd = (char *)malloc((size_t)size)) != NULL)
			_cwd = getcwd(_cwd, (size_t)size);
		if ( getenv("Partition") )
		{
			DeviceName = getenv("Partition");
		}
		if ( getenv("MountAt") )
		{
			MountPoint = getenv("MountAt");    
		}
		if(getenv("FileSystem"))
		{
			FileSystemType = getenv("FileSystem");
		}
	}
	char* _cwd;
	char* DeviceName;
	char* MountPoint;
	char* FileSystemType;
protected:
	virtual ~Mount_Umount() {}	
	Status MountUmountGeneralFunc();	
	Status UmountErrNameTooLongFunc();
	Status MountErrNameTooLongFunc();
	Status MountErrEbusyFunc();
	Status MountErrEAccesFunc();
	Status MountErrEFaultFunc();
	Status MountErrELoopFunc();
	Status MountErrEInval1Func();
	Status MountErrEInval2Func();
	Status MountErrENotblkFunc();
	Status MountErrEPermFunc ();
	Status MountErrENoentFunc ();
	Status MountErrENodevFunc ();
	Status MountErrENotdirFunc ();
	
	virtual int Main(vector<string> args);	
};
#endif
