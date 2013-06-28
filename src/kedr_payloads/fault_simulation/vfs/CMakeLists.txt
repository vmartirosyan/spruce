set(kmodule_name "kedr_fsim_vfs")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

add_fsim_payload(${kmodule_name}
	#"d_alloc_root"
	"d_make_root"
	"__dquot_alloc_space"
	"dquot_alloc_inode"
	"filemap_write_and_wait_range"
	"read_cache_page"
	#"write_one_page"
	"blkdev_get_by_dev"
	"__bread"
	"block_write_full_page"
	"mpage_writepages"
	#"bh_submit_read"
	#"bh_uptodate_or_lock"
	"new_inode"
	"d_path"
	"fiemap_check_flags"
)
