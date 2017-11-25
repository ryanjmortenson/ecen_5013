#include "FreeRTOS.h"
#include "string.h"
#include "task.h"
#include "light.h"
#include "workers.h"
#include "pthread_wrapper.h"
#include "main_task.h"
#include "mqueue_wrapper.h"
#include "temp.h"

int32_t abort_signal;

/*!
* @brief Statck overflow function required by freertos
* @param[in] task_handle handle to the task
* @param[in] task_name tasks name
*/
void vApplicationStackOverflowHook(xTaskHandle task_handle, char *task_name)
{
  for(;;);
}

/*!
* @brief Temporary handler
* @param[in] params for function
* @return NULL
*/
static void * cb (void * params)
{
  return NULL;
}

/*!
* @brief Test task that just sends messages empy log messages
* @param[in] parameters for function
* @return NULL
*/
static void task(void *params)
{
  message_t msg = MSG_INIT(LOG, MAIN_TASK, MAIN_TASK);
  mqd_t msg_q = get_writeable_queue();
  for(;;)
  {
    send_msg(msg_q, &msg, NULL, 0);
    usleep(100);
  }
  PTHREAD_RETURN(NULL);
}

/*!
* @brief Main task that starts up all sub tasks, bail out mechanism is dereferencing
* null to get into a fault handler for inspecting back trace.
* @return 0
*/
int main() {
  pthread_t test;
  int32_t res;

  res = pthread_create(&test, NULL, task, NULL);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = init_workers(3);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = register_cb(LOG, MAIN_TASK, cb);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = register_cb(HEARTBEAT, MAIN_TASK, cb);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = register_cb(HEARTBEAT_SETUP, MAIN_TASK, cb);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = initialize_q();
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = init_light();
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }

  res = init_temp();
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }
  vTaskStartScheduler();
  return 0;
}
