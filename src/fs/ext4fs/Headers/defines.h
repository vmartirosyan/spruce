#ifndef EXT4_IOC_MOVE_EXT
struct move_extent {
	__u32 reserved;		/* should be zero */
	__u32 donor_fd;		/* donor file descriptor */
	__u64 orig_start;	/* logical start offset in block for orig */
	__u64 donor_start;	/* logical start offset in block for donor */
	__u64 len;		/* block length to be moved */
	__u64 moved_len;	/* moved block length */
};
#define EXT4_IOC_MOVE_EXT		_IOWR('f', 15, struct move_extent)
#endif
#ifndef EXT4_IOC_GROUP_ADD
#define EXT4_IOC_GROUP_ADD		_IOW('f', 8, struct ext4_new_group_input)
#endif
#ifndef EXT4_IOC_MIGRATE
#define EXT4_IOC_MIGRATE		_IO('f', 9)
#endif
#ifndef EXT4_IOC_ALLOC_DA_BLKS
#define EXT4_IOC_ALLOC_DA_BLKS		_IO('f', 12)
#endif
#ifndef EXT4_IOC_WAIT_FOR_READONLY
#define EXT4_IOC_WAIT_FOR_READONLY	_IOR('f', 99, long)
#endif
