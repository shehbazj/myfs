const int MYFS_MAGIC = 0x14092015;
const int MYFS_DEFAULT_BLOCK_SIZE = 4*1024;

struct myfs_super_block {
	unsigned int version;
	unsigned int magic;
	unsigned int block_size;
	unsigned int free_blocks;
	char padding[ (4*1024) - (4 * sizeof(unsigned int))];	// you want superblock to also be of size 4K
};
