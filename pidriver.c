#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h> /* Needed for pr_info() */ 
#include <linux/module.h> /* Needed by all modules */ 
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/slab.h>

#define MAX_LENGTH 100000000000
#define DEV_PI_NAME "pi"


static dev_t pi_dev = 0;
static long long pi_p = 0;
static struct cdev *pi_cdev;
static struct class *pi_class;
static DEFINE_MUTEX(pi_mutex);



static long long compute_pi_leibniz(long long N);
static long long compute_pi_leibniz_p(long long terms, int numThreads);
static long long compute_pi_leibniz(long long N)
{
    int64_t pi = 0;
    for (int i = 0; i < N; i++) {
        int64_t tmp = (i & 1) ? (-1) : 1;
        pi += tmp * ((((long long) 1)<<50)/(2 * i + 1));
    }
    return pi << 2;
}


struct thread_data{
    struct completion *comp;
    long start, end;
    long long *pi_output;
};

struct completion comps[4];

static int kthread_compute_pi(void *data)
{
    int64_t pi = 0;
    struct thread_data *d = (struct thread_data *)(data);
    long i = d->start;
    long long end = d->end;
    for (; i < end; i++) {
        int64_t tmp = (i & 1) ? (-1) : 1;
        pi += tmp * ((((long long) 1)<<50)/(2 * i + 1));
    }
    mutex_lock(&pi_mutex);
    *(d->pi_output) += (pi<<2);
    mutex_unlock(&pi_mutex);
    complete(d->comp);
    do_exit(0);
}
static long long compute_pi_leibniz_p(long long terms, int numThreads)
{
    // struct task_struct **threads = kmalloc(sizeof(struct task_struct *) * numThreads, GFP_KERNEL);
    // struct thread_data *datas = kmalloc(sizeof(struct thread_data)* numThreads, GFP_KERNEL);
    struct task_struct *threads[4];
    struct thread_data datas[4];
    long long pi_output = 0;
    for (int i = 0; i < numThreads; i++){
        init_completion(comps+i);
        datas[i].comp = comps+i;
        datas[i].pi_output = &pi_output;
    }
      long long step = terms/numThreads ;
      datas[0].start = 0;
      datas[0].end = step;
      if (numThreads >= 2){
        for (int i = 1; i < numThreads-1; i++){
            datas[i].start = datas[i-1].end;
            datas[i].end = datas[i].start + step;
        }
        datas[numThreads-1].start = datas[numThreads-2].end;
        datas[numThreads-1].end = terms;       
      }
      for (int i = 0; i < numThreads; i++) {
        threads[i] = kthread_create(&kthread_compute_pi, (void*)(datas+i), "task");
        kthread_bind(threads[i], i);
        wake_up_process(threads[i]);
      }
    for (int i = 0; i < numThreads; i++){
        wait_for_completion(comps+i);
    }
    return pi_output;
}



static int pi_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int pi_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t pi_read(struct file *file, char *buf, size_t size, loff_t *offset)
{
    return (ssize_t) compute_pi_leibniz(*offset);
}

static ssize_t pi_write(struct file *file, const char *bur, size_t size, loff_t *offset)
{
    ktime_t kt;
    long long rt;
    int loop = 1;
    switch(size){
    case 0:  // defalut (time measure)
        kt = ktime_get();
        rt = compute_pi_leibniz(*offset);
        // for (int i = 0; i < loop; i++){
        //     printk(KERN_INFO
        //        "(not parallel) pi = %lld",
        //     rt);
        // }
        kt = ktime_sub(ktime_get(), kt);
        return (ssize_t) ktime_to_ns(kt);
        // return kt;
    case 1:  // defalut (time measure)
        kt = ktime_get();
        rt = compute_pi_leibniz_p(*offset, 1);
        // for (int i = 0; i < loop; i++){
        //     printk(KERN_INFO
        //        "(not parallel) pi = %lld",
        //     rt);
        // }
        kt = ktime_sub(ktime_get(), kt);
        return (ssize_t) ktime_to_ns(kt);
        // return kt;
    case 4:  // defalut (time measure)
        kt = ktime_get();
        rt = compute_pi_leibniz_p(*offset, 4);
        // for (int i = 0; i < loop; i++){
        //     printk(KERN_INFO
        //        "(parallel) pi = %lld",
        //     rt);
        // }
        kt = ktime_sub(ktime_get(), kt);
        return (ssize_t) ktime_to_ns(kt);
        // return kt;
    default:  // make check
        return 1;
    }
}

static loff_t pi_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations pi_fops = {
    .owner = THIS_MODULE,
    .read = pi_read,
    .write = pi_write,
    .open = pi_open,
    .release = pi_release,
    .llseek = pi_device_lseek,
};

static int __init init_pi_dev(void) 
{ 
    int rc = 0;

    mutex_init(&pi_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&pi_dev, 0, 1, DEV_PI_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the pi char device. rc = %i",
               rc);
        return rc;
    }

    pi_cdev = cdev_alloc();
    if (pi_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    pi_cdev->ops = &pi_fops;
    rc = cdev_add(pi_cdev, pi_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    pi_class = class_create(THIS_MODULE, DEV_PI_NAME);

    if (!pi_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(pi_class, NULL, pi_dev, NULL, DEV_PI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(pi_class);
failed_class_create:
    cdev_del(pi_cdev);
failed_cdev:
    unregister_chrdev_region(pi_dev, 1);
    return rc;
} 

static void __exit exit_pi_dev(void) 
{ 
    mutex_destroy(&pi_mutex);
    device_destroy(pi_class, pi_dev);
    class_destroy(pi_class);
    cdev_del(pi_cdev);
    unregister_chrdev_region(pi_dev, 1);
} 
 
module_init(init_pi_dev); 
module_exit(exit_pi_dev); 
 
MODULE_LICENSE("GPL");