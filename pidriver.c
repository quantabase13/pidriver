/* 
 * hello-1.c - The simplest kernel module. 
 */ 
#include <linux/kernel.h> /* Needed for pr_info() */ 
#include <linux/module.h> /* Needed by all modules */ 

static int32_t compute_pi_leibniz(size_t N);
static int __init hello_2_init(void) 
{ 
    pr_info("Hello world %d %d.\n", 100, compute_pi_leibniz(100)); 
 
    /* A non 0 return means init_module failed; module can't be loaded. */ 
    return 0; 
} 

int32_t compute_pi_leibniz(size_t N)
{
    int32_t pi = 0;
    for (size_t i = 0; i < N; i++) {
        int32_t tmp = (i & 1) ? (-1) : 1;
        pi += tmp * ((1<<16)/(2 * i + 1));
    }
    return pi << 2;
}

static void __exit hello_2_exit(void) 
{ 
    pr_info("Goodbye world 1.\n"); 
} 
 
module_init(hello_2_init); 
module_exit(hello_2_exit); 
 
MODULE_LICENSE("GPL");