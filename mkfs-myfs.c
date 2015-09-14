#include"my.h"

#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	ssize_t ret;
	struct myfs_super_block sb = {
		.version = 1,
		.magic = MYFS_MAGIC,
		.block_size = MYFS_DEFAULT_BLOCK_SIZE,
		.free_blocks = ~0,
	};

	if(argc!= 2){
		printf("Usage: mkfs-myfs <dev>\n");
		return 0;
	}else{
		fd = open(argv[1], O_RDWR);
		if(fd == -1){
			printf("Error Opening Device\n");
			return -1;
		}
	}

	ret = write(fd, (char *)&sb, sizeof(sb));
	if(ret != MYFS_DEFAULT_BLOCK_SIZE){
		printf("Error formatting device\n");
	}else{
		printf("Device Formatted successfully\n");
	}
	close(fd);
	return 0;
}
