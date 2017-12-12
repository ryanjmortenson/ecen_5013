#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "air.h"
#include "humidity.h"
#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_helper.h"
#include "project_defs.h"
#include "temp.h"
#include "server_task.h"
#include "workers.h"

// Abort signal for all threads
extern int abort_signal;

// Set task id for SEND_LOG* messages
static const task_id_t TASK_ID = MAIN_TASK;

// For converting temp units enum to string
extern char * temp_units_str[];

// For converting task id enum to a string
extern char * task_str[];

// Array of heartbeat registrations
hb_reg_t hb_reg[TASK_ID_LIST_END];

void * hum_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  humidity_rsp_t * rsp = (humidity_rsp_t *)msg->msg;
  SEND_LOG_HIGH("Humidity rsp: %f", ((float) rsp->humidity) / 1024);
  return NULL;
}

void * air_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  air_rsp_t * rsp = (air_rsp_t *)msg->msg;
  SEND_LOG_HIGH("Air rsp: %d, %d", rsp->type, rsp->reading);
  return NULL;
}

void * light_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  light_rsp_t * rsp = (light_rsp_t *)msg->msg;
  SEND_LOG_HIGH("Light rsp lux: %f", rsp->lux);
  return NULL;
}

void * is_dark_rsp_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  is_dark_rsp_t * rsp = (is_dark_rsp_t *)msg->msg;
  if (rsp->dark > 0)
  {
    SEND_LOG_FATAL("Is dark response indicates dark");
  }
  else
  {
    SEND_LOG_FATAL("Is dark response indicates light");
  }
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

#ifndef TIVA
void * hb_handler(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * msg = (message_t *)param;
  int32_t res;
  hb_reg_t * reg = &hb_reg[msg->from];
  struct itimerspec new_value;

  memset(&new_value, 0, sizeof(new_value));
  new_value.it_value.tv_sec = reg->period_seconds * 3;

  SEND_LOG_LOW("Received heartbeat from %s", task_str[msg->from]);
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
    new_value.it_value.tv_sec = hb_setup->period_seconds * 3;
    reg->period_seconds = hb_setup->period_seconds;
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
    LOG_MED("Register task %s for heartbeat period %d s timerid %p",
            task_str[msg->from],
            hb_setup->period_seconds,
            &reg->timerid);
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
      clean_up_socket();
      del_timers();
    }
  }
}

void init_timers()
{
  memset(hb_reg, 0, TASK_ID_LIST_END*sizeof(*hb_reg));
}

void del_timers()
{
  for (uint16_t i = 0; i < TASK_ID_LIST_END; i++)
  {
    if (hb_reg[i].in_use == 1)
    {
      timer_delete(hb_reg[i].timerid);
    }
  }
  init_timers();
}
#endif // TIVA

void sigint_handler(int sig)
{
  FUNC_ENTRY;

  // Set the abort signalf
  SIGNAL_ABORT();

  // Flush queue
  flush_queue();
}

