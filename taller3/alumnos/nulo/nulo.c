#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>

static ssize_t nulo_read(struct file *filp, char __user *data, size_t s, loff_t *off) {
  return 0;
}

static ssize_t nulo_write(struct file *filp, const char __user *data, size_t s, loff_t *off) {
  return s;  
}

static struct file_operations nulo_operaciones = {
  .owner = THIS_MODULE,
  .read = nulo_read,
  .write = nulo_write,
};

static struct cdev nulo_device;
static dev_t major;
static struct class *nulo_class;

static int __init hello_init(void) {
  // registrar dispositivo
  major = register_chrdev(0, "nulo", &nulo_operaciones);
  nulo_class = class_create(THIS_MODULE, "nulo");

  cdev_init(&nulo_device, &nulo_operaciones);
  cdev_add(&nulo_device, major, 1);

  device_create(nulo_class, NULL, MKDEV(major, 0), NULL, "nulo");
  
  return 0;
}

static void __exit hello_exit(void) {
  device_destroy(nulo_class, MKDEV(major, 0));
  cdev_del(&nulo_device);
  unregister_chrdev(major, "nulo");
  class_destroy(nulo_class);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de '/dev/null'");
