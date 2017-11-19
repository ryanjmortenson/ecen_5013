#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define NUM_CHAR_DRIVERS (1)
#define USR3_LED (56)
#define TIMER_MS (500)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Mortenson");
MODULE_DESCRIPTION("LED Control module");
MODULE_VERSION("0");

static s32 led_open(struct inode * node, struct file * fp);
static s32 led_release(struct inode * node, struct file * fp);
static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset);
static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset);
static long led_ioctl(struct file * fp, unsigned int i, unsigned long l);
static void led_timer_cb(unsigned long data);

struct file_operations led_fops = {
  .owner        = THIS_MODULE,
  .open         = led_open,
  .read         = led_read,
  .write        = led_write,
  .release      = led_release,
  .compat_ioctl = led_ioctl
};

struct private_data {
  u32 period_ms;
  u32 on_period_ms;
  u32 off_period_ms;
  u8 duty_cycle;
  u8 read_params;
  bool blink_state;
  bool led_state;
  bool read_state;
  bool timer_set;
};

static dev_t led_device;
static struct cdev led_cdev;
static struct timer_list time;
static struct private_data * p_data;

static s32 __init led_module_init(void)
{
  s32 res;
  printk(KERN_INFO "Initializing LED module");

  p_data = NULL;
  res = gpio_is_valid(USR3_LED);
  if (!res)
  {
    printk(KERN_ERR "GPIO number is invalid %d", res);
    return -ENODEV;
  }

  res = gpio_direction_output(USR3_LED, false);
  if (res != 0)
  {
    printk(KERN_ERR "Could not set GPIO output");
    return -ENODEV;
  }

  p_data = kmalloc(sizeof(*p_data), GFP_KERNEL);
  if (p_data == NULL)
  {
    printk(KERN_ERR "Could not allocate private data");
    return -ENOMEM;
  }

  res = alloc_chrdev_region(&led_device, 0, NUM_CHAR_DRIVERS, "led_module");
  if (res < 0)
  {
    printk(KERN_ERR "Could not allocate char device region, %d", res);
    return -EPERM;
  }
  printk(KERN_DEBUG "Received first char device as %d", led_device);

  cdev_init(&led_cdev, &led_fops);
  res = cdev_add(&led_cdev, led_device, 1);
  if (res < 0)
  {
    printk(KERN_ERR "Could not add character device, %d", res);
    return -EPERM;
  }
  setup_timer(&time, led_timer_cb, 0);
  memset(p_data, 0, sizeof(p_data));
  printk(KERN_DEBUG "Initialization successful");
  return 0;
}

static void __exit led_module_exit(void)
{
  printk(KERN_INFO "Destroying LED module");
  printk(KERN_INFO "Killing timer");
  del_timer(&time);
  printk(KERN_INFO "Deleting cdev device");
  cdev_del(&led_cdev);
  printk(KERN_INFO "Unregistering led device");
  unregister_chrdev_region(led_device, NUM_CHAR_DRIVERS);
  printk(KERN_INFO "Freeing p_data");
  kfree(p_data);
}

module_init(led_module_init);
module_exit(led_module_exit);

static s32 led_open(struct inode * node, struct file * fp)
{
  printk(KERN_DEBUG "open()");
  fp->private_data = p_data;
  return 0;
}

