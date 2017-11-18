#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>

#define NUM_CHAR_DRIVERS (1)
#define USR3_LED (56)
#define TIMER_MS (500)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Mortenson");
MODULE_DESCRIPTION("LED Control module");
MODULE_VERSION("0");

static int led_open(struct inode * node, struct file * fp);
static int led_release(struct inode * node, struct file * fp);
static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset);
static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset);
static void led_timer_cb(unsigned long data);

struct private_data {
  unsigned int period;
  unsigned int duty_cycle;
  unsigned int state;
  unsigned int read_state;
};

struct file_operations led_fops = {
  .owner    = THIS_MODULE,
  .open     = led_open,
  .read     = led_read,
  .write    = led_write,
  .release  = led_release
};

static dev_t led_device;
static struct cdev led_cdev;
static struct timer_list time;
s32 timer_set = 0;

static int __init led_module_init(void)
{
  int res;
  printk(KERN_INFO "Initializing LED module");
  printk(KERN_DEBUG "Attempting to get char region");
  do
  {
    res = gpio_is_valid(USR3_LED);
    if (!res)
    {
      printk(KERN_ERR "GPIO number is invalid %d", res);
      break;
    }

    res = gpio_request(USR3_LED, "sysfs");
    if (res != 0)
    {
      printk(KERN_ERR "Could not request GPIO");
      break;
    }

    res = gpio_direction_output(USR3_LED, false);
    if (res != 0)
    {
      printk(KERN_ERR "Could not set GPIO output");
      break;
    }

    res = gpio_export(USR3_LED, false);
    if (res != 0)
    {
      printk(KERN_ERR "Could not export GPIO");
      break;
    }

    res = alloc_chrdev_region(&led_device, 0, NUM_CHAR_DRIVERS, "led_module");
    if (res < 0)
    {
      printk(KERN_ERR "Could not allocate char device region, %d", res);
      break;
    }
    printk(KERN_DEBUG "Received first char device as %d", led_device);

    cdev_init(&led_cdev, &led_fops);
    res = cdev_add(&led_cdev, led_device, 1);
    if (res < 0)
    {
      printk(KERN_ERR "Could not add character device, %d", res);
      break;
    }
  } while(0);
  return 0;
}

static void __exit led_module_exit(void)
{
  printk(KERN_INFO "Destroying LED module");
  if (timer_set)
  {
    del_timer(&time);
  }
  gpio_unexport(USR3_LED);
  gpio_free(USR3_LED);
  cdev_del(&led_cdev);
  unregister_chrdev_region(led_device, NUM_CHAR_DRIVERS);
}

module_init(led_module_init);
module_exit(led_module_exit);

static int led_open(struct inode * node, struct file * fp)
{
  char * buf;
  struct private_data * p_data;

  printk(KERN_DEBUG "open()");

  buf = kmalloc(sizeof(struct private_data), GFP_KERNEL);
  if (buf == NULL)
  {
    printk(KERN_ERR "Could not malloc memory for private data");
    return -1;
  }
  p_data = (struct private_data *) buf;
  p_data->period = 0;
  p_data->duty_cycle = 0;
  p_data->state = 0;
  p_data->read_state = 0;
  fp->private_data = p_data;
  return 0;
}

static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset)
{
  int res = 0;
  printk(KERN_DEBUG "read()");
  if (!((struct private_data *)fp->private_data)->read_state)
  {
    res = copy_to_user(buffer, "HELLO\n", 7);
    printk(KERN_INFO "Copy to user res %d", res);
    printk(KERN_INFO "length %d", len);
    ((struct private_data *)fp->private_data)->read_state = 1;
    return 6;
  }
  else
  {
    return 0;
  }
}

static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset)
{
  char * buf;
  s32 res;

  printk(KERN_DEBUG "write()");
  printk(KERN_DEBUG "len %d", len);
  buf = kmalloc(len, GFP_KERNEL);
  if (buf == NULL)
  {
    printk(KERN_ERR "Could not malloc memory for user to kernel space copy");
    return 0;
  }

  res = copy_from_user(buf, buffer, len);
  if (res != 0)
  {
    printk(KERN_ERR "Could not copy from user space");
    return 0;
  }

  printk("Setting up timer for LED blink");
  setup_timer(&time, led_timer_cb, 0);
  res = mod_timer(&time, jiffies + msecs_to_jiffies(TIMER_MS));
  if (res != 0)
  {
    printk("Error in modifying timer");
  }
  timer_set = 1;

  return len;
}

static int led_release(struct inode * node, struct file * fp)
{
  printk(KERN_DEBUG "release()");
  kfree(fp->private_data);
  return 0;
}

static void led_timer_cb(unsigned long data)
{
  static int i;
  int ret = 0;
  static bool led_on;
  led_on = !led_on;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  i++;
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(TIMER_MS));
  gpio_set_value(USR3_LED, led_on);

  if (!(ret >= 0))
  {
    printk(KERN_INFO "Error in modifying timer");
  }

  printk(KERN_INFO "Called %d times", i);
}
