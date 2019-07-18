/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "operations.h"
#include "service.h"
#include "driver.h"
#include "queue.h"
#include "semphr.h"



/*==================[definiciones y macros]==================================*/
#define TASK1_0
#define TASK2_0
#define SERVER_1
#define SERVER_B1_0
#define SERVER_B2_1
#define DRIVER_1

/*==================[definiciones de datos internos]=========================*/

// List of variable to be created first before executing any task
volatile TaskHandle_t ServHandle = NULL;
volatile TaskHandle_t DrivHandle = NULL;
volatile TaskHandle_t Task1Handle = NULL;
volatile TaskHandle_t Task2Handle = NULL;
volatile QueueHandle_t QeueMayusculizador;
volatile QueueHandle_t QeueMinusculizador;
volatile xSemaphoreHandle *QTrans_key=0;
volatile QueueHandle_t MsgHandle;
char MsgBuffer[10]="{1hAppy}"; 														//The message will be stored in this buffer
struct Frame message;


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task
void server(void){

#ifdef SERVER_B1_1
    char i=MsgBuffer[0];

	QeueMayusculizador =xQueueCreate(1, sizeof(MsgBuffer));
    switch(i)
    {
    	case '0':
    		QeueMayusculizador =xQueueCreate(1, sizeof(MsgBuffer));
    		if(!xQueueSend(QeueMayusculizador,MsgBuffer,50)){								//Send message to Queue from the buffer then verify if returned 1
    			uartWriteString(UART_USB,"Error: could not send message to Task 1");									//Error handling if the returned value is not 1

    		}
    		break;

    	case '1':
    		QeueMinusculizador =xQueueCreate(1, sizeof(MsgBuffer));
    	    if(!xQueueSend(QeueMinusculizador,MsgBuffer,50)){								//Send message to Queue from the buffer then verify if returned 1
    	    	uartWriteString(UART_USB,"Error: could not send message to Task 2");									//Error handling if the returned value is not 1
    	    	}
    	    break;
    	default:
			uartWriteString(UART_USB,"Not such operation");
    }
#endif


    while(1){

    	if(xSemaphoreTake(QTrans_key,1000)){
    		uartWriteString(UART_USB,"Server has the Queue key\n");
    		//Send2Qu(QeueTransmitir,MsgBuffer,op);				//Send message with operation flag to server if the correct operation  byte is provided
    		xSemaphoreGive(QTrans_key);
    		uartWriteString(UART_USB,"Server released Queue key\n");
    	} else
    		uartWriteString(UART_USB,"Server Could get key");
    	vTaskDelay(1000);
    }
}

void driver(void){


#ifdef DRIVER_B1_1
	//Local variables definitions and declaration
	char data[100]="\0";															//This i
	char op[2]="\0";
	char SOF='{';
	char EnOF='}';
	volatile QueueHandle_t *QeueTransmitir;

	//validate the frame format
	if(!((MsgBuffer[0]==SOF) || (MsgBuffer[0]==EnOF))){
		uartWriteString(UART_USB,"Invalid frame format\n");
		}else
			uartWriteString(UART_USB,"valid frame format\n");

    // Get the message portion from the frame
	if(!GetMsg(data,MsgBuffer)){
		uartWriteString(UART_USB,"Could not get message");

	} else
		printf("data is %s\r\n",data);

	// Get the operation byte and save it in op
	if(!GetOp(op,MsgBuffer)){

		uartWriteString(UART_USB,"No op");

	} else

		if(xSemaphoreTake(QTrans_key,1000)){

			uartWriteString(UART_USB,"Driver has the Queue key");
			Send2Qu(QeueTransmitir,MsgBuffer,op);				//Send message with operation flag to server if the correct operation  byte is provided
			xSemaphoreGive(QTrans_key);
			uartWriteString(UART_USB,"Driver released Queue key");
		}else
			uartWriteString(UART_USB,"Access failed");
#endif DRIVER_B1_1

	while(1){

		if(xSemaphoreTake(QTrans_key,1000)){

				uartWriteString(UART_USB,"Driver has the Queue key\n");
			//	Send2Qu(QeueTransmitir,MsgBuffer,op);				//Send message with operation flag to server if the correct operation  byte is provided
				xSemaphoreGive(QTrans_key);
				//uartWriteString(UART_USB,"Driver released Queue key\n");

		}else
			uartWriteString(UART_USB,"Driver Could not get key\n");
		vTaskDelay(1000);
	}
}

//This task converts the message letters to upper case
void task1(void){
	
	char MsgBuffer[6];
	while(1){
		if(QeueMayusculizador !=0){                                							 		  	  // verify if the queue was created
			if(xQueueReceive(QeueMayusculizador,MsgBuffer,0)){  							  			  // check if received any new message, if not go to sleep 3 second
				if(!UperCase(MsgBuffer)){                           						  		  	  //convert message letters to upper case
					uartWriteString(UART_USB,"Task 1: Error could not convert to upper case");
				}else
					vTaskDelay(2000);																	  // Sleep this task 1 second since it has already processed the msg and also to allow checking task2 without interruption

			} else
				vTaskDelay(1000);
				uartWriteString(UART_USB,"Error: No new message received for Task 1 \n");    // Error handling if no new message is received

		 }else
			 EndTask(&Task1Handle,1);   			  // Error showing the queue was not created yet
	}
}

//This function converts to lower case letter the message
void task2(void){

	char MsgBuffer[5];
	while(1){
		if(QeueMinusculizador !=0){
			if(xQueueReceive(QeueMinusculizador,MsgBuffer,100)){
				if(!LwrCase(MsgBuffer)){                           						  		  	  //convert message letters to upper case
					uartWriteString(UART_USB,"Task 2: Error could not convert to lower  case");
				}else
					vTaskDelay(2000);
			}else
				vTaskDelay(1000);

				uartWriteString(UART_USB," Error: No new message received for Task 2  \n");

		}else
			EndTask(&Task2Handle,2);
	}
}
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma

   QTrans_key = xSemaphoreCreateMutex();
   boardConfig();
   uart_config(9600,0);           //configurar el uart baud rate y habilita y dishabilita usb interrupt
   //UART2_IRQHandler();
   // Crear driver en freeRTOS
   xTaskCreate(
      driver,                     // Funcion de la tarea a ejecutar
      (const char *)"driver",     // Nombre de la tarea como String amigable para el usuario, se pasa com const para solo leer
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      DrivHandle                  // Puntero al handler de la tarea
   );

#ifdef SERVER_1
// Crear tarea server en freeRTOS
   xTaskCreate(
      server,                     	 // Fun cion de la tarea a ejecutar
      (const char *)"server",     	 // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, 	 // Cantidad de stack de la tarea
      0,                          	 // Parametros de tarea
      tskIDLE_PRIORITY+1,         	 // Prioridad de la tarea
      0                 			 // Puntero a la tarea creada en el sistema
   );
#endif


#ifdef TASK1_1
  // Crear tarea task1 en freeRTOS
   xTaskCreate(
      task1,                     // Funcion de la tarea a ejecutar
      (const char *)"task1",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      Task1Handle                  // Puntero a la tarea creada en el sistema
   );
#endif

#ifdef TASK2_1
  // Crear tarea task1 en freeRTOS
   xTaskCreate(
      task2,                     // Funcion de la tarea a ejecutar
      (const char *)"task2",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      Task2Handle                  // Puntero a la tarea creada en el sistema
   );
 #endif



   // Iniciar scheduler
   vTaskStartScheduler();

}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/


/*==================[fin del archivo]========================================*/
