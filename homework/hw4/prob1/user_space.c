#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define DEVICE ("/dev/led_module0")
#define CMD_SIZE (7)
#define BUF_SIZE (256)

typedef struct cmd {
  uint32_t period_ms;
  uint8_t duty_cycle;
  uint8_t read_command;
  uint8_t blink;
} cmd_t;

typedef enum read_cmd {
  READ_ALL,
  READ_PERIOD,
  READ_DUTY_CYCLE,
  READ_LED_STATE,
  READ_BLINK_STATE
} read_cmd_t;

cmd_t cmd0 = {
  .period_ms = 500,
  .duty_cycle = 25,
  .read_command = READ_ALL,
  .blink = 0
};

static inline void print_cmd(cmd_t * command)
{
  printf("Writing a command with the following information\n");
  printf("Period %d\n", command->period_ms);
  printf("Duty Cycle %d\n", command->duty_cycle);
  printf("Read command %d\n", command->read_command);
  printf("Blink %d\n\n", command->blink);
}

static inline void print_rsp(char * buffer)
{
  printf("Response from module\n");
  printf("%s\n", buffer);
}

static inline int copy_cmd(cmd_t * command, char * buf)
{
  if (buf == NULL)
  {
    printf("Buffer is null");
    return -1;
  }

  memcpy(buf, &command->period_ms, sizeof(command->period_ms));
  buf += sizeof(command->period_ms);
  memcpy(buf, &command->duty_cycle, sizeof(command->duty_cycle));
  buf += sizeof(command->duty_cycle);
  memcpy(buf, &command->read_command, sizeof(command->read_command));
  buf += sizeof(command->read_command);
  memcpy(buf, &command->blink, sizeof(command->blink));
  return 0;
}

static inline int32_t run_cmd(int fd, cmd_t * cmd)
{
  char buffer[BUF_SIZE] = {0};
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

  printf("Writing nothing to driver, it should fail gracefully\n");
  res = write(fd, buffer, 0);
  if (res > 0)
  {
    printf("This should have failed");
    return -1;
  }

  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.blink = 1;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.blink = 0;
  cmd0.read_command = READ_BLINK_STATE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.read_command = READ_PERIOD;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.read_command = READ_DUTY_CYCLE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.blink = 1;
  cmd0.period_ms = 1000;
  cmd0.duty_cycle = 50;
  cmd0.read_command = READ_LED_STATE;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  cmd0.read_command = READ_ALL;
  if (run_cmd(fd, &cmd0) == -1)
  {
    return -1;
  }

  close(fd);
  return 0;
}
