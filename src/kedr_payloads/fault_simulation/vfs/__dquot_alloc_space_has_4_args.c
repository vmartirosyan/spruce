#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/quotaops.h>

MODULE_LICENSE("GPL");

static int __init
my_init(void)
{
    // Call target function
    int res = __dquot_alloc_space(NULL, 1, 0, 0);
    return res;
}

static void __exit
my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);