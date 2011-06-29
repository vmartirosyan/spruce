//      DupFileDescriptor.hpp
//
//      Copyright 2011 Shahzadyan Khachik <qwerity@gmail.com>
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

#ifndef TEST_IOCTL_H
#define TEST_IOCTL_H

#include "SyscallTest.hpp"
// Operations
enum IoctlSyscalls
{
    IOCTL_FS_SECRM_FL, /* Secure deletion */
    IOCTL_FS_UNRM_FL,	/* Undelete */
	IOCTL_FS_COMPR_FL,	/* Compress file */
    IOCTL_FS_SYNC_FL,	 /* Synchronous updates */
    IOCTL_FS_IMMUTABLE_FL,	 /* Immutable file */
    IOCTL_FS_APPEND_FL,	 /* writes to file may only append */
    IOCTL_FS_NODUMP_FL,	 /* do not dump file */
    IOCTL_FS_NOATIME_FL,	 /* do not update atime */

    IOCTL_FS_DIRTY_FL,		
    IOCTL_FS_COMPRBLK_FL,	 /* One or more compressed clusters */
    IOCTL_FS_NOCOMP_FL,	 /* Don't compress */
    IOCTL_FS_ECOMPR_FL,	 /* Compression error */

    IOCTL_FS_BTREE_FL,		 /* btree format dir */
    IOCTL_FS_INDEX_FL,		 /* hash-indexed directory */
    IOCTL_FS_IMAGIC_FL,	 /* AFS directory */
    IOCTL_FS_JOURNAL_DATA_FL,	 /* Reserved for ext3 */
    IOCTL_FS_NOTAIL_FL,		 /* file tail should not be merged */
    IOCTL_FS_DIRSYNC_FL,		 /* dirsync behaviour (directories only) */
    IOCTL_FS_TOPDIR_FL,		 /* Top of directory hierarchies*/
    IOCTL_FS_EXTENT_FL,		 /* Extents */
    IOCTL_FS_DIRECTIO_FL,		 /* Use direct i/o */
    IOCTL_FS_RESERVED_FL,		 /* reserved for ext2 lib */
    
    IOCTL_INVALID_FD,
    IOCTL_INVALID_ARGP,

};

class IoctlTest : public SyscallTest
{
	public:
		IoctlTest(Mode mode, int operation, string arguments = "") :
			SyscallTest(mode, operation, arguments, "ioctl"){}
		virtual ~IoctlTest() {}
        
        Status SetGetFlags(int i);
        Status InvalidFD();
        Status InvalidArgp();
	protected:
		virtual int Main(vector<string> args);
};

#endif /* TEST_IOCTL_H */
