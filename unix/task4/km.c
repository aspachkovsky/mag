#include <linux/init.h>     
#include <linux/kernel.h>   
#include <linux/module.h>   
#include <linux/moduleparam.h>  
      
#include <linux/miscdevice.h>   
#include <linux/string.h>   
#include <linux/slab.h>     
#include <linux/sched.h>    
#include <linux/uaccess.h>  
#include <linux/mutex.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pachkovsky Alexander <pbl.symbolist@gmail.com>");
MODULE_DESCRIPTION("Lexigraphical string sorter");

static unsigned long strings_count = 10;
module_param(strings_count, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(strings_count, "Strings count");

static unsigned long string_size = 256;
module_param(string_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(string_size, "String size");

static char *separator = "\n";
module_param(separator, charp, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(separator, "Separator");

struct storage {
    char **strings;
    unsigned int count;
    struct mutex lock;
};

static struct storage* storage_alloc(void) {
    struct storage *st = NULL;

    st = kzalloc(sizeof(*st), GFP_KERNEL);
    if (unlikely(!st)) {
        goto out;
    }

    st->count = 0;
    st->strings = kzalloc(strings_count * sizeof(char*), GFP_KERNEL);
    if (unlikely(!st->strings)) {
        goto out_free;
    }

    mutex_init(&st->lock);

    out:
        return st;

    out_free:
        kfree(st);
        return NULL;
}

static void storage_free(struct storage *st) {
    int i = 0;
    for (; i < st->count; i++) {
        kfree(st->strings[i]);
    }
    kfree(st);
}

void add_to_storage(struct storage *st, char *str) {
    char* in = str;
    int i = 0;
    for (; i < st->count; i++) {
        if (strcmp(in, st->strings[i]) < 0) {
            char* temp = st->strings[i];
            st->strings[i] = in;
            in = temp;
        }
    }

    if (st->count < strings_count) {
        st->strings[st->count] = in;
    }

    st->count++;
}

size_t read_from_storage(struct storage *st, char** pstr) {
    size_t result;
    int i = 0;
    char* str;

    str = kzalloc(strings_count * (string_size * sizeof(char*) + 1) - 1, GFP_KERNEL);
    if (unlikely(!st)) {
        result = -ENOMEM;
        goto out;
    }
    
    for (; i < strings_count; i++) {
        if (i < st->count) {
            strcat(str, st->strings[i]);
        } 
        strcat(str, separator);            
    }

    result = strlen(str);
    *pstr = str;

    out:
        return result;
}


static int lexsorter_open(struct inode *inode, struct file *file) {
    struct storage *st = storage_alloc();
    int error = 0;
    if (unlikely(!st)) {
        error = -ENOMEM;
        goto out;
    }

    file->private_data = st;
    printk(KERN_INFO "lexsorter device opened\n");

    out:
        return error;
}

static int lexsorter_close(struct inode *inode, struct file *file) {
    struct storage *st = file->private_data;
    storage_free(st);
    printk(KERN_INFO "lexsorter device is closing\n");
    return 0;
}

static ssize_t lexsorter_read(struct file *file, char __user * out, size_t size, loff_t * off) {
    struct storage *st = file->private_data;
    char* str;
    ssize_t ret, result;

    if (mutex_lock_interruptible(&st->lock)) {
        result = -ERESTARTSYS;
        goto out;
    }

    ret = read_from_storage(st, &str);
    if (ret < 0) {
        result = ret;
        goto out;
    }

    size = min(ret, size);
    if (copy_to_user(out, str, size)) {
        result = -EFAULT;
        goto out_unlock;
    }

    result = size;

    printk(KERN_INFO "strings (size = %zu) red from lexsorter device:\n%s\n", size, str);

    out_unlock:
        mutex_unlock(&st->lock);

    out:
        return result;
}

static ssize_t lexsorter_write(struct file *file, const char __user * in, size_t size, loff_t * off) {
    struct storage *st = file->private_data;
    char* str = NULL;
    ssize_t result;

    if (size > string_size) {
        result = -EFBIG;
        goto out;
    }

    if (mutex_lock_interruptible(&st->lock)) {
        result = -ERESTARTSYS;
        goto out;
    }

    str = kzalloc(size, GFP_KERNEL);
    if (unlikely(!str)) {
        result = -ENOMEM;
        goto out_unlock;
    }

    if (copy_from_user(str, in, size)) {
        result = -EFAULT;
        goto out_unlock;
    }

    printk(KERN_INFO "string %s (size is %lu) written to lexsorter device\n", str, size);

    add_to_storage(st, str);

    printk(KERN_INFO "-- %d\n", st->count);

    result = size;

    out_unlock:
        mutex_unlock(&st->lock);

    out:
        return result;
}

static struct file_operations reverse_fops = {
    .owner = THIS_MODULE,
    .open = lexsorter_open,
    .read = lexsorter_read,
    .write = lexsorter_write,
    .release = lexsorter_close,
    .llseek = noop_llseek
};

static struct miscdevice reverse_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "lexsorter",
    .fops = &reverse_fops
};

static int __init lexsorter_init(void) {
    if (!strings_count || !string_size) {
        return -1;
    }

    misc_register(&reverse_misc_device);
    printk(KERN_INFO "lexsorter device has been registered, string count is %lu, string size is %lu bytes\n", strings_count, string_size);
    return 0;
}

static void __exit lexsorter_exit(void) {
    misc_deregister(&reverse_misc_device);
    printk(KERN_INFO "lexsorter device has been unregistered\n");
}

module_init(lexsorter_init);
module_exit(lexsorter_exit);
