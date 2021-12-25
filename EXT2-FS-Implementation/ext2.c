#include "ext2.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fsuid.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define EXT2_OFFSET_SUPERBLOCK 1024
#define EXT2_INVALID_BLOCK_NUMBER ((uint32_t) -1)

/* open_volume_file: Opens the specified file and reads the initial
   EXT2 data contained in the file, including the boot sector, group
   descriptor table, file allocation table and root directory.
   
   Parameters:
     filename: Name of the file containing the volume data.
   Returns:
     A pointer to a newly allocated volume_t data structure with all
     fields initialized according to the data in the volume file
     (including superblock and group descriptor table), or NULL if the
     file is invalid or data is missing, or if the file is not an EXT2
     volume file system (s_magic does not contain the correct value).
 */
volume_t *open_volume_file(const char *filename) {
  
  /* TO BE COMPLETED BY THE STUDENT */
  
  int fd = open(filename, O_RDONLY); //file descriptor
  if (fd < 0) {
    return NULL;
  }

  lseek(fd, EXT2_OFFSET_SUPERBLOCK, SEEK_SET);  //seek to superblock

  //Get SuperBlock
  superblock_t *super = malloc(sizeof(superblock_t));
  int chars_read = read(fd, super, sizeof(superblock_t)); 
  
  if (chars_read < 0) {
    free(super);
    return NULL;
  }

  // Initialize volume
  volume_t *volume = malloc(sizeof(volume_t));

  volume->fd = fd;
  volume->super = *super;
  volume->block_size = 1024 << super->s_log_block_size;
  volume->volume_size = super->s_blocks_count * volume->block_size;
  volume->num_groups = super->s_blocks_count % super->s_blocks_per_group == 0 ? 
    super->s_blocks_count / super->s_blocks_per_group : 
    super->s_blocks_count / super->s_blocks_per_group + 1; 
    
  free(super);

  group_desc_t *groups = malloc(sizeof(group_desc_t) * volume->num_groups);

  //read block group descriptor table
  if (volume->block_size >= (EXT2_OFFSET_SUPERBLOCK + 1024)) {
    pread(fd, groups, sizeof(group_desc_t) * volume->num_groups, volume->block_size);
  }
  else {
    pread(fd, groups, sizeof(group_desc_t) * volume->num_groups, EXT2_OFFSET_SUPERBLOCK + volume->block_size);
  }
  
  volume->groups = groups;

  return volume;
}

/* close_volume_file: Frees and closes all resources used by a EXT2 volume.
   
   Parameters:
     volume: pointer to volume to be freed.
 */
void close_volume_file(volume_t *volume) {

  /* TO BE COMPLETED BY THE STUDENT */
  close(volume->fd);
  free(volume);
}

/* read_block: Reads data from one or more blocks. Saves the resulting
   data in buffer 'buffer'. This function also supports sparse data,
   where a block number equal to 0 sets the value of the corresponding
   buffer to all zeros without reading a block from the volume.
   
   Parameters:
     volume: pointer to volume.
     block_no: Block number where start of data is located.
     offset: Offset from beginning of the block to start reading
             from. May be larger than a block size.
     size: Number of bytes to read. May be larger than a block size.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns the number of bytes read from the
     disk. In case of error, returns -1.
 */
ssize_t read_block(volume_t *volume, uint32_t block_no, uint32_t offset,
                   uint32_t size, void *buffer) {

  /* TO BE COMPLETED BY THE STUDENT */
  //https://www.tutorialspoint.com/c_standard_library/c_function_memset.htm
  if (block_no == 0) {
    memset(buffer, 0, size);
    return 0;
  }

  return pread(volume->fd, buffer, size, volume->block_size * block_no + offset);
}

/* read_inode: Fills an inode data structure with the data from one
   inode in disk. Determines the block group number and index within
   the group from the inode number, then reads the inode from the
   inode table in the corresponding group. Saves the inode data in
   buffer 'buffer'.
   
   Parameters:
     volume: pointer to volume.
     inode_no: Number of the inode to read from disk.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns a positive value. In case of error,
     returns -1.
 */
