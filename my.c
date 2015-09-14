#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>


static int myfs_init(void)
{
	int ret = 0;
	return ret;
}

static void myfs_exit(void)
{
	
}	


module_init(myfs_init);
module_exit(myfs_exit);

MODULE_LICENSE("CC0");
MODULE_AUTHOR("Shehbaz Jaffer");
