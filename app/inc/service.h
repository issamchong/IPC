/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
 *===========================================================================*/

#ifndef _SERVICE_H_
#define _SERVICE_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef  struct Frame{
							//Since it is read only flag and crucial; it is defined as const
	const uint8_t SOF;
	volatile const uint8_t OP;
	const char size[2];
	volatile char* data[100];
	const uint8_t EnOF;
	const uint8_t heap_task1;
	const uint8_t heap_task2;
};
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/


int ReadData();  // leer el mensaje y verifique protocolo, utiliza driver.h
int SendData();  // leer de la cola lo que dejo el task selecionado, verifique protocolo y envia al driver.h	
int Op_selector(char  *msg);  // analiza el mensaje y decide a que task lo envia via  ReadData()
int  EndTask(TaskHandle_t *handle, const uint8_t i);
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _SERVER_H_ */
