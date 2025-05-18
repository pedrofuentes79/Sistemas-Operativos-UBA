#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

static int last_number = 0;

static ssize_t azar_read(struct file *filp, char __user *data, size_t size, loff_t *offset) {
    unsigned int random_bytes;
    int random_num;
    int max_str_size;
    char *str;
    ssize_t len;

    if (last_number == 0) {
        return -EPERM;
    }

    // generamos un numero aleatorio
    get_random_bytes(&random_bytes, sizeof(random_bytes));
    random_num = random_bytes % last_number;

    max_str_size = 12; // 10 digitos + newline + null terminator
    str = kmalloc(max_str_size, GFP_KERNEL);

    // convertimos el numero a cadena de caracteres, incluyendo newline
    len = snprintf(str, max_str_size, "%d\n", random_num);

    // copiamos la cadena de caracteres al buffer del usuario
    copy_to_user(data, str, len);

    kfree(str);
    return len;
}

static ssize_t azar_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {
    char *buf;
    int parsed_num;

    if (size == 0) {
        return -EPERM;
    }

    buf = kmalloc(size + 1, GFP_KERNEL);

    copy_from_user(buf, data, size);
    buf[size] = '\0';

    if (kstrtoint(buf, 10, &parsed_num) != 0) {
        // valor invalido. Resetear last_number.
        last_number = 0;
        kfree(buf);
        return -EPERM;
    }

    // Chequeamos que el numero sea positivo
    if (parsed_num < 0) {
        last_number = 0;
        return -EINVAL;
    } else {
        last_number = parsed_num;
    }

    return size;
}

static struct file_operations azar_fops = {
  .owner = THIS_MODULE,
  .read = azar_read,
  .write = azar_write,
};

static dev_t azar_devno;
static struct cdev azar_cdev;
static struct class *azar_class;

static int __init azar_init(void) {
    cdev_init(&azar_cdev, &azar_fops);
    alloc_chrdev_region(&azar_devno, 0, 1, "SO-azar");
    cdev_add(&azar_cdev, azar_devno, 1);

    azar_class = class_create(THIS_MODULE, "azar");
    device_create(azar_class, NULL, azar_devno, NULL, "azar");

    printk(KERN_ALERT "Loading module 'azar'\n");
    return 0;
}

static void __exit azar_exit(void) {
    printk(KERN_ALERT "Unloading module 'azar'\n");

    device_destroy(azar_class, azar_devno);
    class_destroy(azar_class);

    unregister_chrdev_region(azar_devno, 1);
    cdev_del(&azar_cdev);
}

module_init(azar_init);
module_exit(azar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al azar");
