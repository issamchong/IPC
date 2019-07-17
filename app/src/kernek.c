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
#define SERVER_0
#define DRIVER_1
/*==================[definiciones de datos internos]=========================*/

volatile TaskHandle_t ServHandle = NULL;
volatile TaskHandle_t DrivHandle = NULL;
volatile TaskHandle_t Task1Handle = NULL;
volatile TaskHandle_t Task2Handle = NULL;
volatile QueueHandle_t QeueMayusculizador;
volatile QueueHandle_t QeueMinusculizador;
volatile QueueHandle_t QeueTransmitir;
volatile xSemaphoreHandle QuTransmitir=0;
volatile QueueHandle_t MsgHandle;
char MsgBuffer[10]="{1hAppy}"; 														//The message will be stored in this buffer
struct Frame message;


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task
void server(void){

	//declaring local variables
    char i=MsgBuffer[0];
	int notificationValue;

	//creating queue to receive message from driver
	QeueMayusculizador =xQueueCreate(1, sizeof(MsgBuffer));
// Check operation byte to decide what task the message should  be sent to
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
	notificationValue=ulTaskNotifyTake(pdTRUE, (TickType_t)portMAX_DELAY); 			// guardar las notificaciones en este variable, se 											//resetea automatico a zero por tener pdTRUE
}

void driver(void){

	//Local variables definitions and declaration
	char data[100]="\0";
	char op[2]="\0";																//Initialize an empty buffer
	char SOF='{';
	char EnOF='}';
	char MsgAnOp[101]="\0";

	//Create a semaphore for data protection
	QuTransmitir = xSemaphoreCreateMutex();


// validate the frame format

	if(!((MsgBuffer[0]==SOF) || (MsgBuffer[0]==EnOF))){
		uartWriteString(UART_USB,"Invalid frame format\n");
		}else
			uartWriteString(UART_USB,"valid frame format\n");

// Verify if message or operation could be extracted
	if(!GetMsg(data,MsgBuffer)){													// This function extracts the message portion of the frame and saves it into data buffer
		uartWriteString(UART_USB,"Could not get message");							// Print error if message was not extracted

	} else
		printf("data is %s\r\n",data);												// Print data buffer to verify

	if(!GetOp(op,MsgBuffer)){														// This function extracts the message portion of the frame and saves it into data buffer

		uartWriteString(UART_USB,"No op");											// Print error if operation byte  was not extracted

	} else
		printf("operation  is %s\r\n",op);											// Print the operation to test
		strcpy(MsgAnOp,op);
		strcat(MsgAnOp,data);
		printf("operation  is %s\r\n",MsgAnOp);
		QeueTransmitir =xQueueCreate(1, sizeof(MsgAnOp));

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
