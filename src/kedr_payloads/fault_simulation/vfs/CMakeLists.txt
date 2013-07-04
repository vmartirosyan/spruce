set(kmodule_name "kedr_fsim_vfs")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

set(functions
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


# Fail in dquot_alloc_inode kernel function should be triggered by
# corersponded quotas set in userspace.
# Currently is leaved active.
set(dquot_alloc_inode_message "Check dquot_alloc_inode kernel function signature")

if(DEFINED dquot_alloc_inode_HAS_1_ARG OR DEFINED dquot_alloc_inode_HAS_2_ARGS)
	set(dquot_alloc_inode_message "${dquot_alloc_inode_message} [cached]")
else(DEFINED dquot_alloc_inode_HAS_1_ARG OR DEFINED dquot_alloc_inode_HAS_2_ARGS)
	kmodule_try_compile(dquot_alloc_inode_HAS_1_ARG_tmp
		"${CMAKE_CURRENT_BINARY_DIR}/dquot_alloc_inode_has_1_arg"
		"dquot_alloc_inode_has_1_arg.c")
	if(dquot_alloc_inode_HAS_1_ARG_tmp)
		set(dquot_alloc_inode_HAS_1_ARG "TRUE" CACHE INTERNAL "Does dquot_alloc_inode has 1 arg?")
	else(dquot_alloc_inode_HAS_1_ARG_tmp)
		set(dquot_alloc_inode_HAS_1_ARG "FALSE" CACHE INTERNAL "Does dquot_alloc_inode has 1 arg?")
		kmodule_try_compile(dquot_alloc_inode_HAS_2_ARGS_tmp
			"${CMAKE_CURRENT_BINARY_DIR}/dquot_alloc_inode_has_2_args"
			"dquot_alloc_inode_has_2_args.c")
		if(dquot_alloc_inode_HAS_2_ARGS_tmp)
			set(dquot_alloc_inode_HAS_2_ARGS "TRUE" CACHE INTERNAL "Does dquot_alloc_inode has 4 args?")
		else(dquot_alloc_inode_HAS_2_ARGS_tmp)
			set(dquot_alloc_inode_HAS_2_ARGS "FALSE" CACHE INTERNAL "Does dquot_alloc_inode has 4 args?")
		endif(dquot_alloc_inode_HAS_2_ARGS_tmp)
	endif(dquot_alloc_inode_HAS_1_ARG_tmp)
endif(DEFINED dquot_alloc_inode_HAS_1_ARG OR DEFINED dquot_alloc_inode_HAS_2_ARGS)

if(dquot_alloc_inode_HAS_1_ARG)
	set(dquot_alloc_inode_message "${dquot_alloc_inode_message} - has 1 arg")
	list(APPEND functions "dquot_alloc_inode")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/dquot_alloc_inode.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/dquot_alloc_inode_1_arg.data")
elseif(dquot_alloc_inode_HAS_2_ARGS)
	set(dquot_alloc_inode_message "${dquot_alloc_inode_message} - has 2 args")
	list(APPEND functions "dquot_alloc_inode")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/dquot_alloc_inode.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/dquot_alloc_inode_2_args.data")
else(dquot_alloc_inode_HAS_1_ARG)
	set(dquot_alloc_inode_message "${dquot_alloc_inode_message} - not found")
	# Function should exists
	message(FATAL_ERROR ${dquot_alloc_inode_message})
endif(dquot_alloc_inode_HAS_1_ARG)

message(STATUS ${dquot_alloc_inode_message})


# Fail in __dquot_alloc_space kernel function should be triggered by
# corresponded quotas set in userspace.
# 
# Currently is leaved active.
set(__dquot_alloc_space_message "Check __dquot_alloc_space kernel function existence and signature")

if(DEFINED __dquot_alloc_space_HAS_3_ARGS OR DEFINED __dquot_alloc_space_HAS_4_ARGS)
	set(__dquot_alloc_space_message "${__dquot_alloc_space_message} [cached]")
else(DEFINED __dquot_alloc_space_HAS_3_ARGS OR DEFINED __dquot_alloc_space_HAS_4_ARGS)
	kmodule_try_compile(__dquot_alloc_space_HAS_3_ARGS_tmp
		"${CMAKE_CURRENT_BINARY_DIR}/__dquot_alloc_space_has_3_args"
		"__dquot_alloc_space_has_3_args.c")
	if(__dquot_alloc_space_HAS_3_ARGS_tmp)
		set(__dquot_alloc_space_HAS_3_ARGS "TRUE" CACHE INTERNAL "Does __dquot_alloc_space has 3 args?")
	else(__dquot_alloc_space_HAS_3_ARGS_tmp)
		set(__dquot_alloc_space_HAS_3_ARGS "FALSE" CACHE INTERNAL "Does __dquot_alloc_space has 3 args?")
		kmodule_try_compile(__dquot_alloc_space_HAS_4_ARGS_tmp
			"${CMAKE_CURRENT_BINARY_DIR}/__dquot_alloc_space_has_4_args"
			"__dquot_alloc_space_has_4_args.c")
		if(__dquot_alloc_space_HAS_4_ARGS_tmp)
			set(__dquot_alloc_space_HAS_4_ARGS "TRUE" CACHE INTERNAL "Does __dquot_alloc_space has 4 args?")
		else(__dquot_alloc_space_HAS_4_ARGS_tmp)
			set(__dquot_alloc_space_HAS_4_ARGS "FALSE" CACHE INTERNAL "Does __dquot_alloc_space has 4 args?")
		endif(__dquot_alloc_space_HAS_4_ARGS_tmp)
	endif(__dquot_alloc_space_HAS_3_ARGS_tmp)
endif(DEFINED __dquot_alloc_space_HAS_3_ARGS OR DEFINED __dquot_alloc_space_HAS_4_ARGS)

if(__dquot_alloc_space_HAS_3_ARGS)
	set(__dquot_alloc_space_message "${__dquot_alloc_space_message} - has 3 args")
	list(APPEND functions "__dquot_alloc_space")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/__dquot_alloc_space.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/__dquot_alloc_space_3_args.data")
elseif(__dquot_alloc_space_HAS_4_ARGS)
	set(__dquot_alloc_space_message "${__dquot_alloc_space_message} - has 4 args")
	list(APPEND functions "__dquot_alloc_space")
	rule_copy_file("${CMAKE_CURRENT_BINARY_DIR}/__dquot_alloc_space.data"
		"${CMAKE_CURRENT_SOURCE_DIR}/__dquot_alloc_space_4_args.data")
else(__dquot_alloc_space_HAS_3_ARGS)
	set(__dquot_alloc_space_message "${__dquot_alloc_space_message} - not found")
endif(__dquot_alloc_space_HAS_3_ARGS)

message(STATUS ${__dquot_alloc_space_message})

add_fsim_payload(${kmodule_name} ${functions})
