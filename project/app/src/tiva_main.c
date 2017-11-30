#include <stdint.h>
#include <stdbool.h>

// Free RTOS includes
#include "FreeRTOS.h"
#include "task.h"

// TIVA Ware include
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"


#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "utils/lwiplib.h"
#include "utils/locator.h"
#include "utils/ustdlib.h"

// Project includes
#include "air.h"
#include "light.h"
#include "workers.h"
#include "pthread_wrapper.h"
#include "main_task.h"
#include "mqueue_wrapper.h"
#include "temp.h"

int32_t abort_signal;
extern uint32_t task_heap_low;
extern uint32_t TASK_HEAP_SIZE;

/*!
* @brief Prep the onboard led
* @param[in] task_handle handle to the task
* @param[in] task_name tasks name
*/
void prep_led()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
}

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
static void main_thread(void *params)
{
  uint8_t led_state = 0;
  int32_t res;


  uint32_t ui32User0, ui32User1;
  uint8_t pui8MAC[6];

  //
  // Get the MAC address from the user registers.
  //
  ROM_FlashUserGet(&ui32User0, &ui32User1);
  if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
  {
    res = *(uint32_t *)NULL;
  }

  //
  // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
  // address needed to program the hardware registers, then program the MAC
  // address into the Ethernet Controller registers.
  //
  pui8MAC[0] = ((ui32User0 >>  0) & 0xff);
  pui8MAC[1] = ((ui32User0 >>  8) & 0xff);
  pui8MAC[2] = ((ui32User0 >> 16) & 0xff);
  pui8MAC[3] = ((ui32User1 >>  0) & 0xff);
  pui8MAC[4] = ((ui32User1 >>  8) & 0xff);
  pui8MAC[5] = ((ui32User1 >> 16) & 0xff);

  //
  // Lower the priority of the Ethernet interrupt handler.  This is required
  // so that the interrupt handler can safely call the interrupt-safe
  // FreeRTOS functions (specifically to send messages to the queue).
  //
  ROM_IntPrioritySet(INT_EMAC0, 0xC0);

  lwIPInit(120000000, pui8MAC, 0, 0, 0, IPADDR_USE_DHCP);

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

  res = init_air();
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }
  message_t msg = MSG_INIT(LOG, MAIN_TASK, MAIN_TASK);
  mqd_t msg_q = get_writeable_queue();
  for(;;)
  {
    led_state = ~led_state;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led_state);
    send_msg(msg_q, &msg, NULL, 0);
    usleep(1000000);

  }
  PTHREAD_RETURN(NULL);
}

/*!
* @brief Main task that starts up all sub tasks, bail out mechanism is dereferencing
* null to get into a fault handler for inspecting back trace.
* @return 0
*/
int main() {
  pthread_t task;
  int32_t res;
  HeapRegion_t heap_regions[2] = {0};
  heap_regions[0].pucStartAddress = (uint8_t *)&task_heap_low;
  heap_regions[0].xSizeInBytes = (uint32_t)&TASK_HEAP_SIZE;


  // Set frequency for 120 MHz which seems to be required by the ethernet
  SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);
  MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
              SYSCTL_OSC_MAIN |
              SYSCTL_USE_PLL |
              SYSCTL_CFG_VCO_480), 120000000);

  // Get led ready to blink
  prep_led();

  vPortDefineHeapRegions(heap_regions);

  // Through main thread off to start other threads
  res = pthread_create(&task, NULL, main_thread, NULL);
  if (res != 0)
  {
    res = *(uint32_t *)NULL;
  }
  vTaskStartScheduler();
  return 0;
}
