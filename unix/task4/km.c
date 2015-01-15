#include <linux/init.h>     
#include <linux/kernel.h>   
#include <linux/module.h>   
#include <linux/moduleparam.h>  
      
#include <linux/miscdevice.h>   
#include <linux/string.h>   
#include <linux/slab.h>       
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

// binary_heap

struct binary_heap {
    unsigned int size;
    unsigned int capacity;
    unsigned int base_capacity;
    int (*comparator) (char*, char*);
    char **elements;
};

int heap_push(struct binary_heap *heap, char *element) {
    unsigned int i, new_capacity;
    char **new_buf, **old_buf;
    int error = 0;

    if (heap->capacity == heap->size) {
        new_capacity = heap->capacity * 2;
        printk(KERN_INFO "Realloc on heap push(), old capacity is %d, new capacity is %d\n", heap->capacity, new_capacity);

        new_buf = kmalloc(new_capacity * sizeof(char*), GFP_KERNEL);
        if (unlikely(!new_buf)) {
            error = -ENOMEM;
            goto out;
        }

        memcpy(new_buf, heap->elements, heap->size * sizeof(char*));
        heap->capacity = new_capacity;
        old_buf = heap->elements;
        heap->elements = new_buf;      
        kfree(old_buf);      
    }

    if (heap->size == 0) {
        heap->size++;
        heap->elements[0] = element;
    } else {    
        for(i = ++heap->size - 1; i > 0 && (*(heap->comparator))(heap->elements[i / 2], element); i /= 2) {
            heap->elements[i] = heap->elements[i / 2];
        }
        heap->elements[i] = element;
    }

    out:
        return error;
}

int heap_pop(struct binary_heap *heap, char **top) {
    char **old_buf, **new_buf, *last;
    unsigned int i, swap, other, new_capacity;
    int error = 0;

    if ((heap->size <= (heap->capacity / 4)) && (heap->size > heap->base_capacity)) {
        new_capacity = heap->capacity / 2;
        printk(KERN_INFO "Realloc on heap pop(), old capacity is %d, new capacity is %d\n", heap->capacity, new_capacity);
        heap->capacity = new_capacity;
        
        new_buf = kmalloc(heap->capacity * sizeof(char*), GFP_KERNEL);
        if (unlikely(!new_buf)) {
            error = -ENOMEM;
            goto out;
        }
        
        memcpy(new_buf, heap->elements, heap->size * sizeof(char*));
        old_buf = heap->elements;
        heap->elements = new_buf;
        kfree(old_buf);
    }

    if (heap->size == 0) {
        top = NULL;
        goto out;
    }

    *top = heap->elements[0];
    heap->size--;
    last = heap->elements[heap->size];

    for(i = 0; 1; i = swap) {
        swap = i * 2 + 1;
        if (swap >= heap->size) {
            break;
        }

        other = swap + 1;
        if ((other < heap->size) && ((*(heap->comparator))(heap->elements[swap], heap->elements[other]))) {
            swap = other;
        }

        if (!((*(heap->comparator))(last, heap->elements[swap]))) { 
            break;
        } 

        heap->elements[i] = heap->elements[swap];
    }

    heap->elements[i] = last;

    out:
        return error;
}

int lex_comparator(char* first, char* second) {
    return strcmp(first, second) > 0;
}


// Storage

struct storage {
    struct binary_heap heap;     
    struct mutex lock;
};

static struct storage* storage_alloc(void) {
    struct storage *st;
    char **data;

    st = kmalloc(sizeof(*st), GFP_KERNEL);
    if (unlikely(!st)) {
        goto out_fail;
    }

    data = kmalloc(strings_count * sizeof(char*), GFP_KERNEL);
    if (unlikely(!data)) {
        goto out_free;
    }

    st->heap = (struct binary_heap) {
        .size = 0,
        .capacity = strings_count,
        .base_capacity = strings_count,
        .comparator = &lex_comparator,
        .elements = data
    };

    mutex_init(&st->lock);

    return st;

    out_free:
        kfree(st);

    out_fail:
        return NULL;
}

static void storage_free(struct storage *st) {
    kfree(st->heap.elements);
    kfree(st);
}

int read_from_storage(struct storage *st, char **pstr) {
    int error = 0, i;
    char *result, *str;

    result = kzalloc(strings_count * (string_size * sizeof(char) + 1) - 1, GFP_KERNEL);
    if (unlikely(!result)) {
        error = -ENOMEM;
        goto out;
    }
    
    for (i = 0; i < strings_count; i++) {
        if (st->heap.size > 0) {
            error = heap_pop(&(st->heap), &str);
            if (error < 0) {
                goto out_free;
            }

            if (str != NULL) {
                strcat(result, str);
            }
        }
        
        if (i < strings_count - 1) {
            strcat(result, separator);            
        }
    }

    *pstr = result;

    out:
        return error;

    out_free:
        kfree(result);
        return error;
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

static ssize_t lexsorter_read(struct file *file, char __user *out, size_t size, loff_t *off) {
    struct storage *st = file->private_data;
    char *str;
    ssize_t result;

    if (mutex_lock_interruptible(&st->lock)) {
        result = -ERESTARTSYS;
        goto out;
    }

    result = read_from_storage(st, &str);
    if (result < 0) {
        goto out;
    }

    size = min(strlen(str), size);
    if (copy_to_user(out, str, size)) {
        result = -EFAULT;
        goto out_unlock;
    }

    printk(KERN_INFO "strings (size = %zu) red from lexsorter device:\n%s\n", size, str);
    result = size;

    out_unlock:
        mutex_unlock(&st->lock);

    out:
        return result;
}

static ssize_t lexsorter_write(struct file *file, const char __user *in, size_t size, loff_t *off) {
    struct storage *st = file->private_data;
    char *str;
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
    heap_push(&(st->heap), str);
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
    if (!strings_count || !string_size || !separator) {
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