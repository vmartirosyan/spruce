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
	
bool JFSCtl::WriteBlock(string DeviceName, void * buf, uint64_t block_no)
{
	try
	{
		File f(DeviceName, S_IWUSR, O_WRONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek64( fd, block_no * PSIZE, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to file inode." + string(strerror(errno)));
		}
		
		if ( write ( fd, buf, PSIZE ) == -1)
		{
			throw Exception("Cannot write block to disk." + string(strerror(errno)));
		}
		
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::WriteBlock: Cannot write block. " + e.GetMessage());
		return false;
	}
}

void * JFSCtl::ReadBlock(string DeviceName, uint64_t block_no)
{
	try
	{
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		char * buf = new char[PSIZE];
		
		if ( buf == NULL )
		{
			throw Exception("Cannot allocate memory. " + string(strerror(errno)));
		}
		
		if ( lseek64( fd, block_no * PSIZE, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to file inode. " + string(strerror(errno)));
		}
		
		if ( read ( fd, buf, PSIZE ) == -1)
		{
			throw Exception("Cannot read block from disk. " + string(strerror(errno)));
		}
		
		return buf;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::ReadBlock: Cannot read block. " + e.GetMessage());
		return NULL;
	}
}

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

int JFSCtl::GetInodeNum(string FilePath)
{
	struct stat st;
	if ( stat(FilePath.c_str(), &st) == -1 )
		return -1;

	return st.st_ino;
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
		
		if ( lseek64( fd, InodeAddr, SEEK_SET ) == -1)
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
		
		if ( lseek64( fd, InodeAddr, SEEK_SET ) == -1)
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
		Logger::LogError("JFSCtl::SetInode: Cannot set inode. " + e.GetMessage());
		return false;
	}
}

dinomap * JFSCtl::GetIAGCP(string DeviceName)
{
	try
	{
		off64_t IagCPAddr = LocateIAGCP(DeviceName);
		if ( IagCPAddr == -1 )
			throw Exception("Cannot locate iag control page on disk");
			
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek64( fd, IagCPAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to iag control page." + string(strerror(errno)));
		}
		struct dinomap * IagCP = new dinomap;
		if ( read ( fd, IagCP, sizeof(dinomap) ) == -1)
		{
			throw Exception("Cannot read iag control page from disk." + string(strerror(errno)));
		}
		
		
		
		return IagCP;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetIAGCP: Cannot get iag control page. " + e.GetMessage());
		return NULL;
	}
}

bool JFSCtl::SetIAGCP(string DeviceName, dinomap *IAGCP)
{
	try
	{
		off64_t IagCPAddr = LocateIAGCP(DeviceName);
			throw Exception("Cannot locate iag control page on disk");
			
		File f(DeviceName, S_IRUSR, O_WRONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek64( fd, IagCPAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to iag control page." + string(strerror(errno)));
		}
		
		if ( write ( fd, IAGCP, sizeof(dinomap) ) == -1)
		{
			throw Exception("Cannot write iag control page to disk." + string(strerror(errno)));
		}
		
		
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::SetIAGCP: Cannot set iag control page. " + e.GetMessage());
		return false;
	}
}

iag * JFSCtl::GetIAG(string DeviceName, int InodeNum)
{
	try
	{
		const int INODERPERIAG = EXTSPERIAG*32;
		int iag_key = (InodeNum / INODERPERIAG);
		
		
		uint64_t FileSetIAGBlock = LocateFSIAG(DeviceName, iag_key);
		if ( FileSetIAGBlock == 0 )
		{
			throw Exception("Cannot get FileSet IAG Address." + string(strerror(errno)));
		}
		
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		uint64_t FileSetIAGAddr = ( FileSetIAGBlock )* PSIZE;
		
		
		if( iag_key == 0 )
		{
			FileSetIAGAddr += PSIZE;		 // Skip the control page at once
		}
		
		if ( lseek64( fd, FileSetIAGAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to FileSet IAG Address." + string(strerror(errno)));
		}
		struct iag *FileSetIag = new iag;
		if ( read ( fd, FileSetIag, sizeof(iag) ) == -1)
		{
			throw Exception("Cannot read FileSet IAG from disk." + string(strerror(errno)));
		}
		
		return FileSetIag;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetIAG: Cannot get iag . " + e.GetMessage());
		return NULL;
	}
}

bool JFSCtl::SetIAG(string DeviceName, int InodeNum, iag *IAG)
{
	try
	{
		const int INODERPERIAG = EXTSPERIAG*32;
		int iag_key = (InodeNum / INODERPERIAG);
		
		
		uint64_t FileSetIAGBlock = LocateFSIAG(DeviceName, iag_key);
		if ( FileSetIAGBlock == 0 )
		{
			throw Exception("Cannot get FileSet IAG Address." + string(strerror(errno)));
		}
		File f(DeviceName, S_IRUSR, O_RDWR);
		int fd = f.GetFileDescriptor();
		
		uint64_t FileSetIAGAddr = ( FileSetIAGBlock )* PSIZE;
		if( iag_key == 0 )
		{
			FileSetIAGAddr += PSIZE;		 // Skip the control page at once
		}
		if ( lseek64( fd, FileSetIAGAddr, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to FileSet IAG Address." + string(strerror(errno)));
		}
		if ( write ( fd, IAG, sizeof(iag) ) == -1)
		{
			throw Exception("Cannot write iag to disk." + string(strerror(errno)));
		}
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::SetIAG: Cannot set iag. " + e.GetMessage());
		return false;
	}
}

struct dbmap_disk * JFSCtl::GetBmap(string DeviceName)
{
	try
	{
		
		uint64_t BlockMap = LocateBlockMap(DeviceName);
		if ( BlockMap == 0 )
		{
			throw Exception("Cannot get block allocation map." + string(strerror(errno)));
		}
		
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( lseek64( fd, BlockMap, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to block allocation map Address." + string(strerror(errno)));
		}
		struct dbmap_disk *bmapCP = new dbmap_disk;
		if ( read ( fd, bmapCP, sizeof(dbmap_disk) ) == -1)
		{
			throw Exception("Cannot read bmap from disk." + string(strerror(errno)));
		}
		
		return bmapCP;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetBmap: Cannot get iag . " + e.GetMessage());
		return NULL;
	}
}

bool JFSCtl::SetBmap(string DeviceName, dbmap_disk *bmapCP)
{
	try
	{
		uint64_t BlockMap = LocateBlockMap(DeviceName);
		if ( BlockMap == 0 )
		{
			throw Exception("Cannot get block allocation map." + string(strerror(errno)));
		}
		File f(DeviceName, S_IRUSR, O_RDWR);
		int fd = f.GetFileDescriptor();
		
		if ( lseek64( fd, BlockMap, SEEK_SET ) == -1)
		{
			throw Exception("Cannot seek to block allocation map Address." + string(strerror(errno)));
		}
		if ( write ( fd, bmapCP, sizeof(dbmap_disk) ) == -1)
		{
			throw Exception("Cannot write bmap to disk." + string(strerror(errno)));
		}
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::SetIAG: Cannot set iag. " + e.GetMessage());
		return false;
	}
}

dinode * JFSCtl::GetAggregateInode(string DeviceName, int n)
{
	try
	{
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
			
		struct dinode *_n_th_inode = new dinode;
		if ( lseek64( fd, SUPER1_OFF + 2*PSIZE + sizeof(iag) + n*sizeof(dinode), SEEK_SET ) == -1 )
			throw Exception("JFSCtl::GetAggregateInode: Cannot seek to aggregate inode. " + (string)strerror(errno));
			
		if ( read ( fd, _n_th_inode, sizeof(dinode) ) != sizeof(dinode) )
			throw Exception("JFSCtl::GetAggregateInode: Cannot read aggregate inode. " + (string)strerror(errno));
			
		
		return _n_th_inode;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::GetAggregateInode: Cannot get aggregate inode. " + e.GetMessage());
		return NULL;
	}
}

bool JFSCtl::SetAggregateInode(string DeviceName, dinode *_n_th_inode, int n)
{
	try
	{
		File f(DeviceName, S_IRUSR, O_RDWR);
		int fd = f.GetFileDescriptor();
			
		if ( lseek64( fd, SUPER1_OFF + 2*PSIZE + sizeof(iag) + n*sizeof(dinode), SEEK_SET ) == -1 )
			throw Exception("JFSCtl::SetAggregateInode: Cannot seek to aggregate inode. " + (string)strerror(errno));
			
		if ( write ( fd, _n_th_inode, sizeof(dinode) ) != sizeof(dinode) )
			throw Exception("JFSCtl::SetAggregateInode: Cannot write to aggregate inode. " + (string)strerror(errno));
			
		
		return true;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::SetAggregateInode: Cannot set aggregate inode." + e.GetMessage());
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
		
		if ( lseek64( fd, SUPER1_OFF, SEEK_SET  ) == -1 )
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
		
		if ( lseek64( fd, SUPER1_OFF, SEEK_SET  ) == -1 )
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

off64_t JFSCtl::LocateInode(string DeviceName, int InodeNum)
{
	try
	{			
		const int INODERPERIAG = EXTSPERIAG*32;
		int iag_inode_index = InodeNum % INODERPERIAG;
		int inode_ext_desc = iag_inode_index / 32;
		int inode_offset = iag_inode_index % 32 * sizeof(dinode);

		struct iag* FileSetIag  = GetIAG(DeviceName, InodeNum);
		if(FileSetIag == NULL)
		{
			throw Exception("JFSCtl::LocateInode: Cannot get IAG. " + (string)strerror(errno));
		}
		
		
		pxd_t ext_addr = FileSetIag->inoext[inode_ext_desc];
		
		off64_t addr = ext_addr.addr1;
		addr = addr << 32;
		addr += ext_addr.addr2;
		
		return addr * PSIZE + inode_offset;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::LocateInode: Cannot locate inode. " + e.GetMessage());
		return -1;
	}
}


off64_t JFSCtl::LocateIAGCP(string DeviceName)
{
	try
	{	
		uint64_t FileSetIAGBlock = LocateFSIAG(DeviceName, 0);
		
		if ( FileSetIAGBlock == 0 )
		{
			throw Exception("Cannot get FileSet IAG Address." + string(strerror(errno)));
		}
		File f(DeviceName, S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		uint64_t FileSetIAGCP = ( FileSetIAGBlock)* PSIZE; 

		return FileSetIAGCP;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::LocateIAGCP: Cannot locate IAG control page. " + e.GetMessage());
		return -1;
	}
}

uint64_t JFSCtl::LocateFSIAG(string DeviceName, int iag_key)
{
	try
	{
		
		struct dinode *_16_th_inode = GetAggregateInode(DeviceName, 16);
		
		if ( _16_th_inode == NULL )
		{
			throw Exception("Cannot get 16th inode." + string(strerror(errno)));
		}
		
		uint64_t FileSetIAGBlock = 0;
		xad x[8];
		memcpy(x, _16_th_inode->di_xtroot, 8*sizeof(xad));
		
		FileSetIAGBlock = x[XTENTRYSTART+iag_key].addr1;
		FileSetIAGBlock <<= 32;
		FileSetIAGBlock += x[XTENTRYSTART+iag_key].addr2;
		if ( FileSetIAGBlock == 0 )
		{
			throw Exception("Cannot get FileSet IAG Address." + string(strerror(errno)));
		}
		return FileSetIAGBlock;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::LocateFSIAG: Cannot locate Fileset IAG block. " + e.GetMessage());
		return 0;
	}
}

uint64_t JFSCtl::LocateBlockMap(string DeviceName)
{
	try
	{
		
		struct dinode *_2_nd_inode = GetAggregateInode(DeviceName, 2);
		
		if ( _2_nd_inode == NULL )
		{
			throw Exception("Cannot get 2nd inode." + string(strerror(errno)));
		}
		
		
		uint64_t BlockMapBlock = 0;
		xad x[8];
		memcpy(x, _2_nd_inode->di_xtroot, 8*sizeof(xad));
		
		BlockMapBlock = x[XTENTRYSTART].addr1;
		BlockMapBlock <<= 32;
		BlockMapBlock += x[XTENTRYSTART].addr2;
		if ( BlockMapBlock == 0 )
		{
			throw Exception("Cannot get block allocation map Address." + string(strerror(errno)));
		}
		return BlockMapBlock * PSIZE;
	}
	catch(Exception e)
	{
		Logger::LogError("JFSCtl::LocateBlockMap: Cannot locate block allocation map. " + e.GetMessage());
		return 0;
	}
}
