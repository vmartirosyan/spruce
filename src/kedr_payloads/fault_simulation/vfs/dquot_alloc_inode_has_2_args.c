#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/quotaops.h>

MODULE_LICENSE("GPL");

static int __init
my_init(void)
{
    // Call target function
    int res = dquot_alloc_inode(NULL, 1);
    return res;
}

static void __exit
my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);