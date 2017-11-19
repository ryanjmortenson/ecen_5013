/** @file user_space.h
*
* @brief Header for user space functions
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef __USER_SPACE_H__
#define __USER_SPACE_H__

#define DEVICE ("/dev/led_module0")
#define BUF_SIZE (256)

// Command structure used to send data to module
typedef struct cmd {
  uint32_t period_ms;
  uint8_t duty_cycle;
  uint8_t read_command;
  uint8_t blink;
} cmd_t;

void print_cmd(cmd_t * command);
void print_rsp(char * buffer);
int32_t run_cmd_ext(int32_t fd, cmd_t * cmd, char * buffer, int32_t len);
#endif // __USER_SPACE_H__

