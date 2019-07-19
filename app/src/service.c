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
char datamsg[10]="{1hAppy}"; 														//The message will be stored in this buffer

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


int  Send2Task(QueueHandle_t *QuMayu,QueueHandle_t *QuMinu ,char *MsgBuf){

	char *i=MsgBuf;
	i++;
	switch(*i)
	    {
	    	case '0':
	    		*QuMayu =xQueueCreate(1, 202);
	    		if(!xQueueSend(*QuMayu,MsgBuf,50)){								//Send message to Queue from the buffer then verify if returned 1
	    			uartWriteString(UART_USB,"Error: could not send message to Task 1\n");									//Error handling if the returned value is not 1
	    		}
	    		break;

	    	case '1':
	    		*QuMinu =xQueueCreate(1, 202);
	    	    if(!xQueueSend(*QuMinu,MsgBuf,50)){								//Send message to Queue from the buffer then verify if returned 1
	    	    	uartWriteString(UART_USB,"Error: could not send message to Task 2\n");									//Error handling if the returned value is not 1
	    	    	}else
	    	    		uartWriteString(UART_USB,"Server: Message sent to Task 2\n");
	    	    		vTaskDelay(1000);
	    	    break;
	    	default:
				uartWriteString(UART_USB,"Not such operation");
	    }
}


/*==================[end of file]============================================*/