ssize_t read_inode(volume_t *volume, uint32_t inode_no, inode_t *buffer) {
  
  /* TO BE COMPLETED BY THE STUDENT */

  if (inode_no <= 0) return -1;

  int block_group = (inode_no - 1) / volume->super.s_inodes_per_group;
  int inode_index = (inode_no - 1) % volume->super.s_inodes_per_group;
  int inode_offset = inode_index * volume->super.s_inode_size;
  uint32_t inode_table = volume->groups[block_group].bg_inode_table;

  return read_block(volume, inode_table, inode_offset, volume->super.s_inode_size, buffer);
}

/* read_ind_block_entry: Reads one entry from an indirect
   block. Returns the block number found in the corresponding entry.
   
   Parameters:
     volume: pointer to volume.
     ind_block_no: Block number for indirect block.
     index: Index of the entry to read from indirect block.

   Returns:
     In case of success, returns the block number found at the
     corresponding entry. In case of error, returns
     EXT2_INVALID_BLOCK_NUMBER.
 */
static uint32_t read_ind_block_entry(volume_t *volume, uint32_t ind_block_no,
				     uint32_t index) {
  
  /* TO BE COMPLETED BY THE STUDENT */
  uint32_t block_no;
  if (read_block(volume, ind_block_no, index * sizeof(uint32_t), sizeof(uint32_t), &block_no) > 0) {
    return block_no;
  } 
  return EXT2_INVALID_BLOCK_NUMBER;
    
}

/* read_inode_block_no: Returns the block number containing the data
   associated to a particular index. For indices 0-11, returns the
   direct block number; for larger indices, returns the block number
   at the corresponding indirect block.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure where data is to be sourced.
     index: Index to the block number to be searched.

   Returns:
     In case of success, returns the block number to be used for the
     corresponding entry. This block number may be 0 (zero) in case of
     sparse files. In case of error, returns
     EXT2_INVALID_BLOCK_NUMBER.
 */
static uint32_t get_inode_block_no(volume_t *volume, inode_t *inode, uint64_t block_idx) {
  
  /* TO BE COMPLETED BY THE STUDENT */

  //direct blocks
  if (block_idx < 12) { 
    return inode->i_block[block_idx];
  }

  // ex: In a 1KiB block size, there would be 256 indirect blocks per doubly-indirect block, with 256 direct blocks
  // per indirect block for a total of 65536 blocks per doubly-indirect block.
  uint64_t single_ind_entries = volume->block_size / sizeof(uint32_t);    //every block number is 32 bits long
  uint64_t double_ind_entries = single_ind_entries * single_ind_entries;
  uint64_t triple_ind_entries = single_ind_entries * single_ind_entries * single_ind_entries;

  uint64_t single_ind_start_idx = 12;
  uint64_t double_ind_start_idx = single_ind_start_idx + single_ind_entries;
  uint64_t triple_ind_start_idx = double_ind_start_idx + double_ind_entries;
  uint64_t max_block_idx = triple_ind_start_idx + triple_ind_entries - 1;

    //single indirect blocks
  if (block_idx >= single_ind_start_idx && block_idx < double_ind_start_idx) {
    return read_ind_block_entry(volume, inode->i_block_1ind, block_idx - single_ind_start_idx);
  }

  //double indirect blocks
  else if (block_idx >= double_ind_start_idx && block_idx < triple_ind_start_idx) {
    uint32_t ind_block_idx = (block_idx - single_ind_start_idx)/(single_ind_entries) - 1;  
    uint32_t ind_block_no = read_ind_block_entry(volume, inode->i_block_2ind, ind_block_idx);
    uint32_t direct_block_idx = (block_idx - double_ind_start_idx) % single_ind_entries;
    return read_ind_block_entry(volume, ind_block_no, direct_block_idx);
  }
  
  //triple indirect blocks
  else if (block_idx >= triple_ind_start_idx && block_idx <= max_block_idx) {
    uint32_t double_ind_block_idx = (block_idx - double_ind_start_idx)/(double_ind_entries) - 1;
    uint32_t double_ind_block_no = read_ind_block_entry(volume, inode->i_block_3ind, double_ind_block_idx);
    uint32_t single_ind_block_idx = (block_idx - double_ind_start_idx)/(double_ind_entries) % single_ind_entries;
    uint32_t single_ind_block_no = read_ind_block_entry(volume, double_ind_block_no, single_ind_block_idx);
    uint32_t direct_block_idx = (block_idx - double_ind_start_idx) % single_ind_entries;
    return read_ind_block_entry(volume, single_ind_block_no, direct_block_idx);
  }

  return EXT2_INVALID_BLOCK_NUMBER;
}

