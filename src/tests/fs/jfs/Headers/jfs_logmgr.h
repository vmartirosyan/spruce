/*
 *   Copyright (C) International Business Machines Corp., 2000-2004
 *   Portions Copyright (C) Christoph Hellwig, 2001-2002
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
#ifndef	_H_JFS_LOGMGR
#define _H_JFS_LOGMGR

#include "jfs_filsys.h"
//#include "jfs_lock.h"

/*
 *	log manager configuration parameters
 */

/* log page size */
#define	LOGPSIZE	4096
#define	L2LOGPSIZE	12

#define LOGPAGES	16	/* Log pages per mounted file system */

/*
 *	log logical volume
 *
 * a log is used to make the commit operation on journalled
 * files within the same logical volume group atomic.
 * a log is implemented with a logical volume.
 * there is one log per logical volume group.
 *
 * block 0 of the log logical volume is not used (ipl etc).
 * block 1 contains a log "superblock" and is used by logFormat(),
 * lmLogInit(), lmLogShutdown(), and logRedo() to record status
 * of the log but is not otherwise used during normal processing.
 * blocks 2 - (N-1) are used to contain log records.
 *
 * when a volume group is varied-on-line, logRedo() must have
 * been executed before the file systems (logical volumes) in
 * the volume group can be mounted.
 */
/*
 *	log superblock (block 1 of logical volume)
 */
#define	LOGSUPER_B	1
#define	LOGSTART_B	2

#define	LOGMAGIC	0x87654321
#define	LOGVERSION	1

#define MAX_ACTIVE	128	/* Max active file systems sharing log */

struct logsuper {
	__le32 magic;		/* 4: log lv identifier */
	__le32 version;		/* 4: version number */
	__le32 serial;		/* 4: log open/mount counter */
	__le32 size;		/* 4: size in number of LOGPSIZE blocks */
	__le32 bsize;		/* 4: logical block size in byte */
	__le32 l2bsize;		/* 4: log2 of bsize */

	__le32 flag;		/* 4: option */
	__le32 state;		/* 4: state - see below */

	__le32 end;		/* 4: addr of last log record set by logredo */
	char uuid[16];		/* 16: 128-bit journal uuid */
	char label[16];		/* 16: journal label */
	struct {
		char uuid[16];
	} active[MAX_ACTIVE];	/* 2048: active file systems list */
};

#define NULL_UUID "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

/* log flag: commit option (see jfs_filsys.h) */

/* log state */
#define	LOGMOUNT	0	/* log mounted by lmLogInit() */
#define LOGREDONE	1	/* log shutdown by lmLogShutdown().
				 * log redo completed by logredo().
				 */
#define LOGWRAP		2	/* log wrapped */
#define LOGREADERR	3	/* log read error detected in logredo() */

#endif				/* _H_JFS_LOGMGR */
