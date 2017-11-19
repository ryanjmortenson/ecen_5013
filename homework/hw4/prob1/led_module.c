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
#define BUF_SIZE (256)
#define USR3_LED (56)
#define TIMER_MS (500)

// Module info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Mortenson");
MODULE_DESCRIPTION("LED Control module");
MODULE_VERSION("0");

// Function declarations
static s32 led_open(struct inode * node, struct file * fp);
static s32 led_release(struct inode * node, struct file * fp);
static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset);
static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset);
static loff_t led_llseek(struct file * fp, loff_t offset, int input);
static void led_timer_cb(unsigned long data);

typedef enum read_cmd {
  READ_ALL,
  READ_PERIOD,
  READ_DUTY_CYCLE,
  READ_LED_STATE,
  READ_BLINK_STATE
} read_cmd_t;

// File operations
struct file_operations led_fops = {
  .owner        = THIS_MODULE,
  .open         = led_open,
  .read         = led_read,
  .write        = led_write,
  .release      = led_release,
  .llseek       = led_llseek
};

// Set of private data to be used in file pointer struct
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

// Global variables
static dev_t led_device;
static struct cdev led_cdev;
static struct timer_list time;
static struct private_data * p_data;

/*!
* @brief Initialize module
* @return status of initializing module
*/
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

/*!
* @brief Destroy module
*/
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

// Register module init and exit
module_init(led_module_init);
module_exit(led_module_exit);

/*!
* @brief Prep anything needed for driving led
* @param inode inode for device file
* @param fp file pointer to device
* @return NULL
*/
static s32 led_open(struct inode * node, struct file * fp)
{
  printk(KERN_DEBUG "open()");
  fp->private_data = p_data;
  return 0;
}

/*!
* @brief Read information about led state
* @param fp file pointer to device
* @param buffer user space buffer
* @param len length to read
* @param offset offset to start reading at
* @return how may bytes in buffer
*/
static ssize_t led_read(struct file * fp, char __user * buffer, size_t len, loff_t * offset)
{
  s32 res = 0;
  s32 count = 0;
  char * buf = NULL;
  struct private_data * pd = (struct private_data *)fp->private_data;

  printk(KERN_DEBUG "read()");
  printk(KERN_DEBUG "read_params %d", pd->read_params);

  // Only allow 1 read (don't continue copying data to buffer) can be reset with llseek
  if (pd->read_state == false)
  {
    // Alloc a buffer to create string
    buf = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (buf == NULL)
    {
      printk(KERN_ERR "Could not malloc memory for kernel to user space copy");
      return -ENOMEM;
    }

    // Populate buffer based on last read command's read byte
    if (pd->read_params == READ_PERIOD)
    {
      count = snprintf(buf, BUF_SIZE, "Period: %d\n", pd->period_ms);
    }
    else if (pd->read_params == READ_DUTY_CYCLE)
    {
      count = snprintf(buf, BUF_SIZE, "Duty Cycle: %d\n", pd->duty_cycle);
    }
    else if (pd->read_params == READ_LED_STATE)
    {
      count = snprintf(buf, BUF_SIZE, "Current Led State: %d\n", pd->led_state);
    }
    else if (pd->read_params == READ_BLINK_STATE)
    {
      count = snprintf(buf, BUF_SIZE, "Current Blink State: %d\n", pd->blink_state);
    }
    else
    {
      count = snprintf(buf, BUF_SIZE, "Period: %d, Duty Cycle: %d, Current Led State: %d, Current Blink State: %d\n",
                       pd->period_ms,
                       pd->duty_cycle,
                       pd->led_state,
                       pd->blink_state);
    }

    // Check count isn't correct
    if (count <= 0)
    {
      printk(KERN_ERR "Could not format message");
      return -EFAULT;
    }

    // Copy to user space
    res = copy_to_user(buffer, buf, count);
    if (res != 0)
    {
      printk(KERN_ERR "Could not copy data to user space");
      return -EFAULT;
    }
    kfree(buf);
  }
  // Set read state to read, can be reset with llseek
  pd->read_state = true;
  return count;
}

/*!
* @brief Write commands
* @param fp file pointer to device
* @param buffer user space buffer
* @param len length to written
* @param offset offset to start writing
* @return bytes written
*/
static ssize_t led_write(struct file * fp, const char __user * buffer, size_t len, loff_t * offset)
{
  char * buf;
  s32 res;
  struct private_data * pd = (struct private_data *)fp->private_data;

  printk(KERN_DEBUG "write()");

  // Validate the length, should be 7 bytes to have all commands
  if (len != 7)
  {
    printk(KERN_ERR "Not enough bytes were sent");
    return -EINVAL;
  }

  // Alloc a buffer to copy userspace information into
  buf = kmalloc(len, GFP_KERNEL);
  if (buf == NULL)
  {
    printk(KERN_ERR "Could not malloc memory for user to kernel space copy");
    return -ENOMEM;
  }

  // Copy from user space
  res = copy_from_user(buf, buffer, len);
  if (res != 0)
  {
    printk(KERN_ERR "Could not copy from user space");
    return -EFAULT;
  }

  // Parse information sent from user space
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

  // Validate the duy cycle between 0 and 100
  if (pd->duty_cycle > 100)
  {
    printk(KERN_ERR "Duty Cycle must be between 0 and 100");
    return -EINVAL;
  }

  // Set up to blink if that was requested
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
      res = mod_timer(&time, jiffies + msecs_to_jiffies(0xffffffff));
      if (res < 0)
      {
        printk("Error in modifying timer");
        return -EFAULT;
      }
      pd->timer_set = false;
    }
    printk(KERN_INFO "Not setting up timer, blink_state is off");
  }
  return len;
}

/*!
* @brief Reset read state so multiple reads can be down with 1 open/close cycle
* @param fp file pointer to device
* @param len length to written
* @param input not used
* @return 0
*/
static loff_t led_llseek(struct file * fp, loff_t offset, int input)
{
  struct private_data * pd = (struct private_data *)fp->private_data;
  pd->read_state = 0;
  return 0;
}

/*!
* @brief Release the file
* @param fp file pointer to device
* @param fp file pointer to device
* @return status
*/
static s32 led_release(struct inode * node, struct file * fp)
{
  struct private_data * pd = (struct private_data *)fp->private_data;
  printk(KERN_DEBUG "release()");
  pd->read_state = false;
  return 0;
}

/*!
* @brief Timer callback
* @param data not used
*/
static void led_timer_cb(unsigned long data)
{
  s32 ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  // Flip the status of the led state
  p_data->led_state = !p_data->led_state;

  // Set the time to either on or off period
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(p_data->led_state ? p_data->on_period_ms : p_data->off_period_ms));
  if (!(ret >= 0))
  {
    printk(KERN_INFO "Error in modifying timer");
  }

  // Set gpio to on or off
  gpio_set_value(USR3_LED, p_data->led_state);
}
