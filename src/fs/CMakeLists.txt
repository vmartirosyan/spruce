message("-- Configuring filesystem specific sources.")

add_subdirectory (btrfs)
add_subdirectory (ext4fs)
add_subdirectory (jfs)
add_subdirectory (xfs)