static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset)
{
  s32 res = 0;
  s32 count = 0;
  char * buf = NULL;
  struct private_data * pd = (struct private_data *)fp->private_data;

  if (pd->read_state == false)
  {
    buf = kmalloc(256, GFP_KERNEL);
    if (buf == NULL)
    {
      printk(KERN_ERR "Could not malloc memory for kernel to user space copy");
      return -ENOMEM;
    }

    if (pd->read_params == 1)
    {
      count = snprintf(buf, 256, "Period: %d\n", pd->period_ms);
    }
    else if (pd->read_params == 2)
    {
      count = snprintf(buf, 256, "Duty Cycle: %d\n", pd->duty_cycle);
    }
    else if (pd->read_params == 3)
    {
      count = snprintf(buf, 256, "Current Led State: %d\n", pd->led_state);
    }
    else if (pd->read_params == 4)
    {
      count = snprintf(buf, 256, "Current Blink State: %d\n", pd->blink_state);
    }
    else
    {
      count = snprintf(buf, 256, "Period: %d, Duty Cycle: %d, Current Led State: %d, Current Blink State: %d\n",
                     pd->period_ms,
                     pd->duty_cycle,
                     pd->led_state,
                     pd->blink_state);
    }

    if (count <= 0)
    {
      printk(KERN_ERR "Could not format message");
      return -EFAULT;
    }

    res = copy_to_user(buffer, buf, count);
    if (res != 0)
    {
      printk(KERN_ERR "Could not copy data to user space");
      return -EFAULT;
    }
    kfree(buf);
  }
  pd->read_state = true;
  return count;
}

static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset)
{
  char * buf;
  s32 res;
  struct private_data * pd = (struct private_data *)fp->private_data;

  printk(KERN_DEBUG "write()");

  buf = kmalloc(len, GFP_KERNEL);
  if (buf == NULL)
  {
    printk(KERN_ERR "Could not malloc memory for user to kernel space copy");
    return -ENOMEM;
  }

  res = copy_from_user(buf, buffer, len);
  if (res != 0)
  {
    printk(KERN_ERR "Could not copy from user space");
    return -EFAULT;
  }

  if (len != 7)
  {
    printk(KERN_ERR "Not enough bytes were sent");
    return -EINVAL;
  }

  pd->period_ms = *(u32 *)buf;
  pd->duty_cycle = (u8)*(buf + sizeof(u32));
  pd->on_period_ms = pd->period_ms * pd->duty_cycle / 100;
  pd->off_period_ms = pd->period_ms * (100 - pd->duty_cycle) / 100;
  pd->read_params = (u8)*(buf + sizeof(u32) + 1);
  pd->blink_state = (bool)*(buf + sizeof(u32) + 2);
  printk(KERN_DEBUG "period_ms %u", pd->period_ms);
  printk(KERN_DEBUG "on_period %u", pd->on_period_ms);
  printk(KERN_DEBUG "off_period %u", pd->off_period_ms);
  printk(KERN_DEBUG "duty_cycle %u", (u32)pd->duty_cycle);
  kfree(buf);

  if (pd->duty_cycle > 100)
  {
    printk(KERN_ERR "Duty Cycle must be between 0 and 100");
    return -EINVAL;
  }

  if (pd->blink_state)
  {
    printk("Setting up timer for LED blink");
    res = mod_timer(&time, jiffies + msecs_to_jiffies(pd->period_ms));
    if (res < 0)
    {
      printk("Error in modifying timer");
      return -EFAULT;
    }
    pd->led_state = true;
    gpio_set_value(USR3_LED, pd->led_state);
    pd->timer_set = true;
  }
  else
  {
    if (pd->timer_set)
    {
      del_timer(&time);
      pd->timer_set = false;
    }
    printk(KERN_INFO "Not setting up timer, blink_state is off");
  }
  return len;
}

static long led_ioctl(struct file * fp, unsigned int i, unsigned long l)
{
  printk("i: %d", i);
  printk("l: %lu", l);
  return 0;
}

static s32 led_release(struct inode * node, struct file * fp)
{
  struct private_data * pd = (struct private_data *)fp->private_data;
  printk(KERN_DEBUG "release()");
  pd->read_state = false;
  return 0;
}

static void led_timer_cb(unsigned long data)
{
  s32 ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  p_data->led_state = !p_data->led_state;
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(p_data->led_state ? p_data->on_period_ms : p_data->off_period_ms));
  if (!(ret >= 0))
  {
    printk(KERN_INFO "Error in modifying timer");
  }
  gpio_set_value(USR3_LED, p_data->led_state);
}
