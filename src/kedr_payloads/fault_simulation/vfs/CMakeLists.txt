set(kmodule_name "kedr_fsim_vfs")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

set(functions
# Callback function cannot be currently intercepted by KEDR.
# Also, fail in that function may be triggered by corersponded quotas set in userspace.
# Currently is leaved active.
	"dquot_alloc_inode"
	"filemap_write_and_wait_range"
	"read_cache_page"
# TODO: check code for fault simulation	
	#"write_one_page"
	"__bread"
	"block_write_full_page"
	"mpage_writepages"
	"bh_submit_read"
# Result of that function depends on buffer state, which is affected by the caller. Fault simulation shouldn't be performed here.
# Currently is leaved active.
	"bh_uptodate_or_lock"
	"new_inode"
# Function returns false only if given len is insufficient.
# That situation should be triggered by the corresponded path instead of fault simulation.
# Currently is leaved active.
	"d_path"
# Fail in that function should be triggered by corresponded flags set.
# Also, function should return inconsistent flags(in fieamap object), so it shouldn't fail unpredictably.
# Currently is leaved active.
	"fiemap_check_flags"
)

kmodule_is_function_exist(blkdev_get_by_dev EXISTS_blkdev_get_by_dev)

if(EXISTS_blkdev_get_by_dev)
	list(APPEND functions "blkdev_get_by_dev")
endif(EXISTS_blkdev_get_by_dev)

kmodule_is_function_exist(d_alloc_root EXISTS_d_alloc_root)

if(EXISTS_d_alloc_root)
	list(APPEND functions "d_alloc_root")
endif(EXISTS_d_alloc_root)


kmodule_is_function_exist(d_make_root EXISTS_d_make_root)

if(EXISTS_d_make_root)
	list(APPEND functions "d_make_root")
endif(EXISTS_d_make_root)


# Fail in __d_quot_alloc_space kernel function should be triggered by
# corresponded quotas set in userspace.
# 
# Currently is leaved active.
set(d_quot_alloc_space_message "Check __d_quot_alloc_space kernel function existence and signature")

if(DEFINED __d_quot_alloc_space_HAS_3_ARGS OR DEFINED __d_quot_alloc_space_HAS_4_ARGS)
	set(d_quot_alloc_space_message "${d_quot_alloc_space_message} [cached]")
else(DEFINED __d_quot_alloc_space_HAS_3_ARGS OR DEFINED __d_quot_alloc_space_HAS_4_ARGS)
	kmodule_try_compile(__d_quot_alloc_space_HAS_3_ARGS_tmp
		"${CMAKE_CURRENT_BINARY_DIR}/__d_quot_alloc_space_has_3_args"
		"__d_quot_alloc_space_has_3_args.c")
	if(__d_quot_alloc_space_HAS_3_ARGS_tmp)
		set(__d_quot_alloc_space_HAS_3_ARGS "TRUE" CACHE INTERNAL "Does __d_quot_alloc_space has 3 args?")
	else(__d_quot_alloc_space_HAS_3_ARGS_tmp)
		set(__d_quot_alloc_space_HAS_3_ARGS "FALSE" CACHE INTERNAL "Does __d_quot_alloc_space has 3 args?")
		kmodule_try_compile(__d_quot_alloc_space_HAS_4_ARGS_tmp
			"${CMAKE_CURRENT_BINARY_DIR}/__d_quot_alloc_space_has_4_args"
			"__d_quot_alloc_space_has_4_args.c")
		if(__d_quot_alloc_space_HAS_4_ARGS_tmp)
			set(__d_quot_alloc_space_HAS_4_ARGS "TRUE" CACHE INTERNAL "Does __d_quot_alloc_space has 4 args?")
		else(__d_quot_alloc_space_HAS_4_ARGS_tmp)
			set(__d_quot_alloc_space_HAS_4_ARGS "FALSE" CACHE INTERNAL "Does __d_quot_alloc_space has 4 args?")
		endif(__d_quot_alloc_space_HAS_4_ARGS_tmp)
	endif(__d_quot_alloc_space_HAS_3_ARGS_tmp)
endif(DEFINED __d_quot_alloc_space_HAS_3_ARGS OR DEFINED __d_quot_alloc_space_HAS_4_ARGS)

if(__d_quot_alloc_space_HAS_3_ARGS)
	set(d_quot_alloc_space_message "${d_quot_alloc_space_message} - has 3 args")
	list(APPEND functions "__d_quot_alloc_space")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/__d_quot_alloc_space.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/__d_quot_alloc_space_3_args.data")
elseif(__d_quot_alloc_space_HAS_4_ARGS)
	set(d_quot_alloc_space_message "${d_quot_alloc_space_message} - has 4 args")
	list(APPEND functions "__d_quot_alloc_space")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/__d_quot_alloc_space.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/__d_quot_alloc_space_4_args.data")
else(__d_quot_alloc_space_HAS_3_ARGS)
	set(d_quot_alloc_space_message "${d_quot_alloc_space_message} - not found")
endif(__d_quot_alloc_space_HAS_3_ARGS)

message(STATUS ${d_quot_alloc_space_message})

add_fsim_payload(${kmodule_name} ${functions})
