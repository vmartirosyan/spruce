#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int __init
my_init(void)
{
    // Call target function
    struct inode* i = NULL;
    struct dentry* d = d_make_root(i);
    dput(d);

    return 0;
}

static void __exit
my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);