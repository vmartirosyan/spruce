#include <linux/module.h>
#include <linux/init.h>

MODULE_AUTHOR("Tsyvarev Andrey");
MODULE_LICENSE("GPL");

int __init bug_module_init(void)
{
	int err = 1;
	
	BUG_ON(err);
	
	return 0;
}

void __exit bug_module_exit(void)
{
}

module_init(bug_module_init);
module_exit(bug_module_exit);
