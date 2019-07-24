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
#define TASK1_1
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
volatile xSemaphoreHandle MsgHandle_key=0;
volatile xSemaphoreHandle DataProcessed_key=0;
volatile QueueHandle_t MsgHandle;
volatile QueueHandle_t MsgHandle_2;
volatile QueueHandle_t DataProcessed_handle;
//char HexFrame[110]="7B313530546869732069732052544F5320636F757273652054503120696E20746573742C616E6420697420697320776F726B696E67217D";
char HexFrame[110]="7B323530546869732069732052544F5320636F757273652054503120696E20746573742C616E6420697420697320776F726B696E67217D"; 														//The Frame will be stored in this buffer
char AsciFrame[55]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
struct Program_Memory Report;
struct Frame message;																					//Only Server has access to that structure


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task

void server(void){

	/* Inspect our own high water mark on entering the Server. */
	volatile TaskStatus_t xTaskDetails;
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
	Report.ServerStartStack=(const)xTaskDetails.usStackHighWaterMark;
	Report.ServerStartHeap=(const)xPortGetFreeHeapSize();

	char MsgFromDriver[sizeof(AsciFrame)]="\0";
	char MsgToDriver[sizeof(AsciFrame)]="\0";
	char *f=MsgFromDriver;
	char flag[2]="\0";


	char SizeData[3]="\0";

// Checking if new message is available and where to send it
    while(1){

    	if(!(xQueueReceive(MsgHandle,MsgFromDriver,1000))){                          									//Important QueueReceive clears msg when called again
    		uartWriteString(UART_USB,"Server <-Driver: No received\n");
    	}else{
    		SizeData[0]=MsgFromDriver[1];
    		SizeData[1]=MsgFromDriver[2];
    		flag[0]=MsgFromDriver[0];
    		message.size= atoi(SizeData);
    		message.operation=	atoi(*f);
    		strcpy(message.data,MsgFromDriver+3);
    		printf("Server-> Report: Msg size %d\n",message.size);
    		printf("Server <-Driver:Received message is:\n %s\n",message.data);
    		vTaskDelay(500);
    	}

        switch(*f)
        {
        	case '0':

        		printf("Server-> Flag- %c\n",*f);
        		if(!(xQueueSend(QeueMayusculizador,message.data,50))){
        			uartWriteString(UART_USB,"Server-> Task1: No sent\n");
        		}else{
        			vTaskDelay(1000);
        			if(!(xQueueReceive(DataProcessed_handle,message.dataProcessed,1000))){                          									//Important QueueReceive clears msg when called again
        				uartWriteString(UART_USB,"Server <- Task1: No received\n");
        			}else{
        				printf("Server <-Task1: Received- %s\n",message.dataProcessed);
        			}
        		}
        		break;

        	case '1':
    			printf("Server-> Report: Flag  %c\n",*f);
				vTaskDelay(500);
        		if(!(xQueueSend(QeueMinusculizador,message.data,50))){
        			uartWriteString(UART_USB,"Server-> Task2: No sent\n");
		        }else{
		        	vTaskDelay(1000);
		        	if(!(xQueueReceive(DataProcessed_handle,message.dataProcessed,1000))){                          									//Important QueueReceive clears msg when called again
		        		uartWriteString(UART_USB,"Server <- Task2: No received\n");
		        	}else{
		        		strcat(MsgToDriver,flag);
		        		strcpy(MsgToDriver+1,SizeData);
		        		strcpy(MsgToDriver+3,message.dataProcessed);
		        		if(!(xQueueSend(MsgHandle_2,(const)MsgToDriver,1000))){
		        			uartWriteString(UART_USB,"Server -> Driver: No sent\n");
		        			}else{
				        		printf("Server -> Driver: Sent msg:\n%s\n",MsgToDriver);
		        				vTaskDelay(4000);
		        			}
		        		}
		        }
        	    break;
        	case '2':
    			printf("Server-> Report: Flag  %c\n",*f);
        		printf("Server -> Report: Total remaining stack size is %d\n",Report.DriverEndStack +Report.ServerEndStack+Report.Task1EndStack+Report.Task2EndStack);

        	    break;
        	case '\0':
        		uartWriteString(UART_USB,"Server -> Report: No Flag \n");
        		break;

        	default:
        		break;
        }
    	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
    	Report.ServerEndStack=(const)xTaskDetails.usStackHighWaterMark;
    	Report.ServerEndHeap=(const)xPortGetFreeHeapSize();;
    }
}

