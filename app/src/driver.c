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
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint8_t *msg[99];
uint8_t v;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

// Funcion que se ejecuta periodicamente

//This function copies the data portion of the the message buffer to data  and casts data type to read-only on MsgBuffer inside this block only
int GetMsg(char *data, const char* buffer ){



	//buffer[1]='G';
	memmove(data,buffer+1,sizeof(buffer)-1);


}

void uart_config(int baud,bool_t state )
{
     uartConfig(UART_USB,baud);
	 //uartInterrupt( UART_USB, state); // Habilitar el ISR del uart
}

/*==================[end of file]============================================*/
