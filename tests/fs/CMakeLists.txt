cmake_minimum_required(VERSION 2.4)

message("Configuring tests for FS specific tests.")

add_subdirectory (btrfs)
add_subdirectory (ext4fs)
add_subdirectory (jfs)
add_subdirectory (reiserfs)
add_subdirectory (xfs)

project (Spruce)


