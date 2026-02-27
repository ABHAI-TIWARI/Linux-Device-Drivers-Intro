#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char"
#define CLASS_NAME "simple_char_class"
#define BUFFER_SIZE 256

static dev_t dev_number;
static struct cdev simple_cdev;
static struct class *simple_class;
static struct device *simple_device;

static char message[BUFFER_SIZE] = "Hello from kernel space!\n";
static size_t message_len = 25;

static int simple_char_open(struct inode *inode, struct file *file)
{
	pr_info("simple_char: device opened\n");
	return 0;
}

static int simple_char_release(struct inode *inode, struct file *file)
{
	pr_info("simple_char: device closed\n");
	return 0;
}

static ssize_t simple_char_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset)
{
	size_t bytes_to_read;

	if (*offset >= message_len)
		return 0;

	bytes_to_read = min(count, message_len - (size_t)*offset);
	if (copy_to_user(user_buffer, message + *offset, bytes_to_read))
		return -EFAULT;

	*offset += bytes_to_read;
	return bytes_to_read;
}

static ssize_t simple_char_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset)
{
	size_t bytes_to_write;

	bytes_to_write = min(count, (size_t)(BUFFER_SIZE - 1));
	if (copy_from_user(message, user_buffer, bytes_to_write))
		return -EFAULT;

	message[bytes_to_write] = '\0';
	message_len = bytes_to_write;

	pr_info("simple_char: received %zu bytes\n", bytes_to_write);
	return bytes_to_write;
}

static const struct file_operations simple_fops = {
	.owner = THIS_MODULE,
	.open = simple_char_open,
	.release = simple_char_release,
	.read = simple_char_read,
	.write = simple_char_write,
};

static int __init simple_driver_init(void)
{
	int result;

	result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
	if (result < 0)
		return result;

	cdev_init(&simple_cdev, &simple_fops);
	simple_cdev.owner = THIS_MODULE;

	result = cdev_add(&simple_cdev, dev_number, 1);
	if (result < 0)
		goto err_unregister;

	simple_class = class_create(CLASS_NAME);
	if (IS_ERR(simple_class)) {
		result = PTR_ERR(simple_class);
		goto err_cdev_del;
	}

	simple_device = device_create(simple_class, NULL, dev_number, NULL, DEVICE_NAME);
	if (IS_ERR(simple_device)) {
		result = PTR_ERR(simple_device);
		goto err_class_destroy;
	}

	pr_info("simple_char: registered at major=%d minor=%d\n", MAJOR(dev_number), MINOR(dev_number));
	return 0;

err_class_destroy:
	class_destroy(simple_class);
err_cdev_del:
	cdev_del(&simple_cdev);
err_unregister:
	unregister_chrdev_region(dev_number, 1);
	return result;
}

static void __exit simple_driver_exit(void)
{
	device_destroy(simple_class, dev_number);
	class_destroy(simple_class);
	cdev_del(&simple_cdev);
	unregister_chrdev_region(dev_number, 1);
	pr_info("simple_char: unloaded\n");
}

modul e_init(simple_driver_init);
module_exit(simple_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ABHAI-TIWARI");
MODULE_DESCRIPTION("A simple Linux character device driver example");
MODULE_VERSION("1.0");