/* read_file_block: Returns the content of a specific file, limited to
   a single block. If the number of bytes requested spans beyond the
   limit of the block, returns only the bytes within that block's
   boundaries.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the file.
     offset: Offset, in bytes from the start of the file, of the data
             to be read.
     max_size: Maximum number of bytes to read from the block.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns the number of bytes read from the
     disk. In case of error, returns -1.
 */
ssize_t read_file_block(volume_t *volume, inode_t *inode, uint64_t offset,
                        uint64_t max_size, void *buffer) {
    
  /* TO BE COMPLETED BY THE STUDENT */

  uint32_t block_no = get_inode_block_no(volume, inode, offset/volume->block_size);
  return read_block(volume, block_no, offset % volume->block_size, max_size, buffer);
}

/* read_file_content: Returns the content of a specific file, limited
   to the size of the file only. May need to read more than one block,
   with data not necessarily stored in contiguous blocks.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the file.
     offset: Offset, in bytes from the start of the file, of the data
             to be read.
     max_size: Maximum number of bytes to read from the file.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns the number of bytes read from the
     disk. In case of error, returns -1.
 */
ssize_t read_file_content(volume_t *volume, inode_t *inode, uint64_t offset, uint64_t max_size, void *buffer) {

  uint32_t read_so_far = 0;

  if (offset + max_size > inode_file_size(volume, inode))
    max_size = inode_file_size(volume, inode) - offset;
  
  while (read_so_far < max_size) {
    int rv = read_file_block(volume, inode, offset + read_so_far,
			     max_size - read_so_far, buffer + read_so_far);
    if (rv <= 0) return rv;
    read_so_far += rv;
  }
  return read_so_far;
}

/* follow_directory_entries: Reads all entries in a directory, calling
   function 'f' for each entry in the directory. Stops when the
   function returns a non-zero value, or when all entries have been
   traversed.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the directory.
     context: This pointer is passed as an argument to function 'f'
              unmodified.
     buffer: If function 'f' returns non-zero for any file, and this
             pointer is set to a non-NULL value, this buffer is set to
             the directory entry for which the function returned a
             non-zero value. If the pointer is NULL, nothing is
             saved. If none of the existing entries returns non-zero
             for 'f', the value of this buffer is unspecified.
     f: Function to be called for each directory entry. Receives three
        arguments: the file name as a NULL-terminated string, the
        inode number, and the context argument above.

   Returns:
     If the function 'f' returns non-zero for any directory entry,
     returns the inode number for the corresponding entry. If the
     function returns zero for all entries, or the inode is not a
     directory, or there is an error reading the directory data,
     returns 0 (zero).
 */
