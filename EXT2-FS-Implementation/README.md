# EXT2 - File System - Implementation in C

In this project, we emulate the operations of the EXT2 File System by using C:

## Functions Implemented:

- open_volume_file() - Opens the specified file and reads the initial
  EXT2 data contained in the file, including the boot sector, group
  descriptor table, file allocation table and root directory
- close_volume_file() - Frees and closes all resources used by a EXT2 volume
- read_block() - Reads data from one or more blocks
- read_inode() - Fills an inode data structure with the data from one
  inode in disk. Determines the block group number and index within
  the group from the inode number, then reads the inode from the
  inode table in the corresponding group
- read_ind_block_entry() - Reads one entry from an indirect
  block. Returns the block number found in the corresponding entry
- get_inode_block_no() - Returns the block number containing the data
  associated to a particular index
- read_file_block() - Returns the content of a specific file, limited to
  a single block
- follow_directory_entries() - Reads all entries in a directory, calling
  function 'f' for each entry in the directory
- find_file_from_path() - Searches for a file based on its full path