void driver(void){


	/* Inspect our own high water mark on entering the Server. */
	volatile TaskStatus_t xTaskDetails;
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
	Report.DriverStartStack=(const)xTaskDetails.usStackHighWaterMark;
	Report.DriverStartHeap=(const)xPortGetFreeHeapSize();


	char data_2Server[sizeof(AsciFrame)]="\0";
	char data_FromServer[sizeof(AsciFrame)]="\0";
	char op[2]="\0";
	char SOF='{';
	char EnOF='}';

	// Initialize local variables
	ASCI(HexFrame,sizeof(HexFrame),AsciFrame);
	printf("Driver-> Report: Frame received - %s \n",AsciFrame);

	//validate the AsciFrame format
	if(!((AsciFrame[0]==SOF) && (AsciFrame[sizeof(AsciFrame)-1]==EnOF))){
		uartWriteString(UART_USB,"Driver-> Report: Invalid Frame \n");
		}else{
			uartWriteString(UART_USB,"Driver-> Report: Valid Frame \n");
			if(!GetData(data_2Server,AsciFrame,sizeof(AsciFrame))){
				uartWriteString(UART_USB,"Driver <- GetData: Failed\N");
			}
		}

	while(1){


			if(!(xQueueSend(MsgHandle,data_2Server,1000))){
				uartWriteString(UART_USB,"Driver-> Server: No sent\n");
			}else{
				vTaskDelay(4000);
			}
			if(!(xQueueReceive(MsgHandle_2,data_FromServer,1000))){
				uartWriteString(UART_USB,"Driver <- Server: No Received\n");
			}else{
				uartWriteString(UART_USB,"Driver <- Server: Received\n");
				vTaskDelay(500);

				}
			vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
		    Report.DriverEndStack=(const)xTaskDetails.usStackHighWaterMark;
		    Report.DriverEndHeap=(const)xPortGetFreeHeapSize();
	}
}

//This task converts the message letters to upper case
void task1(void){
	
	/* Inspect our own high water mark on entering the Server. */
		volatile TaskStatus_t xTaskDetails;
		vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
		Report.Task1StartStack=(const)xTaskDetails.usStackHighWaterMark;
		Report.Task1StartHeap=(const)xPortGetFreeHeapSize();

		char Task1Buffer[sizeof(AsciFrame)]="\0";
		while(1){
			if(QeueMayusculizador !=0){
				if(!(xQueueReceive(QeueMayusculizador,Task1Buffer,3000))){
					uartWriteString(UART_USB," Task1 <- Server : No received\n");
				}else
					if(!UperCase(Task1Buffer)){                           						  		  	  //convert message letters to upper case
						uartWriteString(UART_USB,"Task1 -> Report: No lower case\n");
					}else{
						if(xSemaphoreTake(DataProcessed_key,1000)){
							uartWriteString(UART_USB,"Task1 -> Server: No key\n");
						}else{
							if(!(xQueueSend(DataProcessed_handle,Task1Buffer,50))){
								uartWriteString(UART_USB,"Task1-> Server: No sent \n");
							}
							xSemaphoreGive(DataProcessed_key);
						}
					}

			}else{
				EndTask(&Task1Handle,1);
			}
			vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
			Report.Task1EndStack=(const)xTaskDetails.usStackHighWaterMark;
			Report.Task1EndHeap=(const)xPortGetFreeHeapSize();
			vTaskDelay(3000);

			//printf("Task 1: available heap is %d\n",Report.Task1EndHeap);
		}
}

//This function converts to lower case letter the message
void task2(void){

	/* Inspect our own high water mark on entering the Server. */
	volatile TaskStatus_t xTaskDetails;
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
	Report.Task2StartStack=(const)xTaskDetails.usStackHighWaterMark;
	Report.Task2StartHeap=(const)xPortGetFreeHeapSize();

	char Task2Buffer[sizeof(AsciFrame)]="\0";
	while(1){
		if(QeueMinusculizador !=0){
			if(!(xQueueReceive(QeueMinusculizador,Task2Buffer,2000))){
				uartWriteString(UART_USB," Task2 <- Server : No received\n");
			}else
				if(!LwrCase(Task2Buffer)){                           						  		  	  //convert message letters to upper case
					uartWriteString(UART_USB,"Task2 -> Report: No lower case\n");
				}else{
					if(xSemaphoreTake(DataProcessed_key,1000)){
						uartWriteString(UART_USB,"Task2 -> Server: No key\n");
					}else{
						if(!(xQueueSend(DataProcessed_handle,Task2Buffer,50))){
							uartWriteString(UART_USB,"Task2-> Server: No sent \n");
						}
						xSemaphoreGive(DataProcessed_key);
					}
				}

		}else{
			EndTask(&Task2Handle,2);
		}
		vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);
		Report.Task2EndStack=(const)xTaskDetails.usStackHighWaterMark;
		Report.Task2EndHeap=(const)xPortGetFreeHeapSize();
		vTaskDelay(3000);

	//	printf("Task 2: available heap is %d\n",Report.Task2EndHeap);
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
   DataProcessed_handle =xQueueCreate(1, sizeof(AsciFrame));
   DataProcessed_key=xSemaphoreCreateMutex();
   MsgHandle_key=xSemaphoreCreateMutex();
   MsgHandle_2=xQueueCreate(1, sizeof(AsciFrame));


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
