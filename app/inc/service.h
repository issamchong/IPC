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
#include "queue.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

// This structure contains information about the frame sent
typedef  struct Frame{

	volatile const uint8_t Operation;
	const char sizeT[2];
	volatile char* data[100];
	const uint8_t heap_task1;
	const uint8_t heap_task2;
};
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/


int  EndTask(TaskHandle_t *handle, const uint8_t i);
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _SERVER_H_ */
