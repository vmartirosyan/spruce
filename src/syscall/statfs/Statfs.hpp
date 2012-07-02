//      Statfs.hpp
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

#ifndef STATFS_H
#define STATFS_H

#include <sys/types.h>
#include "SyscallTest.hpp"
#include <sys/vfs.h>
#include <linux/limits.h>

// Operations
enum StatfsOps
{
	StatfsNormalFunc,
	StatfsErrAccess,
	StatfsErrFault,
	StatfsErrLoop,
	StatfsErrNameTooLong,
	StatfsErrNoEnt,
	StatfsErrNotDir	
};

using std::map;
using std::pair;

class StatfsTest : public SyscallTest
{			
public:	
	StatfsTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "statfs")
	{
		FileSystemTypesMap["ext4"] = 0xEF53; //EXT4_SUPER_MAGIC;
		FileSystemTypesMap["btrfs"] = -1; // does not support?
		FileSystemTypesMap["xfs"] = 0x58465342; //XFS_SUPER_MAGIC;
		FileSystemTypesMap["jfs"] = 0x3153464a; //JFS_SUPER_MAGIC
		MountAt = "";
		if ( getenv("MountAt") )
			MountAt = getenv("MountAt");
		FileSystem = "";
		if ( getenv("FileSystem") )
			FileSystem = getenv("FileSystem");
	}
	
	Status NormalFunc();
	Status ErrAccess();
	Status ErrFault();
	Status ErrLoop();
	Status ErrNameTooLong();
	Status ErrNoEnt();
	Status ErrNotDir();
	   
protected:
	virtual int Main(vector<string> args);	
	map<string, int> FileSystemTypesMap;
	string MountAt;
	string FileSystem;
	
};
#endif



