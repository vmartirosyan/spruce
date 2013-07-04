#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int __init
my_init(void)
{
    // Call target function
    struct block_device *bdev = blkdev_get_by_dev(MKDEV(0,1), 0, NULL);
    bdput(bdev);
    return 0;
}

static void __exit
my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);