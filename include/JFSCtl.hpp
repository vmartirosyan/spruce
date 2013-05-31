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
#include <linux/types.h>
#include <jfs_dmap.h>
#include <map>
#include <string>
using namespace std;

class JFSCtl
{
public:
	JFSCtl();
	void * ReadBlock(string DeviceName, uint64_t block_no);
	bool WriteBlock(string DeviceName, void * buf, uint64_t block_no);
	struct dinode * GetInode(string DeviceName, string FilePath, bool ReloadFromDisk = false);
	bool SetInode(string DeviceName, string FilePath, struct dinode * inode);	
	struct dinode * GetInode(string DeviceName, int InodeNum, bool ReloadFromDisk = false);
	bool SetInode(string DeviceName, int InodeNum, struct dinode * inode);	
	struct jfs_superblock * GetSuperBlock(string DeviceName, bool ReloadFromDisk = false);
	bool SetSuperBlock(string DeviceName, struct jfs_superblock * SuperBlock);
	int GetInodeNum(string FilePath);
	struct dinomap * GetIAGCP(string);
	bool SetIAGCP(string, dinomap*);
	struct iag * GetIAG(string, int);
	bool SetIAG(string, int, iag*);
	struct dinode * GetAggregateInode(string , int);
	bool SetAggregateInode(string, dinode*, int);
	bool SetBmap(string, dbmap_disk *);
	struct dbmap_disk * GetBmap(string);
	bool SetFirstDmap(string, dmap *);
	struct dmap * GetFirstDmap(string);
	bool SetDmapCtl(string, dmapctl *);
	struct dmapctl * GetDmapCtl(string);
private:
	off64_t LocateInode(string, int);
	off64_t LocateIAGCP(string);
	uint64_t LocateFSIAG(string, int);
	uint64_t LocateBlockMap(string);
	map<int, struct dinode *> _InodeCache;
	struct jfs_superblock * _SuperBlock;
};
