#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "project_defs.h"
#include "spi.h"

#define DEVICE "/dev/spidev1.0"
#define EXPORT_FILE "/sys/class/gpio/export"
#define GPIO_CSN_VALUE "/sys/class/gpio/gpio60/value"
#define GPIO_CSN_DIRECTION "/sys/class/gpio/gpio60/direction"
#define GPIO_CE_VALUE "/sys/class/gpio/gpio30/value"
#define GPIO_CE_DIRECTION "/sys/class/gpio/gpio30/direction"
#define MAX_BUF_SIZE 32
#define OUT "out"

static int32_t fd;
const static int32_t mode = 0;
const static int32_t bits_per_word = 8;
const static int32_t max_speed_hz = 50000;
static uint8_t rx_byte = 0;
static FILE * export;
static FILE * csn_value;
static FILE * ce_value;
static FILE * direction;

uint8_t spi_init()
{
  // Get access to the device
  if ((fd = open(DEVICE, O_RDWR)) < 0)
  {
    return FAILURE;
  }

  // Write the mode tothe device
  if (ioctl(fd, SPI_IOC_WR_MODE32, &mode) == -1)
  {
    close(fd);
    return FAILURE;
  }

  // Write the words per bit to the device
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) == -1)
  {
    close(fd);
    return FAILURE;
  }

  // Write the max speed in hz to the device
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &max_speed_hz) == -1)
  {
    close(fd);
    return FAILURE;
  }

  // Open export file
  if((export = fopen(EXPORT_FILE, "w")) == 0)
  {
    close(fd);
    return FAILURE;
  }

  // Make GPIO 60 j9 pin 12 an output CSN
  fseek(export, 0, SEEK_SET);
  fprintf(export, "%d", 60);

  // Make GPIO 30 j9 pin 11 an output CSN
  fseek(export, 0, SEEK_SET);
  fprintf(export, "%d", 30);
  fflush(export);
  fclose(export);

  // Make the CSN pin an output
  if((direction = fopen(GPIO_CSN_DIRECTION, "w")) == 0)
  {
     close(fd);
     fclose(export);
     return FAILURE;
  }
  fseek(direction, 0, SEEK_SET);
  fprintf(direction, OUT);
  fflush(direction);
  fclose(direction);

  // Make the CE pin an output
  if((direction = fopen(GPIO_CE_DIRECTION, "w")) == 0)
  {
     close(fd);
     fclose(export);
     return FAILURE;
  }
  fseek(direction, 0, SEEK_SET);
  fprintf(direction, OUT);
  fflush(direction);
  fclose(direction);

  // Set initial value of CSN to high (active low)
  if ((csn_value = fopen(GPIO_CSN_VALUE, "w")) == 0)
  {
     close(fd);
     fclose(export);
     fclose(direction);
     return FAILURE;
  }
  fseek(csn_value, 0, SEEK_SET);
  fprintf(csn_value, "%d", 1);
  fflush(csn_value);

  // Set initial value of CE to low (active low)
  if ((ce_value = fopen(GPIO_CE_VALUE, "w")) == 0)
  {
     close(fd);
     fclose(export);
     fclose(direction);
     return FAILURE;
  }
  fseek(ce_value, 0, SEEK_SET);
  fprintf(ce_value, "%d", 0);
  fflush(ce_value);

  return SUCCESS;
} // spi_init()

void spi_send_byte(uint8_t byte)
{
  // Reset rx byte
  rx_byte = 0;

  // Construct spi transfer structure
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)&byte,
    .rx_buf = (unsigned long)&rx_byte,
    .len = 1,
    .delay_usecs = 0,
    .speed_hz = max_speed_hz,
    .bits_per_word = bits_per_word
  };

  // Make a spi ioctl call
  ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
} // spi_send_byte()

void spi_send_bytes(uint8_t * bytes, uint32_t length)
{
  // Reset rx byte
  rx_byte = 0;

  // Construct spi transfer structure
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)bytes,
    .rx_buf = (unsigned long)&rx_byte,
    .len = length,
    .delay_usecs = 0,
    .speed_hz = max_speed_hz,
    .bits_per_word = bits_per_word
  };

  // Make a spi ioctl call
  ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
} // spi_send_bytes()

uint8_t spi_receive_byte()
{
  // Return the rx_byte
  return rx_byte;
} // spi_receive_byte()

void spi_shutdown()
{
  // Close all open files
  close(fd);
  fclose(csn_value);
  fclose(ce_value);
} // spi_shutdown()

void spi_csn_enable()
{
  // Write a 0 to the value file
  fprintf(csn_value, "%d", 0);

  // Flush value to file
  fflush(csn_value);
} // spi_csn_enable()

void spi_csn_disable()
{
  // Write a 0 to the value file
  fprintf(csn_value, "%d", 1);

  // Flush value to file
  fflush(csn_value);
} // spi_csn_disable()

void spi_ce_enable()
{
  // Write a 0 to the value file
  fprintf(ce_value, "%d", 1);

  // Flush value to file
  fflush(ce_value);
} // spi_ce_enable()

void spi_ce_disable()
{
  // Write a 0 to the value file
  fprintf(ce_value, "%d", 0);

  // Flush value to file
  fflush(ce_value);
} // spi_ce_disable()
