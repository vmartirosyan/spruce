/*
 *   Copyright (C) International Business Machines Corp., 2000-2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef _H_JFS_DINODE
#define _H_JFS_DINODE

/*
 *	jfs_dinode.h: on-disk inode manager
 */



/* extended mode bits (on-disk inode di_mode) */
#define IFJOURNAL	0x00010000	/* journalled file */
#define ISPARSE		0x00020000	/* sparse file enabled */
#define INLINEEA	0x00040000	/* inline EA area free */
#define ISWAPFILE	0x00800000	/* file open for pager swap space */

/* more extended mode bits: attributes for OS/2 */
#define IREADONLY	0x02000000	/* no write access to file */
#define IHIDDEN		0x04000000	/* hidden file */
#define ISYSTEM		0x08000000	/* system file */

#define IDIRECTORY	0x20000000	/* directory (shadow of real bit) */
#define IARCHIVE	0x40000000	/* file archive bit */
#define INEWNAME	0x80000000	/* non-8.3 filename format */

#define IRASH		0x4E000000	/* mask for changeable attributes */
#define ATTRSHIFT	25	/* bits to shift to move attribute
				   specification to mode position */

/* extended attributes for Linux */

#define JFS_NOATIME_FL		0x00080000 /* do not update atime */

#define JFS_DIRSYNC_FL		0x00100000 /* dirsync behaviour */
#define JFS_SYNC_FL		0x00200000 /* Synchronous updates */
#define JFS_SECRM_FL		0x00400000 /* Secure deletion */
#define JFS_UNRM_FL		0x00800000 /* allow for undelete */

#define JFS_APPEND_FL		0x01000000 /* writes to file may only append */
#define JFS_IMMUTABLE_FL	0x02000000 /* Immutable file */

#define JFS_FL_USER_VISIBLE	0x03F80000
#define JFS_FL_USER_MODIFIABLE	0x03F80000
#define JFS_FL_INHERIT		0x03C80000

/* These are identical to EXT[23]_IOC_GETFLAGS/SETFLAGS */
#define JFS_IOC_GETFLAGS	_IOR('f', 1, long)
#define JFS_IOC_SETFLAGS	_IOW('f', 2, long)

#define JFS_IOC_GETFLAGS32	_IOR('f', 1, int)
#define JFS_IOC_SETFLAGS32	_IOW('f', 2, int)

#endif /*_H_JFS_DINODE */
