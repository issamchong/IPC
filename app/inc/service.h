/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2019, Issam Almustafa <Lssam.almustafa@gmail.com>
 * All rights reserved.
 * Date: 2019/07/25
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

	volatile  uint8_t operation;
	volatile  uint8_t size;
	volatile char* data[55];
	volatile char* dataProcessed[55];
};

typedef  struct MedirPerformance{

	volatile  uint8_t estado ;
	volatile  uint32_t id_of_package;
	volatile  uint8_t * payload;
	volatile  uint32_t tiempo_de_llegada;
	volatile  uint32_t tiempo_de_recepcion;
	volatile  uint32_t tiempo_de_inicio;
	volatile  uint32_t tiempo_de_fin;
	volatile  uint32_t tiempo_de_salida;
	volatile  uint32_t tiempo_de_transmision;
	volatile  uint16_t package_length;
	volatile  uint16_t  alocated_memory;

};

typedef  struct Program_Memory{

	volatile  uint8_t ServerStartStack;
	volatile  uint8_t DriverStartStack;
	volatile  uint8_t Task1StartStack;
	volatile  uint8_t Task2StartStack;
	volatile  uint8_t ServerStartHeap;
	volatile  uint8_t DriverStartHeap;
	volatile  uint8_t Task1StartHeap;
	volatile  uint8_t Task2StartHeap;

	volatile  uint8_t ServerEndStack;
	volatile  uint8_t DriverEndStack;
	volatile  uint8_t Task1EndStack;
	volatile  uint8_t Task2EndStack;
	volatile  uint8_t ServerEndHeap;
	volatile  uint8_t DriverEndHeap;
	volatile  uint8_t Task1EndHeap;
	volatile  uint8_t Task2EndHeap;

};

typedef struct MedirPerformance *Token_pt;									//Define a pointer type that can only points at this type of structure

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
int GetOp(char *op,const  char* buffer );										//This function extracts the operation flag from the  data
int GetMsg(char *data,const  char* buffer,uint8_t size );						//This function removes Start Of Frame and End Of Frame from data since it is not needed
int  EndTask(TaskHandle_t *handle, const uint8_t i);							//This function ends suspends the task and prompts a message
int fsmMesurePerformance(Token_pt t, uint8_t *pLoad,uint16_t memSize);
int CompileToken(Token_pt pt, char *msg);													//This function compiles the token message in a string form from the integer fields of the token structure
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _SERVER_H_ */
