//      JFSCtl.cpp
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

#include <JFSCtl.hpp>
#include <sys/stat.h>

JFSCtl::JFSCtl():
	_SuperBlock(NULL)
	{}

struct dinode * JFSCtl::GetInode(string DeviceName, string FilePath, bool ReloadFromDisk)
{
	int InodeNum = GetInodeNum(FilePath);
	
	if ( InodeNum == -1 )
		return NULL;
		
	return GetInode(DeviceName, InodeNum, ReloadFromDisk);
}

bool JFSCtl::SetInode(string DeviceName, string FilePath, struct dinode * inode)
{
	int InodeNum = GetInodeNum(FilePath);
	
	if ( InodeNum == -1 )
		return false;
		
	return SetInode(DeviceName, InodeNum, inode);
}

struct dinode * JFSCtl::GetInode(string DeviceName, int InodeNum, bool ReloadFromDisk)
{
	// First check the cache.
	if ( !ReloadFromDisk && _InodeCache.find(InodeNum) != _InodeCache.end() )
		return _InodeCache[InodeNum];
		
	// Now get the real dinode
	try
	{
		off64_t InodeAddr = LocateInode(DeviceName, InodeNum);
		if ( InodeAddr == -1 )
			throw Exception("Cannot locate inode on disk");
			
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek( fd, InodeAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to file inode." + string(strerror(errno)));
		}
		struct dinode * FileInode = new dinode;
		if ( read ( fd, FileInode, sizeof(dinode) ) == -1)
		{
			throw Exception("Cannot read file inode from disk." + string(strerror(errno)));
		}
		
		// Add the inode to the cache
		_InodeCache[InodeNum] = FileInode;
		
		return FileInode;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetInode: Cannot get inode. " + e.GetMessage());
		return NULL;
	}
}
bool JFSCtl::SetInode(string DeviceName, int InodeNum, struct dinode * inode)
{
	try
	{
		off64_t InodeAddr = LocateInode(DeviceName, InodeNum);
		if ( InodeAddr == -1 )
			throw Exception("Cannot locate inode on disk");
			
		File f(DeviceName, S_IRUSR, O_WRONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek( fd, InodeAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to file inode." + string(strerror(errno)));
		}
		
		if ( write ( fd, inode, sizeof(dinode) ) == -1)
		{
			throw Exception("Cannot write file inode from disk." + string(strerror(errno)));
		}
		
		// Add the inode to the cache
		_InodeCache[InodeNum] = inode;
		
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::SetInode: Cannot get inode. " + e.GetMessage());
		return false;
	}
}
	
struct jfs_superblock * JFSCtl::GetSuperBlock(string DeviceName, bool ReloadFromDisk)
{
	// First check if the super block is already loaded
	// and if there is no need to reload from disk then return it
	if ( _SuperBlock == NULL )
		_SuperBlock = new jfs_superblock;
	else if ( !ReloadFromDisk )
		return _SuperBlock;
		
	// Now get the superblock from disk
	try
	{
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek( fd, SUPER1_OFF, SEEK_SET  ) == -1 )
			throw Exception("JFSCtl::GetSuperBlock: Cannot seek to superblock start. " + (string)strerror(errno));
			
		if ( read ( fd, _SuperBlock, sizeof(jfs_superblock) ) != sizeof(jfs_superblock) )
			throw Exception("JFSCtl::GetSuperBlock: Cannot read superblock from device. " + (string)strerror(errno));
			
		return _SuperBlock;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetSuperBlock: Cannot get superblock. " + e.GetMessage());
		return NULL;
	}
}


bool JFSCtl::SetSuperBlock(string DeviceName, struct jfs_superblock * SuperBlock)
{
	if ( _SuperBlock == NULL )
		_SuperBlock = new jfs_superblock;
		
	if ( SuperBlock != _SuperBlock )
		memcpy(_SuperBlock, SuperBlock, sizeof(jfs_superblock));
		
	try
	{
		File f(DeviceName, S_IWUSR, O_WRONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek( fd, SUPER1_OFF, SEEK_SET  ) == -1 )
			throw Exception("Cannot seek to superblock start. " + (string)strerror(errno));
			
		if ( write ( fd, _SuperBlock, sizeof(jfs_superblock) ) != sizeof(jfs_superblock) )
			throw Exception("Cannot write superblock to device. " + (string)strerror(errno));
			
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetSuperBlock: Cannot get superblock. " + e.GetMessage());
		return false;
	}
}

int JFSCtl::GetInodeNum(string FilePath)
{
	struct stat st;
	if ( stat(FilePath.c_str(), &st) == -1 )
		return -1;
		
	return st.st_ino;
}

off64_t JFSCtl::LocateInode(string DeviceName, int InodeNum)
{
	try
	{
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
			
		const int INODERPERIAG = EXTSPERIAG*32;
		int iag_key = (InodeNum / INODERPERIAG) * INODERPERIAG + 4096;
		int iag_inode_index = InodeNum % INODERPERIAG;
		int inode_ext_desc = iag_inode_index / 32;
		int inode_offset = iag_inode_index % 32 * sizeof(dinode);
		
		struct dinode _16_th_inode;
		if ( lseek( fd, SUPER1_OFF + PSIZE + iag_key + sizeof(iag) + 16*sizeof(dinode), SEEK_SET ) == -1 )
			throw Exception("JFSCtl::GetInode: Cannot seek to 16-th inode in aggregate IAG. " + (string)strerror(errno));
			
		if ( read ( fd, &_16_th_inode, sizeof(dinode) ) != sizeof(dinode) )
			throw Exception("JFSCtl::GetInode: Cannot read 16-th inode in aggregate IAG. " + (string)strerror(errno));
			
		uint64_t FileSetIAGBlock = 0;
		xad x[8];
		memcpy(x, _16_th_inode.di_xtroot, 8*sizeof(xad));
		// Find a non-zero address
		for ( int j = 0; j < 8; ++j )
		{
			if ( x[j].addr1 != 0 || x[j].addr2 != 0 )
			{			
				FileSetIAGBlock = x[j].addr1;
				FileSetIAGBlock <<= 32;
				FileSetIAGBlock += x[j].addr2;
				
				break;
			}
		}
		
		if ( FileSetIAGBlock == 0 )
		{
			throw Exception("Cannot get FileSet IAG Address." + string(strerror(errno)));
		}
		
		uint64_t FileSetIAGAddr = ( FileSetIAGBlock + 1 )* PSIZE; // Skip the control page at once
		if ( lseek( fd, FileSetIAGAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to FileSet IAG Address." + string(strerror(errno)));
		}
		struct iag FileSetIag;
		if ( read ( fd, &FileSetIag, sizeof(FileSetIag) ) == -1)
		{
			throw Exception("Cannot read FileSet IAG from disk." + string(strerror(errno)));
		}
		
		pxd_t ext_addr = FileSetIag.inoext[inode_ext_desc];
		
		return ext_addr.addr2*PSIZE + inode_offset;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetSuperBlock: Cannot get superblock. " + e.GetMessage());
		return -1;
	}
}
