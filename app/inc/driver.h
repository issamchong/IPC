/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
 *===========================================================================*/

#ifndef _DRIVER_H_
#define _DRIVER_H_

/*==================[inclusions]=============================================*/

#include "sapi.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

// Funcion que inicializa la tarea
void uart_config(int baud, bool_t state);

//This function copies the data portion of the the message buffer to data  and casts data type to read-only on MsgBuffer inside this block only
int GetMsg(char *data,const  char* buffer );
//This function gets the operation byte and saves it into operation buffer
int GetOp(char *op,const  char* buffer );
int Send2Qu(QueueHandle_t *handler,const  char* Msg_Only,const char* Msg_Op);



/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _TASK2_H_ */
