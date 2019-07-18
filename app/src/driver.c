/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "driver.h"  // <= own header


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
QueueHandle_t handle;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint8_t *msg[99];
uint8_t v;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


//This function copies the data portion of the the message buffer to data  and casts data type to read-only on MsgBuffer inside this block only
int GetMsg(char *data, const char* buffer ){

	//buffer[1]='G';                          			//code to validate read-only permission
	memcpy(data,buffer+2,6);
}

// This functions sets the baud rate of the USB UART
void uart_config(int baud,bool_t state )
{
     uartConfig(UART_USB,baud);
	 //uartInterrupt( UART_USB, state); 				// Enable UART interrupt
}

//This function extracts the operation byte from the frame and saves it into a operation buffer
int GetOp(char *op, const char* buffer ){

	op[0]='0';
}
int Send2Qu(QueueHandle_t *handler,const  char* Msg_Only,const char* Msg_Op){


	strcat(Msg_Op,Msg_Only);
	printf("Message with operation flag   is %s\r\n",Msg_Op);
	handler =xQueueCreate(1, sizeof(Msg_Op));

}

/*==================[end of file]============================================*/
