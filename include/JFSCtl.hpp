//      JFSCtl.hpp
//      
//      Copyright (C) 2011-2013, Institute for System Programming
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

#include <File.hpp>
#include <Exception.hpp>
#include <Logger.hpp>
#include <jfs_dinode.h>
#include <jfs_superblock.h>
#include <jfs_filsys.h>
#include <jfs_imap.h>
#include <map>
#include <string>
using namespace std;

class JFSCtl
{
public:
	JFSCtl();
	struct dinode * GetInode(string DeviceName, string FilePath, bool ReloadFromDisk = false);
	bool SetInode(string DeviceName, string FilePath, struct dinode * inode);
	
	struct dinode * GetInode(string DeviceName, int InodeNum, bool ReloadFromDisk = false);
	bool SetInode(string DeviceName, int InodeNum, struct dinode * inode);
	
	struct jfs_superblock * GetSuperBlock(string DeviceName, bool ReloadFromDisk = false);
	bool SetSuperBlock(string DeviceName, struct jfs_superblock * SuperBlock);
	int GetInodeNum(string FilePath);
	dinomap * GetIAGCP(string, int);
	bool SetIAGCP(string, int, dinomap*);
private:
	off64_t LocateInode(string, int);
	off64_t LocateIAGCP(string, int);
	uint64_t LocateFSIAG(int, int);
	map<int, struct dinode *> _InodeCache;
	struct jfs_superblock * _SuperBlock;
};
