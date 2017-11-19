/** @file user_space.c
*
* @brief LED module test
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "led_module.h"
#include "user_space.h"

#define CMD_SIZE (7)

// An initial command that will be changed throughout testing
static cmd_t cmd0 = {
  .period_ms = 500,
  .duty_cycle = 25,
  .read_command = READ_ALL,
  .blink = 0
};

void print_cmd(cmd_t * command)
{
  printf("Writing a command with the following information\n");
  printf("Period %d\n", command->period_ms);
  printf("Duty Cycle %d\n", command->duty_cycle);
  printf("Read command %d\n", command->read_command);
  printf("Blink %d\n\n", command->blink);
}

void print_rsp(char * buffer)
{
  printf("Response from module\n");
  printf("%s\n", buffer);
}

int32_t run_cmd_ext(int32_t fd, cmd_t * cmd, char * buffer, int32_t len)
{
  int res;

  print_cmd(cmd);
  if (copy_cmd(cmd, buffer))
  {
    printf("Copy command failed");
    return -1;
  }

  res = write(fd, buffer, CMD_SIZE);
  if (res < 0)
  {
    printf("Failed to write to led module");
    return -1;
  }

  lseek(fd, 0, SEEK_SET);
  memset(buffer, 0, BUF_SIZE);
  res = read(fd, buffer, BUF_SIZE);
  if (res < 0)
  {
    printf("Failed to read to led module");
    return -1;
  }
  print_rsp(buffer);
}

/*!
* @brief Copy a command structure into a buffer
* @param command to copy
* @param buffer to copy command into
* @return status of copy
*/
static inline int copy_cmd(cmd_t * command, char * buf)
{
  if (buf == NULL)
  {
    printf("Buffer is null");
    return -1;
  }

  // Copy structure into linear buffer
  memcpy(buf, &command->period_ms, sizeof(command->period_ms));
  buf += sizeof(command->period_ms);
  memcpy(buf, &command->duty_cycle, sizeof(command->duty_cycle));
  buf += sizeof(command->duty_cycle);
  memcpy(buf, &command->read_command, sizeof(command->read_command));
  buf += sizeof(command->read_command);
  memcpy(buf, &command->blink, sizeof(command->blink));
  return 0;
}

/*!
* @brief Run a command and get current status of led module
* @param command to run
* @return status of running command
*/
static inline int32_t run_cmd(int32_t fd, cmd_t * cmd)
{
  char buffer[BUF_SIZE] = {0};
  return run_cmd_ext(fd, cmd, buffer, BUF_SIZE);
}

/*!
* @brief Run a bunch of commands to exercise led module
* @return status of executing commands
*/
#ifndef NO_MAIN
int main()
{
  int fd = 0;
  int res = 0;
  char buffer[BUF_SIZE];

  fd = open(DEVICE, O_RDWR);
  if (fd < 0)
  {
    printf("Couldn't open %s\n", DEVICE);
    return 0;
  }

  // Write poorly formatted command to led module which should handle it
  // gracefully
  printf("Writing nothing to driver, it should fail gracefully\n");
  res = write(fd, buffer, 0);
  if (res > 0)
  {
    printf("This should have failed");
    return -1;
  }

  // Run a correctly formatted command
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted command with blink on
  cmd0.blink = 1;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted command with blink off and read blink state
  cmd0.blink = 0;
  cmd0.read_command = READ_BLINK_STATE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read period
  cmd0.read_command = READ_PERIOD;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read duty cycle
  cmd0.read_command = READ_DUTY_CYCLE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read led state
  cmd0.blink = 1;
  cmd0.period_ms = 1000;
  cmd0.duty_cycle = 50;
  cmd0.read_command = READ_LED_STATE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and all
  cmd0.read_command = READ_ALL;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  // Close file descriptor
  close(fd);
  return 0;
}
#endif // NO_MAIN
