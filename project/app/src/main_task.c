/** @file main_task.c
*
* @brief Main project tasks
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "temp.h"
#include "workers.h"

#ifdef BBB
#include "led.h"
#define USR3_LED (3)
#endif // BBB

typedef struct hb_reg {
  timer_t timerid;
  uint32_t period_seconds;
  uint8_t in_use;
} hb_reg_t;

extern char * temp_units_str[];

hb_reg_t hb_reg[TASK_ID_LIST_END];

static const task_id_t TASK_ID = MAIN_TASK;

// Abort signal for all threads
int abort_signal = 0;
mqd_t msg_q;

void * light_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  light_rsp_t * rsp = (light_rsp_t *)msg->msg;
  SEND_LOG_HIGH("Light rsp lux: %f", rsp->lux);
  return NULL;
}

void * temp_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  temp_rsp_t * rsp = (temp_rsp_t *)msg->msg;

  SEND_LOG_HIGH("Temp rsp units: %s, temp: %f",
                temp_units_str[rsp->temp_units],
                rsp->temp);
  return NULL;
}

void * hb_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  int32_t res;
  hb_reg_t * reg = &hb_reg[msg->from];
  struct itimerspec new_value;

  memset(&new_value, 0, sizeof(new_value));
  new_value.it_interval.tv_sec = reg->period_seconds;

  SEND_LOG_LOW("Received heartbeat from %d", msg->from);
  if (reg->timerid != 0)
  {
    res = timer_settime(reg->timerid, 0, &new_value, NULL);
    if (res < 0)
    {
      LOG_ERROR("Couldn't set timer, %s aborting", strerror(errno));
      SIGNAL_ABORT();
    }
  }
  return NULL;
}

void hb_timeout_handler(int sig, siginfo_t * info, void * data)
{
  FUNC_ENTRY;
  for (uint16_t i = 0; i < TASK_ID_LIST_END; i++)
  {
    if (info->si_value.sival_ptr == &hb_reg[i].timerid)
    {
      LOG_ERROR("Timer for %d expired aborting", i);
#ifdef BBB
      set_brightness(USR3_LED, ON_BRIGHTNESS);
#endif // BBB
      SIGNAL_ABORT();
    }
  }
}

void sigint_handler(int sig)
{
  FUNC_ENTRY;

  // Set the abort signalf
  SIGNAL_ABORT();

  // Flush queue
  flush_queue();
}

void * hb_setup(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  hb_setup_t * hb_setup = (hb_setup_t *)msg->msg;
  hb_reg_t * reg = &hb_reg[msg->from];
  status_t status = SUCCESS;
  int res;
  struct sigevent sevp;
  struct itimerspec new_value;

  do
  {
    if (reg->in_use == 1)
    {
      LOG_ERROR("Heartbeat for %d already in use", msg->from);
      break;
    }

    memset(&sevp, 0, sizeof(sevp));
    sevp.sigev_notify = SIGEV_SIGNAL;
    sevp.sigev_signo = SIGUSR1;
    sevp.sigev_value.sival_ptr = &reg->timerid;
    res = timer_create(CLOCK_MONOTONIC, &sevp, &reg->timerid);
    if (res < 0)
    {
      LOG_ERROR("Couldn't create timer, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    memset(&new_value, 0, sizeof(new_value));
    new_value.it_value.tv_sec = hb_setup->period_seconds * 2;
    new_value.it_interval.tv_sec = hb_setup->period_seconds;
    res = timer_settime(reg->timerid, 0, &new_value, NULL);
    if (res < 0)
    {
      LOG_ERROR("Couldn't set timer, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    res = timer_gettime(reg->timerid, &new_value);
    if (res < 0)
    {
      LOG_ERROR("Couldn't get timer, %s", strerror(errno));
      status = FAILURE;
      break;
    }
  } while (0);

  if (status == FAILURE)
  {
    SIGNAL_ABORT();
  }
  else
  {
    LOG_MED("Register task %d for heartbeat period %d s timerid %p",
            msg->from,
            hb_setup->period_seconds,
            &reg->timerid);
  }

  return NULL;
}

status_t init_main_task(int argc, char *argv[])
{
  FUNC_ENTRY;
  char * file_name;
  uint32_t num_workers = 1;
  status_t status = SUCCESS;
  int32_t res;
  struct sigaction timer_handler;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize log to print errors
  log_init();

  memset(hb_reg, 0, sizeof(*hb_reg)*TASK_ID_LIST_END);

  // The first argument is required which is the log name
  if (argc < 2)
  {
    LOG_ERROR("The log name is a required argument");
    return FAILURE;
  }
  else
  {
    file_name = argv[1];
    LOG_HIGH("Log output file is %s", file_name);
  }

  if (argc > 2)
  {
    num_workers = atoi(argv[2]);
    if (num_workers < 1 || num_workers > 20)
    {
      LOG_ERROR("Number of worker threads must be between 1 and 20");
      return FAILURE;
    }
  }

  // Initialize the rest
  do
  {
    // Register signal handler
    res = sigaction(SIGINT, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT handler %s", strerror(errno));
      status = FAILURE;
      break;
    }

    res = sigaction(SIGTERM, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT handler %s", strerror(errno));
      status = FAILURE;
      break;
    }

    timer_handler.sa_sigaction = hb_timeout_handler;
    timer_handler.sa_flags     = SA_SIGINFO;

    res = sigaction(SIGUSR1, &timer_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGEV_SIGNAL handler %s", strerror(errno));
      status = FAILURE;
      break;
    }
    if (init_workers(num_workers) != SUCCESS)
    {
      LOG_ERROR("Could not initialize workers");
      res = FAILURE;
      break;
    }

    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }

    if (register_cb(LIGHT_RSP, MAIN_TASK, light_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register light response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(TEMP_RSP, MAIN_TASK, temp_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register temp response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(HEARTBEAT_SETUP, MAIN_TASK, hb_setup) != SUCCESS)
    {
      LOG_ERROR("Could not register heartbeat setup handler");
      status = FAILURE;
      break;
    }

    if (register_cb(HEARTBEAT, MAIN_TASK, hb_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register heartbeat setup handler");
      status = FAILURE;
      break;
    }

    if (log_msg_init(file_name) != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_temp() != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_light() != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

#ifdef BBB
    if (init_led(USR3_LED) != SUCCESS)
    {
      LOG_ERROR("Could not initialize led");
      status = FAILURE;
      break;
    }
#endif // BBB
    SEND_INIT_COMPLETE();
  } while(0);

#ifdef BBB
  if (status == FAILURE)
  {
    set_brightness(USR3_LED, ON_BRIGHTNESS);
  }
#endif // BBB
  return status;
}

status_t dest_main_task()
{
  status_t status = SUCCESS;

  if (dest_light() != SUCCESS)
  {
    LOG_ERROR("Could not destory light task");
    status = FAILURE;
  }

  if (dest_temp() != SUCCESS)
  {
    LOG_ERROR("Could not destory temp task");
    status = FAILURE;
  }

  if (log_msg_dest() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }

  if (unregister_cb(LIGHT_RSP, MAIN_TASK, light_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat handler");
    status = FAILURE;
  }

  if (unregister_cb(TEMP_RSP, MAIN_TASK, temp_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat handler");
    status = FAILURE;
  }

  if (unregister_cb(HEARTBEAT, MAIN_TASK, hb_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat handler");
    status = FAILURE;
  }

  if (unregister_cb(HEARTBEAT_SETUP, MAIN_TASK, hb_setup) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat setup handler");
    status = FAILURE;
  }

  if (dest_workers() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }
  log_destroy();
  return status;
}

void main_task()
{
  FUNC_ENTRY;
  int count = 0;

  while(!abort_signal)
  {
    get_temp_f(count % 2, MAIN_TASK);
    send_light_req(count % 2, MAIN_TASK);
    count++;
    usleep(5000000);
  }
}

status_t send_hb_setup(uint32_t period_seconds, task_id_t from)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(HEARTBEAT_SETUP, TASK_ID, from);
  hb_setup_t setup = {.period_seconds = period_seconds};

  if (send_msg(msg_q, &msg, &setup, sizeof(setup)) != SUCCESS)
  {
    LOG_ERROR("Could not send heartbeat setup request");
    status = FAILURE;
  }
  return status;
}

status_t send_hb(task_id_t from)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(HEARTBEAT, TASK_ID, from);
  uint8_t byte;

  if (send_msg(msg_q, &msg, &byte, 1) != SUCCESS)
  {
    LOG_ERROR("Could not send heartbeat");
    status = FAILURE;
  }
  return status;
}
