#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h> // copy from user
#include <linux/fs.h>
#include <linux/delay.h>
#include "ssd1306.h"

static int ssd1306_open(struct inode *inode, struct file *file) 
{
    struct ssd1306_i2c_module *module;

    module = container_of(file->private_data, struct ssd1306_i2c_module, misc_dev);
    file->private_data = module;
    
    if (!module) return -ENODEV;
    
    pr_info("%s, %d\n", __func__, __LINE__);
    
    ssd1306_set_cursor(module, 0, 0);
    ssd1306_print_string(module, "Hello From Func Open");

    msleep(1000);
    return 0;
}

int ssd1306_release(struct inode *node, struct file *filep)
{
    struct ssd1306_i2c_module *module = filep->private_data;

    pr_info("%s, %d\n", __func__, __LINE__);

    ssd1306_clear(module);
    ssd1306_set_cursor(module, 0, 0);
    ssd1306_print_string(module, "Bye from Func Close");
    return 0;
}

ssize_t ssd1306_write_user(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct ssd1306_i2c_module *module = file->private_data;

    char *kbuf = kmalloc(count + 1, GFP_KERNEL);

    if (kbuf == NULL) return -ENOMEM;

    if (copy_from_user(kbuf, buf, count)) {
        kfree(kbuf);
        return -EFAULT;
    }

    kbuf[count] = '\0';

    ssd1306_print_string(module, kbuf);

    kfree(kbuf);

    return count;
}

struct cursor_pos {
    int x;
    int y;
};

#define IOCTL_MAGIC 'S'
#define IOCTL_SSD1306_CLEAR _IO(IOCTL_MAGIC, 0)
#define IOCTL_SSD1306_SET_CURSOR _IOW(IOCTL_MAGIC, 1, struct cursor_pos)
static long ssd1306_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct ssd1306_i2c_module *module = file->private_data;
    struct cursor_pos pos;

    switch(cmd) {
        case IOCTL_SSD1306_CLEAR:
            ssd1306_clear(module);
            break;

        case IOCTL_SSD1306_SET_CURSOR:
            if (copy_from_user(&pos, (void __user *)arg, sizeof(pos)))
                return -EFAULT;
            
            ssd1306_set_cursor(module, pos.x, pos.y);
            break;

        default:
            return -ENOTTY;
    }
    return 0;
}


static const struct file_operations ssd1306_fops = {
    .owner = THIS_MODULE,
    .open = ssd1306_open,
    .write = ssd1306_write_user,
    .release = ssd1306_release,
    .unlocked_ioctl = ssd1306_ioctl,
};


static int ssd1306_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
	struct ssd1306_i2c_module *module;
    int ret = 0;

	module = kmalloc(sizeof(*module), GFP_KERNEL);
	if (!module) {
		pr_err("kmalloc failed\n");
		return -ENOMEM;
	}

	module->client = client;
	module->line_num = 0;
	module->cursor_position = 0;
	module->font_size = SSD1306_DEF_FONT_SIZE;

    // Setup misc device
    module->misc_dev.minor = MISC_DYNAMIC_MINOR;
    module->misc_dev.name = "ssd1306";
    module->misc_dev.fops = &ssd1306_fops;
    module->misc_dev.parent = &client->dev;

    ret = misc_register(&module->misc_dev);
    if (ret) {
        kfree(module);
        dev_err(&client->dev, "Failed to register misc device \n");
    }

    i2c_set_clientdata(client, module);

	ssd1306_display_init(module);
	ssd1306_set_cursor(module, 0, 0);
	pr_info("Tuoi: %s, %d\n", __func__, __LINE__);
    return 0;
}

static int ssd1306_i2c_remove(struct i2c_client *client)
{
	struct ssd1306_i2c_module *module = i2c_get_clientdata(client);
    
	ssd1306_clear(module);
	ssd1306_write(module, true, 0xAE); // Entire Display OFF

    misc_deregister(&module->misc_dev);

	kfree(module);
	pr_info("Tuoi: %s, %d\n", __func__, __LINE__);

    return 0;
}

static const struct of_device_id ssd1306_dt_ids[] = {
	{ .compatible = "ssd1306" },
	{ },
};

MODULE_DEVICE_TABLE(of, ssd1306_dt_ids);

static struct i2c_driver ssd1306_i2c_driver = {
    .probe = ssd1306_i2c_probe,
    .remove = ssd1306_i2c_remove,
    .driver = {
        .name = "ssd1306",
        .owner = THIS_MODULE,
        .of_match_table = ssd1306_dt_ids,
    },
};

// A macro to define default init() and exit() functions.
module_i2c_driver(ssd1306_i2c_driver);

MODULE_AUTHOR("Tuoi");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSD1306 DRIVER KERNEL");