#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>

MODULE_LICENSE("GPL");

static int f(void* data)
{
    (void)data;
    return 0;
}

static int __init
my_init(void)
{
    // Call target function
    struct task_struct* res = kthread_create_on_node(f, NULL, -1, "my_name");
    wake_up_process(res);

    return 0;
}

static void __exit
my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);