#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "user_interactable"
#define CLASS_NAME  "user_interactable_class"
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ABHAI-TIWARI");
MODULE_DESCRIPTION("User-interactable character driver with read/write support");
MODULE_VERSION("1.0");

/*
 * Device state shared by all file operations.
 * - buffer holds the latest message written from user space.
 * - data_len tracks valid bytes in buffer.
 * - lock protects buffer/data_len from concurrent access.
 */
struct user_interactable_device {
	dev_t dev_num;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	char *buffer;
	size_t data_len;
	struct mutex lock;
};

static struct user_interactable_device ui_dev;

/*
 * open() is called when user space opens /dev/user_interactable.
 * We only log access here; no private allocation is required.
 */
static int user_interactable_open(struct inode *inode, struct file *file)
{
	pr_info("user_interactable: device opened\n");
	return 0;
}

/*
 * release() is called on close().
 * This keeps lifecycle visible in kernel logs.
 */
static int user_interactable_release(struct inode *inode, struct file *file)
{
	pr_info("user_interactable: device closed\n");
	return 0;
}

/*
 * read() copies kernel buffer data to user buffer.
 * - *offset gives file position so repeated reads eventually return EOF.
 * - return value is number of bytes copied to user space.
 */
static ssize_t user_interactable_read(struct file *file,
				      char __user *user_buf,
				      size_t count,
				      loff_t *offset)
{
	size_t bytes_to_copy;

	if (*offset < 0)
		return -EINVAL;

	mutex_lock(&ui_dev.lock);

	if (*offset >= ui_dev.data_len) {
		mutex_unlock(&ui_dev.lock);
		return 0;
	}

	bytes_to_copy = min(count, ui_dev.data_len - (size_t)*offset);

	if (copy_to_user(user_buf, ui_dev.buffer + *offset, bytes_to_copy)) {
		mutex_unlock(&ui_dev.lock);
		return -EFAULT;
	}

	*offset += bytes_to_copy;
	mutex_unlock(&ui_dev.lock);

	pr_info("user_interactable: read %zu bytes\n", bytes_to_copy);
	return bytes_to_copy;
}

/*
 * write() stores user data into kernel buffer.
 * For simplicity, each write replaces previous content.
 */
static ssize_t user_interactable_write(struct file *file,
				       const char __user *user_buf,
				       size_t count,
				       loff_t *offset)
{
	size_t bytes_to_copy;

	if (count == 0)
		return 0;

	bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - 1));

	mutex_lock(&ui_dev.lock);

	if (copy_from_user(ui_dev.buffer, user_buf, bytes_to_copy)) {
		mutex_unlock(&ui_dev.lock);
		return -EFAULT;
	}

	ui_dev.buffer[bytes_to_copy] = '\0';
	ui_dev.data_len = bytes_to_copy;
	*offset = 0;
	mutex_unlock(&ui_dev.lock);

	pr_info("user_interactable: wrote %zu bytes\n", bytes_to_copy);
	return bytes_to_copy;
}

static const struct file_operations user_interactable_fops = {
	.owner = THIS_MODULE,
	.open = user_interactable_open,
	.release = user_interactable_release,
	.read = user_interactable_read,
	.write = user_interactable_write,
};

/*
 * Module initialization:
 * 1) allocate char-device numbers,
 * 2) initialize and add cdev,
 * 3) create class and /dev node,
 * 4) initialize buffer with a default greeting.
 */
static int __init user_interactable_init(void)
{
	int ret;
	const char *default_message = "Hello from user_interactable_driver\n";

	ret = alloc_chrdev_region(&ui_dev.dev_num, 0, 1, DEVICE_NAME);
	if (ret)
		return ret;

	cdev_init(&ui_dev.cdev, &user_interactable_fops);
	ui_dev.cdev.owner = THIS_MODULE;

	ret = cdev_add(&ui_dev.cdev, ui_dev.dev_num, 1);
	if (ret)
		goto err_unregister;

	ui_dev.class = class_create(CLASS_NAME);
	if (IS_ERR(ui_dev.class)) {
		ret = PTR_ERR(ui_dev.class);
		goto err_cdev_del;
	}

	ui_dev.device = device_create(ui_dev.class, NULL, ui_dev.dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(ui_dev.device)) {
		ret = PTR_ERR(ui_dev.device);
		goto err_class_destroy;
	}

	ui_dev.buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (!ui_dev.buffer) {
		ret = -ENOMEM;
		goto err_device_destroy;
	}

	mutex_init(&ui_dev.lock);
	strscpy(ui_dev.buffer, default_message, BUFFER_SIZE);
	ui_dev.data_len = strlen(ui_dev.buffer);

	pr_info("user_interactable: loaded (major=%d minor=%d)\n",
		MAJOR(ui_dev.dev_num), MINOR(ui_dev.dev_num));
	return 0;

err_device_destroy:
	device_destroy(ui_dev.class, ui_dev.dev_num);
err_class_destroy:
	class_destroy(ui_dev.class);
err_cdev_del:
	cdev_del(&ui_dev.cdev);
err_unregister:
	unregister_chrdev_region(ui_dev.dev_num, 1);
	return ret;
}

/*
 * Module cleanup reverses init in exact opposite order.
 */
static void __exit user_interactable_exit(void)
{
	kfree(ui_dev.buffer);
	device_destroy(ui_dev.class, ui_dev.dev_num);
	class_destroy(ui_dev.class);
	cdev_del(&ui_dev.cdev);
	unregister_chrdev_region(ui_dev.dev_num, 1);

	pr_info("user_interactable: unloaded\n");
}

module_init(user_interactable_init);
module_exit(user_interactable_exit);
