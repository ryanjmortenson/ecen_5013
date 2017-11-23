#include "FreeRTOS.h"
#include "string.h"
#include "task.h"

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

static void
task(void *pvParameters)
{
  for(volatile int i = 0; i < 1000; i++)
  {
    i = i;
  }
}

int main() {
  xTaskCreate(task, "LED", 512, NULL, 3, NULL);
  vTaskStartScheduler();
  return 0;
}
