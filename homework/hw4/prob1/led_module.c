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
static void led_timer_cb(unsigned long data);

struct private_data {
  u32 period_ms;
  u32 on_period_ms;
  u32 off_period_ms;
  u8 duty_cycle;
  bool blink_state;
  bool led_state;
  bool read_state;
  bool timer_set;
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
static struct private_data p_data;

static s32 __init led_module_init(void)
{
  s32 res;
  printk(KERN_INFO "Initializing LED module");
  printk(KERN_DEBUG "Attempting to get char region");

  res = gpio_is_valid(USR3_LED);
  if (!res)
  {
    printk(KERN_ERR "GPIO number is invalid %d", res);
    return -EPERM;
  }

  res = gpio_direction_output(USR3_LED, false);
  if (res != 0)
  {
    printk(KERN_ERR "Could not set GPIO output");
    return -EPERM;
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
  memset(&p_data, 0, sizeof(p_data));
  return 0;
}

static void __exit led_module_exit(void)
{
  printk(KERN_INFO "Destroying LED module");
  if (p_data.timer_set)
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

static s32 led_open(struct inode * node, struct file * fp)
{
  printk(KERN_DEBUG "open()");
  return 0;
}

static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset)
{
  s32 res = 0;
  s32 count = 0;
  char * buf = NULL;

  if (p_data.read_state == false)
  {
    buf = kmalloc(256, GFP_KERNEL);
    if (buf == NULL)
    {
      printk(KERN_ERR "Could not malloc memory for kernel to user space copy");
      return -ENOMEM;
    }

    printk(KERN_INFO "period_ms %d", p_data.period_ms);
    count = snprintf(buf, 256, "Period: %d, Duty Cycle: %d, Current Led State: %d, Current Blink State: %d\n",
                   p_data.period_ms,
                   p_data.duty_cycle,
                   p_data.led_state,
                   p_data.blink_state);

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
  p_data.read_state = true;
  return count;
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
    return -ENOMEM;
  }

  res = copy_from_user(buf, buffer, len);
  if (res != 0)
  {
    printk(KERN_ERR "Could not copy from user space");
    return -EFAULT;
  }

  if (len != 6)
  {
    printk(KERN_ERR "Not enough bytes were sent");
    return -EINVAL;
  }

  p_data.period_ms = *(u32 *)buf;
  p_data.duty_cycle = (u32)*(buf + sizeof(u32));
  p_data.on_period_ms = p_data.period_ms * p_data.duty_cycle / 100;
  p_data.off_period_ms = p_data.period_ms * (100 - p_data.duty_cycle) / 100;
  p_data.blink_state = (bool)*(buf + sizeof(u32) + 1);
  printk(KERN_DEBUG "period_ms %u", p_data.period_ms);
  printk(KERN_DEBUG "on_period %u", p_data.on_period_ms);
  printk(KERN_DEBUG "off_period %u", p_data.off_period_ms);
  printk(KERN_DEBUG "duty_cycle %u", (u32)p_data.duty_cycle);
  kfree(buf);

  if (p_data.blink_state)
  {
    printk("Setting up timer for LED blink");
    setup_timer(&time, led_timer_cb, 0);
    res = mod_timer(&time, jiffies + msecs_to_jiffies(p_data.period_ms));
    if (res != 0)
    {
      printk("Error in modifying timer");
      return -EFAULT;
    }
    p_data.led_state = true;
    gpio_set_value(USR3_LED, p_data.led_state);
    p_data.timer_set = true;
  }
  else
  {
    if (p_data.timer_set)
    {
      del_timer(&time);
      p_data.timer_set = false;
    }
    printk(KERN_INFO "Not setting up timer, blink_state is off");
  }
  return len;
}

static s32 led_release(struct inode * node, struct file * fp)
{
  printk(KERN_DEBUG "release()");
  p_data.read_state = false;
  kfree(fp->private_data);
  return 0;
}

static void led_timer_cb(unsigned long data)
{
  s32 ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  p_data.led_state = !p_data.led_state;
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(p_data.led_state ? p_data.on_period_ms : p_data.off_period_ms));
  if (!(ret >= 0))
  {
    printk(KERN_INFO "Error in modifying timer");
  }
  gpio_set_value(USR3_LED, p_data.led_state);
}
