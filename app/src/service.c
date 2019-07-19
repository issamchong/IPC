/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "service.h"  // <= own header

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/



// This function suspends the task, it takes the task handle and the task number, the number to decide the message
int EndTask(TaskHandle_t *handle,const uint8_t i){


      vTaskDelay(500);
      switch (i)
      {
      case 1:
    	  uartWriteString(UART_USB,"Task 1 suspended, no Queue created.  \n");
    	  break;
      case 2:
    	  uartWriteString(UART_USB,"Task 2 suspended, no Queue created.  \n");
    	  break;
      default:
    	  uartWriteString(UART_USB,"No such task.  \n");
      }
      vTaskSuspend(*handle);
	  return 1;
}





/*==================[end of file]============================================*/
