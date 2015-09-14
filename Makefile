obj-m := myfs.o
myfs-objs := my.o

all: ko mkfs-myfs

mkfs-myfs:
	gcc mkfs-myfs.c my.h -o mkfs-myfs

ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf mkfs-myfs
