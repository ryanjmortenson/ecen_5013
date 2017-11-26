/** @file i2c.c
*
* @brief I2C for tiva
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// TIVA includes
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Project includes
#include "i2c.h"
#include "log.h"
#include "project_defs.h"
#include "FreeRTOS.h"
#include "pthread_wrapper.h"


typedef struct bus_mutex {
  pthread_mutex_t lock;
  uint16_t refcount;
} bus_mutex_t;

struct i2c_descriptor {
  int32_t addr;
};

i2c_descriptor_t desc;

bus_mutex_t bus_lock = {0};

static uint8_t initialized = 0;
static void init_gpio()
{
  if (initialized == 0)
  {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
    I2CMasterInitExpClk(I2C0_BASE, 16000000, true);
    initialized = 1;
  }
}

status_t i2c_init(int32_t i2c_bus, i2c_descriptor_t ** i2cd, uint8_t addr)
{
  FUNC_ENTRY;
  int32_t res;
  status_t status = SUCCESS;

  // Initialize GPIO
  init_gpio();

  do
  {
    *i2cd = malloc(sizeof(**i2cd));
    if (*i2cd == NULL)
    {
      status = FAILURE;
      break;
    }
    (*i2cd)->addr = addr;

    if (bus_lock.refcount == 0)
    {
      res = pthread_mutex_init(&bus_lock.lock, NULL);
      if (res < 0)
      {
        status = FAILURE;
        break;
      }
      bus_lock.refcount++;
    }
  } while(0);
  return status;
}

status_t i2c_write_byte(i2c_descriptor_t * i2cd, uint8_t byte)
{
  FUNC_ENTRY;
  return i2c_write_bytes(i2cd, &byte, 1);
}

status_t i2c_write_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  // Check for a null pointer for unit tests
  CHECK_NULL(bytes);

  pthread_mutex_lock(&bus_lock.lock);
  I2CMasterSlaveAddrSet(I2C0_BASE, i2cd->addr, false);
  if (len == 1)
  {
    I2CMasterDataPut(I2C0_BASE, *bytes);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(I2C0_BASE));
  }
  else
  {
    for (uint8_t i = 0; i < len - 1; i++)
    {
      bytes += i;
      I2CMasterDataPut(I2C0_BASE, *bytes);
      I2CMasterControl(I2C0_BASE, (i == 0) ? I2C_MASTER_CMD_BURST_SEND_START : I2C_MASTER_CMD_BURST_SEND_CONT);
      while (I2CMasterBusy(I2C0_BASE));
    }
    I2CMasterDataPut(I2C0_BASE, *(bytes + 1));
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while (I2CMasterBusy(I2C0_BASE));
  }
  pthread_mutex_unlock(&bus_lock.lock);
  return SUCCESS;
}
status_t i2c_read_byte(i2c_descriptor_t * i2cd, uint8_t * byte)
{
  FUNC_ENTRY;
  // Check for a null pointer for unit tests
  CHECK_NULL(byte);
  return i2c_read_bytes(i2cd, byte, 1);
}

status_t i2c_read_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  // Check for a null pointer for unit tests
  CHECK_NULL(bytes);
  pthread_mutex_lock(&bus_lock.lock);
  I2CMasterSlaveAddrSet(I2C0_BASE, i2cd->addr, true);
  if (len == 1)
  {
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while (I2CMasterBusy(I2C0_BASE));
    *bytes = I2CMasterDataGet(I2C0_BASE);
  }
  else
  {
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while (I2CMasterBusy(I2C0_BASE));
    *bytes = I2CMasterDataGet(I2C0_BASE);
    for (uint8_t i = 1; i < len; i++)
    {
      I2CMasterControl(I2C0_BASE, (i == len - 1) ? I2C_MASTER_CMD_BURST_RECEIVE_FINISH : I2C_MASTER_CMD_BURST_RECEIVE_CONT);
      while (I2CMasterBusy(I2C0_BASE));
      *(bytes + i) = I2CMasterDataGet(I2C0_BASE);
    }
  }
  pthread_mutex_unlock(&bus_lock.lock);
  return SUCCESS;
}

status_t i2c_dest(i2c_descriptor_t * i2cd)
{
  FUNC_ENTRY;

  // Decrement reference count
  bus_lock.refcount--;

  // Cleanup mutex if reference count is 0
  if (bus_lock.refcount == 0)
  {
    pthread_mutex_destroy(&bus_lock.lock);
  }
  free(i2cd);
  return SUCCESS;
}
