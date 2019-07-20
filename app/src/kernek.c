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
#define TASK2_1
#define SERVER_1
#define DRIVER_1
#define DRIVER_B1_1
#define DRIVER_B2_1
#define Task2_Test_0
/*==================[definiciones de datos internos]=========================*/

// List of variable to be created first before executing any task
volatile TaskHandle_t ServHandle = NULL;
volatile TaskHandle_t DrivHandle = NULL;
volatile TaskHandle_t Task1Handle = NULL;
volatile TaskHandle_t Task2Handle = NULL;
volatile QueueHandle_t QeueMayusculizador;
volatile QueueHandle_t QeueMinusculizador;
volatile xSemaphoreHandle QTrans_key=0;
volatile xSemaphoreHandle QuT1_key=0;
volatile xSemaphoreHandle QuT2_key=0;
volatile QueueHandle_t MsgHandle;
char HexFrame[110]="7B313530546869732069732052544F5320636F757273652054503120696E20746573742C616E6420697420697320776F726B696E67217D"; 														//The message will be stored in this buffer
char AsciFrame[55]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task

void server(void){

	struct Frame message;
	char msg[sizeof(AsciFrame)]="\0";
	char *f=msg;
	char SizeData[3]="\0";

// Checking if new message is available and where to send it
    while(1){

    	if(!(xSemaphoreTake(QTrans_key,1000))){
    		uartWriteString(UART_USB,"Server: Key was not released\n");
    	}else{
    		if(!(xQueueReceive(MsgHandle,msg,1000))){                          									//Important QueueReceive clears msg when called again
    			uartWriteString(UART_USB,"Server: Could not access Queue\n");
    			}else{
    				xSemaphoreGive(QTrans_key);vTaskDelay(1000);
    				SizeData[0]=msg[1];
    				SizeData[1]=msg[2];
    				message.size= atoi(SizeData);
    				message.operation=	atoi(*f);
    				printf("Server: size of data is %d\n",message.size);
    			}
    		}vTaskDelay(500);

        switch(*f)
        {
        	case '0':

        		if(xSemaphoreTake(QuT1_key,1000)){
  		        	xQueueSend(QeueMayusculizador,msg,50);
        		    xSemaphoreGive(QuT1_key);
        		}vTaskDelay(1000);

        		break;

        	case '1':
    			printf("Server: operation flag is %c\n",*f);
        		if(xSemaphoreTake(QuT2_key,1000)){
        			xQueueSend(QeueMinusculizador,msg,50);
		        	xSemaphoreGive(QuT2_key);
		        }vTaskDelay(1000);
        	    break;
        	case '\0':
        		uartWriteString(UART_USB,"Server: No message from Driver\n");
        		break;

        	default:
        		break;
        }
    }
}

void driver(void){


// Set up section of Driver
#ifdef DRIVER_B1_1
	//Local variables definitions and declaration
	char data[sizeof(AsciFrame)]="\0";
	char op[2]="\0";
	char SOF='{';
	char EnOF='}';

	ASCI(HexFrame,sizeof(HexFrame),AsciFrame);
	printf("Driver: frame  is %s \n",AsciFrame);

	//validate the AsciFrame format
	if(!((AsciFrame[0]==SOF) && (AsciFrame[sizeof(AsciFrame)-1]==EnOF))){
		uartWriteString(UART_USB,"Driver: Invalid AsciFrame format\n");
		}else{
			uartWriteString(UART_USB,"Driver: valid AsciFrame format\n");
			if(!GetMsg(data,AsciFrame,sizeof(AsciFrame))){
				uartWriteString(UART_USB,"Driver: Could not get message");
			} else
				if(!GetOp(op,AsciFrame)){
					uartWriteString(UART_USB,"Driver: No operation");
				} else
					strcat(op,data);
					printf("Driver: Message to server is %s\r\n",op);
		}


#endif DRIVER_B1_1

//Sending new messages to Server only only key is available
#ifdef DRIVER_B2_1
	while(1){

		if(xSemaphoreTake(QTrans_key,1000)){
			xQueueSend(MsgHandle,op,1000);
			xSemaphoreGive(QTrans_key);
		}vTaskDelay(3000);
	}
#endif DRIVER_B2_1
}

//This task converts the message letters to upper case
void task1(void){
	
	VTaskDelay(1000);
	char MsgBuffer[6];
	while(1){
		if(QeueMayusculizador !=0){                                							 		  	  // verify if the queue was created
			if(xQueueReceive(QeueMayusculizador,MsgBuffer,0)){  							  			  // check if received any new message, if not go to sleep 3 second
				if(!UperCase(MsgBuffer)){                           						  		  	  //convert message letters to upper case
					uartWriteString(UART_USB,"Task 1: Could not convert to upper case");
				}else
					vTaskDelay(2000);																	  // Sleep this task 1 second since it has already processed the msg and also to allow checking task2 without interruption

			} else
				vTaskDelay(1000);
				uartWriteString(UART_USB,"Task1: No new message received for Task 1 \n");    // Error handling if no new message is received
		 }else
			 EndTask(&Task1Handle,1);   			  // Error showing the queue was not created yet
	}
}

//This function converts to lower case letter the message
void task2(void){

#ifdef Task2_Test_1
	QeueMinusculizador =xQueueCreate(1, sizeof(MsgBuffer));
	xQueueSend(QeueMinusculizador,MsgBuffer,50);
#endif Task2_Test_1

	char Task2Buffer[sizeof(AsciFrame)]="\0";
	while(1){
		if(QeueMinusculizador !=0){
			if(xQueueReceive(QeueMinusculizador,Task2Buffer,3000)){
				if(!LwrCase(Task2Buffer)){                           						  		  	  //convert message letters to upper case
					uartWriteString(UART_USB,"Task 2: Could not convert to lower  case\n");
				}
			}else
				uartWriteString(UART_USB," Task 2 : No new message received for Task 2  \n");
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

   //Create Mutex and Queues
   QeueMayusculizador =xQueueCreate(1, sizeof(AsciFrame));
   QeueMinusculizador =xQueueCreate(1, sizeof(AsciFrame));
   MsgHandle =xQueueCreate(1, sizeof(AsciFrame));
   QTrans_key = xSemaphoreCreateMutex();
   QuT1_key = xSemaphoreCreateMutex();
   QuT2_key = xSemaphoreCreateMutex();

#ifdef DRIVER_1
   // Create tasks
   xTaskCreate(
      driver,                     // Funcion de la tarea a ejecutar
      (const char *)"driver",     // Nombre de la tarea como String amigable para el usuario, se pasa com const para solo leer
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+2,         // Prioridad de la tarea
      DrivHandle                  // Puntero al handler de la tarea
   );
#endif DRIVER_1


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

#endif SERVER_1

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