uint32_t follow_directory_entries(volume_t *volume, inode_t *inode, void *context,
				  dir_entry_t *buffer,
				  int (*f)(const char *name, uint32_t inode_no, void *context)) {

  /* TO BE COMPLETED BY THE STUDENT */
  
  //check if inode mode is a directory
  if ((inode->i_mode & 0x4000) != 0x4000) return 0;

  int file_size = inode_file_size(volume, inode);
  int file_offset = 0;
  dir_entry_t *dir = malloc(sizeof(dir_entry_t));  

  while (file_offset < file_size) {
    ssize_t bytes_read = read_file_content(volume, inode, file_offset, sizeof(dir_entry_t), dir);
    if (bytes_read == -1 || dir->de_inode_no == 0) break;

    //https://stackoverflow.com/questions/7648911/are-ext2-directory-entry-names-guaranteed-to-be-null-terminated-on-a-valid-file/10789448
    //Get a NULL terminated string since name of dir_entry is not always NULL terminated
    char file_name[dir->de_name_len + 1]; 
    memcpy(file_name, dir->de_name, dir->de_name_len);
    file_name[dir->de_name_len] = '\0';

    int f_out = f(file_name, dir->de_inode_no, context);
    if (f_out != 0) {
      if (buffer != NULL) {
        memcpy(buffer, dir, sizeof(dir_entry_t));
      }     
      int inode_no = dir->de_inode_no;
      free(dir);
      return inode_no;
    }

    file_offset += dir->de_rec_len;
  }

  free(dir);
  return 0;
}

/* Simple comparing function to be used as argument in find_file_in_directory function */
static int compare_file_name(const char *name, uint32_t inode_no, void *context) {
  return !strcmp(name, (char *) context);
}

/* find_file_in_directory: Searches for a file in a directory.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the directory.
     name: NULL-terminated string for the name of the file. The file
           name must match this name exactly, including case.
     buffer: If the file is found, and this pointer is set to a
             non-NULL value, this buffer is set to the directory entry
             of the file. If the pointer is NULL, nothing is saved. If
             the file is not found, the value of this buffer is
             unspecified.

   Returns:
     If the file exists in the directory, returns the inode number
     associated to the file. If the file does not exist, or the inode
     is not a directory, or there is an error reading the directory
     data, returns 0 (zero).
 */
uint32_t find_file_in_directory(volume_t *volume, inode_t *inode, const char *name,
                                dir_entry_t *buffer) {
  
  return follow_directory_entries(volume, inode, (char *) name, buffer, compare_file_name);
}

/* find_file_from_path: Searches for a file based on its full path.
   
   Parameters:
     volume: Pointer to volume.
     path: NULL-terminated string for the full absolute path of the
           file. Must start with '/' character. Path components
           (subdirectories) must be delimited by '/'. The root
           directory can be obtained with the string "/".
     dest_inode: If the file is found, and this pointer is set to a
                 non-NULL value, this buffer is set to the inode of
                 the file. If the pointer is NULL, nothing is
                 saved. If the file is not found, the value of this
                 buffer is unspecified.

   Returns:
     If the file exists, returns the inode number associated to the
     file. If the file does not exist, or there is an error reading
     any directory or inode in the path, returns 0 (zero).     
 */
uint32_t find_file_from_path(volume_t *volume, const char *path, inode_t *dest_inode) {
  /* TO BE COMPLETED BY THE STUDENT */

  // eg /FOO/BAR/BUZZ.TXT

  inode_t *inode = malloc(sizeof(inode_t));  
  uint32_t inode_no = EXT2_ROOT_INO;  //The root directory is Inode 2
  read_inode(volume, inode_no, inode);

  char *path_editable = malloc(strlen(path));
  memcpy(path_editable, path, strlen(path));
  char delim[2] = "/";

  // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm 
  char *token = strtok(path_editable, delim);

  if (token == NULL) {
    if (inode_no != 0 && dest_inode != NULL) {
      memcpy(dest_inode, inode, sizeof(inode_t));
    }
    free(inode);
    free(path_editable);
    return inode_no;
  }

  dir_entry_t *dir = malloc(sizeof(dir_entry_t));

  while (token != NULL) {
    inode_no = find_file_in_directory(volume, inode, token, dir);
    if (inode_no == 0) {
      break;
    }

    read_inode(volume, inode_no, inode);
    token = strtok(NULL, delim);
  }
  
  if (inode_no != 0 && dest_inode != NULL) {
    memcpy(dest_inode, inode, sizeof(inode_t));
  }

  free(inode);
  free(dir);
  free(path_editable);
  
  return inode_no;
}
