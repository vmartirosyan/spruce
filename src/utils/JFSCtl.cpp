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

using namespace std;

JFSCtl::JFSCtl(const std::string& DeviceName): DeviceName(DeviceName) {}

void JFSCtl::ReadBlock(uint64_t block_no, void* buf, size_t size)
{
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, block_no * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::ReadBlock: Cannot seek to file inode");
	}
	
	if ( read ( fd, buf, size ) == -1)
	{
		throw Exception("JFSCtl::ReadBlock: Cannot read block from disk");
	}
}

	
void JFSCtl::WriteBlock(uint64_t block_no, const void * buf, size_t size)
{
	File f(DeviceName, S_IWUSR, O_WRONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, block_no * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::WriteBlock: Cannot seek to file inode");
	}
	
	if ( write ( fd, buf, size ) == -1)
	{
		throw Exception("JFSCtl::WriteBlock: Cannot write block to disk");
	}
}


int JFSCtl::GetInodeNum(string FilePath)
{
	struct stat st;
	if ( stat(FilePath.c_str(), &st) == -1 )
		return -1;

	return st.st_ino;
}

void JFSCtl::GetInode(int InodeNum, struct dinode * inode)
{
	off64_t InodeAddr = LocateInode(InodeNum);
		
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, InodeAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetInode: Cannot seek to file inode");
	}
	if ( read ( fd, inode, sizeof(dinode) ) == -1)
	{
		throw Exception("JFSCtl::SetInode: Cannot read file inode from disk");
	}
}

void JFSCtl::SetInode(int InodeNum, const struct dinode * inode)
{
	off64_t InodeAddr = LocateInode(InodeNum);
			
	File f(DeviceName, S_IRUSR, O_WRONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, InodeAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetInode: Cannot seek to file inode");
	}
	
	if ( write ( fd, inode, sizeof(dinode) ) == -1)
	{
		throw Exception("JFSCtl::SetInode: Cannot write file inode from disk");
	}
}

void JFSCtl::GetIAGCP(struct dinomap * IAGCP)
{
	off64_t IagCPAddr = LocateIAGCP();
	if ( IagCPAddr == -1 )
		throw Exception("JFSCtl::GetIAGCP: Cannot locate iag control page on disk");
			
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, IagCPAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::GetIAGCP: Cannot seek to iag control page");
	}
	if ( read ( fd, IAGCP, sizeof(dinomap) ) == -1)
	{
		throw Exception("JFSCtl::GetIAGCP: Cannot read iag control page from disk");
	}
}

void JFSCtl::SetIAGCP(const struct dinomap *IAGCP)
{
	off64_t IagCPAddr = LocateIAGCP();
		
	File f(DeviceName, S_IRUSR, O_WRONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, IagCPAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetIAGCP: Cannot seek to iag control page");
	}
	
	if ( write ( fd, IAGCP, sizeof(dinomap) ) == -1)
	{
		throw Exception("JFSCtl::SetIAGCP: Cannot write iag control page to disk");
	}
}

void JFSCtl::GetIAG(int InodeNum, struct iag * IAG)
{
	const int INODERPERIAG = EXTSPERIAG*32;
	int iag_key = (InodeNum / INODERPERIAG);
	
	uint64_t FileSetIAGBlock = LocateFSIAG(iag_key);
	
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	uint64_t FileSetIAGAddr = ( FileSetIAGBlock )* PSIZE;
	
	if( iag_key == 0 )
	{
		FileSetIAGAddr += PSIZE;		 // Skip the control page at once
	}
	
	if ( lseek64( fd, FileSetIAGAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::GetIAG: Cannot seek to FileSet IAG address");
	}

	if ( read ( fd, IAG, sizeof(iag) ) == -1)
	{
		throw Exception("JFSCtl::GetIAG: Cannot read FileSet IAG from disk");
	}
}

void JFSCtl::SetIAG(int InodeNum, const struct iag *IAG)
{
	const int INODERPERIAG = EXTSPERIAG*32;
	int iag_key = (InodeNum / INODERPERIAG);
	
	uint64_t FileSetIAGBlock = LocateFSIAG(iag_key);

	File f(DeviceName, S_IRUSR, O_RDWR);
	int fd = f.GetFileDescriptor();
	
	uint64_t FileSetIAGAddr = ( FileSetIAGBlock )* PSIZE;
	if( iag_key == 0 )
	{
		FileSetIAGAddr += PSIZE;		 // Skip the control page at once
	}
	if ( lseek64( fd, FileSetIAGAddr, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetIAG: Cannot seek to FileSet IAG address");
	}
	if ( write ( fd, IAG, sizeof(iag) ) == -1)
	{
		throw Exception("JFSCtl::SetIAG: Cannot write iag to disk");
	}
}

void JFSCtl::GetBmap(struct dbmap_disk * bmapCP)
{
	uint64_t BlockMap = LocateBlockMap();
	
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::GetBmap: Cannot seek to block allocation map address");
	}

	if ( read ( fd, bmapCP, sizeof(dbmap_disk) ) == -1)
	{
		throw Exception("JFSCtl::GetBmap: Cannot read bmap from disk");
	}
}

void JFSCtl::SetBmap(const struct dbmap_disk * bmapCP)
{
	uint64_t BlockMap = LocateBlockMap();

	File f(DeviceName, S_IRUSR, O_RDWR);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetBmap: Cannot seek to block allocation map address");
	}
	if ( write ( fd, bmapCP, sizeof(dbmap_disk) ) == -1)
	{
		throw Exception("JFSCtl::SetBmap: Cannot write bmap to disk");
	}
}

