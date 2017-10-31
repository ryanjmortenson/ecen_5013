/** @file mock_i2c.c
*
* @brief Mock I2C for workstation development
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <fcntl.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "i2c.h"
#include "log.h"
#include "project_defs.h"

#define I2C_PATH "/dev/i2c-"
#define I2C_PATH_MAX_LEN (256)

struct i2c_descriptor {
  int32_t fd;
  int32_t addr;
};

status_t i2c_init(int32_t i2c_bus, i2c_descriptor_t ** i2cd, uint8_t addr)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  char i2c_path[I2C_PATH_MAX_LEN] = {0};
  int32_t res;
  int32_t fd;

  do
  {
    snprintf(i2c_path, I2C_PATH_MAX_LEN, "%s%d", I2C_PATH, i2c_bus);
    *i2cd = malloc(sizeof(**i2cd));
    if (*i2cd == NULL)
    {
      LOG_ERROR("Could not malloc a new i2c descriptor");
      status = FAILURE;
      break;
    }

    LOG_HIGH("Opening I2C device %s", i2c_path);
    fd = open(i2c_path, O_RDWR);
    if (fd < 0)
    {
      LOG_ERROR("Could not open %s, %s", i2c_path, strerror(errno));
      status = FAILURE;
      break;
    }

    LOG_MED("Initializing communication with address 0x%02x", addr);
    res = ioctl(fd, I2C_SLAVE, addr);
    if (res < 0)
    {
      LOG_ERROR("Could not open %s, %s", i2c_path, strerror(errno));
      status = FAILURE;
      break;
    }
  } while(0);

  if (status == SUCCESS)
  {
    LOG_FATAL("fd: %d", fd);
    (*i2cd)->fd = fd;
    (*i2cd)->addr = addr;
  }
  return status;
}

status_t i2c_dest(i2c_descriptor_t * i2cd)
{
  free(i2cd);
  close(i2cd->fd);
  return SUCCESS;
}

status_t i2c_write_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  int32_t res;
  status_t status = SUCCESS;

  res = write(i2cd->fd, bytes, len);
  if (res != len)
  {
    LOG_ERROR("Could not write %d bytes to addr: %d, %s",
              len,
              i2cd->addr,
              strerror(errno));
    status = FAILURE;
  }
  return status;
}

status_t i2c_write_byte(i2c_descriptor_t * i2cd, uint8_t byte)
{
  FUNC_ENTRY;
  return i2c_write_bytes(i2cd, &byte, 1);
}

status_t i2c_read_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  int32_t res;
  status_t status = SUCCESS;

  res = read(i2cd->fd, bytes, len);
  if (res != len)
  {
    LOG_ERROR("Could not read %d byte from addr: %d", len, i2cd->addr);
    status = FAILURE;
  }
  return status;
}

status_t i2c_read_byte(i2c_descriptor_t * i2cd, uint8_t * byte)
{
  FUNC_ENTRY;
  return i2c_read_bytes(i2cd, byte, 1);
}

