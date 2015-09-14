#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include "my.h"
#include <linux/buffer_head.h>

/*
Function creates, configures and returns an inode.
*/

struct inode *myfs_get_inode (struct super_block *sb,
				const struct inode *dir, umode_t mode,
				dev_t dev)
{
	struct inode *inode = new_inode(sb); // assigns new inode to a sb.
	
	printk("%s()\n",__func__);
	if(inode){
		inode->i_ino = get_next_ino();	// get a inode number. this should not be 0. 
		inode_init_owner(inode, dir, mode);	// assign uid, gid and mode for new inode
		
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;	// ctime - change time - last time file attributes were changed
											// mtime - modify time - last time file content was changed
											// atime - access time - last time file was accessed
		switch(mode & S_IFMT){
			case S_IFDIR:
				printk("Incrementing the inode dir link count");
				inc_nlink(inode);
				break;
			
			case S_IFREG:
			case S_IFLNK:
			default:
				printk(KERN_ERR "MyFS cannot create root directory\n");
		
			return NULL;
			break;
		}	
	}
	return inode;
}

static int myfs_iterate(struct file *filp, struct dir_context *ctx /*, void *dirent, filldir_t filldir*/)
{
	return 0;
}

static struct file_operations myfs_dir_ops = {
	.owner = THIS_MODULE,
	.iterate = myfs_iterate,
};

struct dentry *myfs_lookup(struct inode *parent_inode,
		struct dentry *child_dentry, unsigned int flags)
{
	return NULL;
}

static struct inode_operations myfs_inode_ops = {
	.lookup = myfs_lookup,
};

int myfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct buffer_head *bh;
	struct myfs_super_block *sb_disk;
	bh = (struct buffer_head *)sb_bread(sb,0); // read 0th sector into sb. sb_bread(device, superblock, sector)
	sb_disk = (struct myfs_super_block *)bh->b_data; // assign buffer head data (superblock) to disk sb structure
	brelse(bh);		// free buffer head

	printk(KERN_INFO "Magic Number Obtained = [%d]\n", sb_disk->magic);
	if(sb_disk ->magic != MYFS_MAGIC) {
		printk("Disk Magic Mismatch\n");
		return -EPERM;
	}

	if(sb_disk ->block_size != MYFS_DEFAULT_BLOCK_SIZE){
		printk(KERN_ERR "MyFS seems to be formatted with a different block size [%d]\n", sb_disk ->block_size);
		return -EPERM;
	}

	printk(KERN_INFO "device dectected with version [%d], magic [%x], block_size[%d], free_blocks [%d]\n", sb_disk->version, sb_disk->magic, sb_disk->block_size, sb_disk->free_blocks);

	printk("%s()\n",__func__);
	sb -> s_magic = MYFS_MAGIC;
	inode = myfs_get_inode(sb, NULL, S_IFDIR, 0);	// create and configure an inode and assign it to inode
	sb -> s_root = d_make_root(inode);		// make inode as root of superblock. s_root is of type dentry.
	inode -> i_op = &myfs_inode_ops;
	inode -> i_fop = &myfs_dir_ops;
							// make the iniitalized inode the superblock root inode
	if(!sb->s_root)					
		return -ENOMEM;
	
	return 0;
}


static struct dentry *myfs_mount(struct file_system_type *fs_type,
				int flags, const char * dev_name,
				void *data)
{
	struct dentry *ret;
	
	printk("%s()\n",__func__);
	ret = mount_bdev(fs_type, flags, dev_name, data, myfs_fill_super);
	
	if(unlikely(IS_ERR(ret)))
		printk(KERN_ERR "Error mounting MyFS\n");
	else
		printk(KERN_INFO "%s() MyFS successfully mounted on [%s]\n", __func__, dev_name);

	return ret;
}

/*
This function deletes reference from all denteries in the tree.
*/

static void myfs_kill_superblock(struct super_block *s)
{
	printk("%s()\n",__func__);
	printk(KERN_INFO "MyFS Super Block Destroyed, Unmount Successful\n");
}

/*

struct file system type is as described here:

http://lxr.free-electrons.com/source/include/linux/fs.h#L1927
*/

struct file_system_type myfs_fs_type = {
	.owner = THIS_MODULE,			// this field tells who is owner of struct file_operations. 
						// This prevents module to get unloaded when it is in operation. 
						// When initialized with THIS_MODULE current module holds the ownership on it

	.name  = "myfs",			// name field is required for mount command to identify which fs to load
						// mount -t myfs device mount_point

	.mount = myfs_mount,
	.kill_sb = myfs_kill_superblock,	
	.fs_flags = 0, 				// various flags like FS_REQUIRES_DEV for non-virtual file systems can be placed here
};

/*
Called while the filesystem is loaded.
*/

static int myfs_init(void)
{
	int ret = 0;
	printk("%s()\n",__func__);
	
	ret = register_filesystem(&myfs_fs_type);
	if(ret == 0)
		printk("%s() MYFS Registered\n",__func__);	
	else
		printk("%s() Error Registering MyFS\n", __func__);

	return ret;
}

/*
Called when module is unloaded
*/

static void myfs_exit(void)
{
	int ret;
	printk("%s()\n",__func__);
	ret = unregister_filesystem(&myfs_fs_type);
	
	if(ret == 0)
		printk("MyFS was unregistered successfully\n");	
	else
		printk("%s() Unsuccessful filesystem unregister\n",__func__);
}	

module_init(myfs_init);
module_exit(myfs_exit);

MODULE_LICENSE("CC0");
MODULE_AUTHOR("Shehbaz Jaffer");
