/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2019, Issam Almustafa <Lssam.almustafa@gmail.com>
 * All rights reserved.
 * Date: 2019/07/25
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



int GetMsg(char *data, const char* buffer,uint8_t size ){

	memcpy(data,buffer+2,size-2);
	return 1;
}
int GetOp(char *op, const char* buffer ){

	op[0]=buffer[1];

	return 1;
}
int EndTask(TaskHandle_t *handle,const uint8_t i){										// This function suspends the task, it takes the task handle and the task number, the number to decide the message



      vTaskDelay(500);
      switch (i)																		//Verify the task number
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
