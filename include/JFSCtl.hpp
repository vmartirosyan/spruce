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

class JFSCtl
{
public:
	/* Create object controlled given device. */
	JFSCtl(const std::string& DeviceName);

	/* 
	 * Write content of block with given number into user-provided buffer.
	 * 
	 * Only 'size' bytes will be written.
	 */
	void ReadBlock(uint64_t block_no, void * buf, size_t size);
	
	/*
	 * Write block with given number.
	 * 
	 * Only 'size' bytes will be read from buffer, content of other
	 * 4096-size bytes is unspecified.
	 */
	void WriteBlock(uint64_t block_no, const void * buf, size_t size);
	
	/* Fill dinode structure with content of inode with given number. */
	void GetInode(int InodeNum, struct dinode * inode);

	/* Write inode with given number to device. */
	void SetInode(int InodeNum, const struct dinode * inode);	

	/* Return inode number for given file. */
	static int GetInodeNum(const std::string& FilePath);
	
	/* Fill dinomap structure with content of inode map control page. */
	void GetIAGCP(struct dinomap *);
	/* Write inode map control page */
	void SetIAGCP(const struct dinomap *);
	
	/* 
	 * Fill dinomap structure with content of allocation group page
	 * for inode with given number.
	 */
	void GetIAG(int, struct iag *);
	/* Write allocation group page for given inode. */
	void SetIAG(int, const struct iag *);
	
	/* 
	 * Fill dinode structure with content of aggregate inode with
	 * given number.
	 */
	void GetAggregateInode(int, struct dinode *);
	/* Write aggregate inode with given number. */
	void SetAggregateInode(int, const struct dinode *);
	
	/* 
	 * Fill dbmap_disk structure with content of aggregate disk
	 * allocation map descriptor.
	 */
	void GetBmap(struct dbmap_disk *);
	/* Write aggregate disk allocation map descriptor. */
	void SetBmap(const struct dbmap_disk *);
	
	/* 
	 * Fill dmap structure with content of [first?] aggregate disk
	 * allocation map descriptor.
	 */
	void GetFirstDmap(struct dmap *);
	/* Write [first?] aggregate disk allocation map descriptor. */
	void SetFirstDmap(const struct dmap *);
	
	/* Fill dmapctl structure with content of disk map control page. */
	void GetDmapCtl(struct dmapctl *);
	/* Write disk map control page. */
	void SetDmapCtl(const struct dmapctl *);

private:
	std::string DeviceName;
	
	off64_t LocateInode(int);
	off64_t LocateIAGCP(void);
	uint64_t LocateFSIAG(int);
	uint64_t LocateBlockMap(void);
};