void JFSCtl::GetFirstDmap(struct dmap * dmap1)
{
	uint64_t BlockMap = LocateBlockMap();
	
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap + 4 * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::GetFirstDmap: Cannot seek to dmap address");
	}

	if ( read ( fd, dmap1, sizeof(dmap) ) == -1)
	{
		throw Exception("JFSCtl::GetFirstDmap: Cannot read dmap from disk");
	}
}

void JFSCtl::SetFirstDmap(const struct dmap *dmap1)
{
	uint64_t BlockMap = LocateBlockMap();

	File f(DeviceName, S_IRUSR, O_RDWR);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap + 4 * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetFirstDmap: Cannot seek to dmap address");
	}
	if ( write ( fd, dmap1, sizeof(dmap) ) == -1)
	{
		throw Exception("JFSCtl::SetFirstDmap: Cannot write dmap to disk");
	}
}

void JFSCtl::GetDmapCtl(struct dmapctl * dmc)
{
	uint64_t BlockMap = LocateBlockMap();
	
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap + 3 * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::GetDmapCtl: Cannot seek to dmapctl address");
	}
	if ( read ( fd, dmc, sizeof(dmapctl) ) == -1)
	{
		throw Exception("JFSCtl::GetDmapCtl: Cannot read dmapctl from disk");
	}
}

void JFSCtl::SetDmapCtl(const struct dmapctl *dmc)
{
	uint64_t BlockMap = LocateBlockMap();

	File f(DeviceName, S_IRUSR, O_RDWR);
	int fd = f.GetFileDescriptor();
	
	if ( lseek64( fd, BlockMap + 3 * PSIZE, SEEK_SET ) == -1)
	{
		throw Exception("JFSCtl::SetDmapCtl: Cannot seek to dmapctl address");
	}
	if ( write ( fd, dmc, sizeof(dmapctl) ) == -1)
	{
		throw Exception("JFSCtl::SetDmapCtl: Cannot write dmapctl to disk");
	}
}

void JFSCtl::GetAggregateInode(int n, struct dinode * _n_th_inode)
{
	File f(DeviceName, S_IRUSR, O_RDONLY);
	int fd = f.GetFileDescriptor();
		
	if ( lseek64( fd, SUPER1_OFF + 2*PSIZE + sizeof(iag) + n*sizeof(dinode), SEEK_SET ) == -1 )
		throw Exception("JFSCtl::GetAggregateInode: Cannot seek to aggregate inode");
		
	if ( read ( fd, _n_th_inode, sizeof(dinode) ) != sizeof(dinode) )
		throw Exception("JFSCtl::GetAggregateInode: Cannot read aggregate inode");
}

void JFSCtl::SetAggregateInode(int n, const struct dinode *_n_th_inode)
{
	File f(DeviceName, S_IRUSR, O_RDWR);
	int fd = f.GetFileDescriptor();
		
	if ( lseek64( fd, SUPER1_OFF + 2*PSIZE + sizeof(iag) + n*sizeof(dinode), SEEK_SET ) == -1 )
		throw Exception("JFSCtl::SetAggregateInode: Cannot seek to aggregate inode");
		
	if ( write ( fd, _n_th_inode, sizeof(dinode) ) != sizeof(dinode) )
		throw Exception("JFSCtl::SetAggregateInode: Cannot write to aggregate inode");
}

off64_t JFSCtl::LocateInode(int InodeNum)
{
	const int INODERPERIAG = EXTSPERIAG*32;
	int iag_inode_index = InodeNum % INODERPERIAG;
	int inode_ext_desc = iag_inode_index / 32;
	int inode_offset = iag_inode_index % 32 * sizeof(dinode);

	struct iag FileSetIag;
	GetIAG(InodeNum, &FileSetIag);
	
	pxd_t ext_addr = FileSetIag.inoext[inode_ext_desc];
	
	off64_t addr = ext_addr.addr1;
	addr = addr << 32;
	addr += ext_addr.addr2;
	
	return addr * PSIZE + inode_offset;
}


off64_t JFSCtl::LocateIAGCP(void)
{
	uint64_t FileSetIAGBlock = LocateFSIAG(0);
	
	uint64_t FileSetIAGCP = FileSetIAGBlock * PSIZE; 

	return FileSetIAGCP;
}

uint64_t JFSCtl::LocateFSIAG(int iag_key)
{
	struct dinode _16_th_inode;
	GetAggregateInode(16, &_16_th_inode);
	
	uint64_t FileSetIAGBlock = 0;
	xad x[8];
	memcpy(x, _16_th_inode.di_xtroot, 8*sizeof(xad));
	
	FileSetIAGBlock = x[XTENTRYSTART+iag_key].addr1;
	FileSetIAGBlock <<= 32;
	FileSetIAGBlock += x[XTENTRYSTART+iag_key].addr2;
	if ( FileSetIAGBlock == 0 )
	{
		throw Exception("JFSCtl::LocateFSIAG: Cannot get FileSet IAG address");
	}
	return FileSetIAGBlock;
}

uint64_t JFSCtl::LocateBlockMap(void)
{
	struct dinode _2_nd_inode;
	
	GetAggregateInode(2, &_2_nd_inode);
	
	uint64_t BlockMapBlock = 0;
	xad x[8];
	memcpy(x, _2_nd_inode.di_xtroot, 8*sizeof(xad));
	
	BlockMapBlock = x[XTENTRYSTART].addr1;
	BlockMapBlock <<= 32;
	BlockMapBlock += x[XTENTRYSTART].addr2;
	if ( BlockMapBlock == 0 )
	{
		throw Exception("FSCtl::LocateBlockMap: Cannot get block allocation map address");
	}
	return BlockMapBlock * PSIZE;
}
