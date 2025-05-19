#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

#define SLOT_COUNT 3

static int current_users = 0;
static spinlock_t current_users_lock;


// Decidi usar un spinlock en vez de un semaforo ya que las operaciones que se van a hacer 
// cuando esta puesto el lock son muy simples (incrementar y decrementar un contador)

typedef struct user_data {  
    bool valid;
    char input;
    spinlock_t lock;
} user_data;

static int letras_open(struct inode *inod, struct file *filp) {
    user_data *udata;

    spin_lock(&current_users_lock);
    if (current_users >= SLOT_COUNT){
        spin_unlock(&current_users_lock);
        return -EPERM;
    } else {
        current_users++;
        spin_unlock(&current_users_lock);
    }

    // inicializo la estructura, invalida hasta que se escriba
    filp->private_data = kmalloc(sizeof(user_data), GFP_KERNEL);
    udata = (user_data *) filp->private_data;
    udata->valid = false; 
    spin_lock_init(&(udata->lock));

    return 0;
}

static int letras_release(struct inode *inod, struct file *filp) {
    spin_lock(&current_users_lock);
    current_users--;
    spin_unlock(&current_users_lock);
    
    kfree(filp->private_data);
    return 0;
}

static ssize_t letras_read(struct file *filp, char __user *data, size_t size, loff_t *offset) {
    user_data *udata = (user_data *) filp->private_data;
    char *char_copies;
    int i;
 
    // bloqueo para que no se pueda leer hasta que no se termine de escribir. 
    spin_lock(&(udata->lock));
    if (!udata->valid) {
        spin_unlock(&(udata->lock));
        return -EPERM;
    }
    char_copies = kmalloc(size, GFP_KERNEL);
    for (i = 0; i < size; i++) {
        char_copies[i] = udata->input;
    }

    spin_unlock(&(udata->lock));

    copy_to_user(data, char_copies, size);
    kfree(char_copies);
    return size;
}

static ssize_t letras_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {
    user_data *udata = (user_data *) filp->private_data;

    // bloqueo para que no se pueda escribir hasta que no se termine de leer. 
    spin_lock(&(udata->lock));
    // solo se puede escribir el primer caracter. Si ya se habia escrito, se ignora.
    if (!(udata->valid)) {

        // guardar el primer caracter escrito
        copy_from_user(&(udata->input), data, 1);
        udata->valid = true;
        
    }
    
    spin_unlock(&(udata->lock));
    return size;
}

static struct file_operations letras_fops = {
  .owner = THIS_MODULE,
  .open = letras_open,
  .read = letras_read,
  .write = letras_write,
  .release = letras_release,
};

static dev_t letras_devno;
static struct cdev letras_cdev;
static struct class *letras_class;

static int __init letras_init(void) {
    spin_lock_init(&current_users_lock);

    cdev_init(&letras_cdev, &letras_fops);
    alloc_chrdev_region(&letras_devno, 0, 1, "SO-letras123");
    cdev_add(&letras_cdev, letras_devno, 1);

    letras_class = class_create(THIS_MODULE, "letras123");
    device_create(letras_class, NULL, letras_devno, NULL, "letras123");

    printk(KERN_ALERT "Loading module 'letras123'\n");
    return 0;
}

static void __exit letras_exit(void) {
    printk(KERN_ALERT "Unloading module 'letras123'\n");

    device_destroy(letras_class, letras_devno);
    class_destroy(letras_class);

    unregister_chrdev_region(letras_devno, 1);
    cdev_del(&letras_cdev);
}

module_init(letras_init);
module_exit(letras_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al letras");
