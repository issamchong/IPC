/* Copyright 2019-2020, Issam ALmustafa
 * All rights reserved.
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

/*==================[Inclusions]============================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "operations.h"
#include "service.h"
#include "driver.h"
#include "queue.h"
#include "semphr.h"

/*==================[definitions and  macros]==================================*/
#define TASK1_1
#define TASK2_1
#define SERVER_1
#define DRIVER_1
#define DRIVER_B1_1
#define DRIVER_B2_1
#define Task2_Test_0
/*==================[Definition of internal data]=========================*/
																							// List of variable to be created first before executing any task
volatile TaskHandle_t ServHandle = NULL;
volatile TaskHandle_t DrivHandle = NULL;
volatile TaskHandle_t Task1Handle = NULL;
volatile TaskHandle_t Task2Handle = NULL;
volatile QueueHandle_t QeueMayusculizador;
volatile QueueHandle_t QeueMinusculizador;
volatile xSemaphoreHandle DataProcessed_key=0;
volatile QueueHandle_t MsgHandle;
volatile QueueHandle_t MsgHandle_2;
volatile QueueHandle_t DataProcessed_handle;
static char HexFrame[110]; 																														//The Frame will be stored in this buffer
static char AsciFrame[55]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
static struct Program_Memory Report;
static struct Frame message;
static int InterruptCounter=0;

/*==================[Definition of external data]=========================*/

/*==================[Declaration of internal functions ]====================*/



void Port_Interrupt(void){

	   char c = uartRxRead( UART_USB );
	   HexFrame[InterruptCounter]=c;
	   InterruptCounter++;
}


void server(void){																	// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task

	volatile TaskStatus_t xTaskDetails;												// This variable stores the information about the stack available
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);							// This function stores in the variable declared above the information about the stack, it also requires the task handle as a parameter
	Report.ServerStartStack=(const)xTaskDetails.usStackHighWaterMark;				// The current stack size available is stored in the ServerStartStack of  Report structure and cast type it to read only
	Report.ServerStartHeap=(const)xPortGetFreeHeapSize();							// Get the current available heap size and assign to the ServerStartHeap member of the Report structure and cast type it to read only
	char MsgFromDriver[sizeof(AsciFrame)]="\0";										// Declaring variable that will store the data from the driver, this variable of size AssciFrame define above
	char *MsgToDriver=(char *)pvPortMalloc(sizeof(AsciFrame));						// Declaring variable that will store the data to be sent to the driver, this variable of size AssciFrame define above
	char *f=MsgFromDriver;															// This pointer holds the first operation byte in order to determine the proper operation
	char SizeData[3]="\0";															// This string is used to store the size bytes of the data
    while(1){

    	if(!(xQueueReceive(MsgHandle,MsgFromDriver,1000))){                     	// Check if any message was received and store it in MsgFromDriver buffer, important QueueReceive clears buffer  when called again
    		uartWriteString(UART_USB,"Server <-Driver: No received\n");				// Error capture if message was not received
    	}else{
    		SizeData[0]=MsgFromDriver[1];											// Storing the information about the message size  which correspond to byte 1 and 2
    		SizeData[1]=MsgFromDriver[2];
    		message.size= atoi(SizeData);											// Convert the Size string to integer and assign it to the size entry of the message structure
    		message.operation=	atoi(*f);											// Convert the flag to integer and assign to the operation entry of the message structure
    		strcpy(message.data,MsgFromDriver+3);									// Copy the data portion only to the data entry of the message structure
    		printf("Server-> Report: Msg size %d\n",message.size);					// Server is reporting the data size
    		printf("Server <-Driver:Received message is:\n %s\n",message.data);		// Server is reporting the data received
    		vTaskDelay(500);
    	}

        switch(*f)
        {
        	case '0':

    			printf("Server-> Report: Flag  %c\n",*f);										// Print the operation flag
				vTaskDelay(500);
        		if(!(xQueueSend(QeueMayusculizador,message.data,50))){							//Send data to QeueMinusculizador for task2 to read it
        			uartWriteString(UART_USB,"Server-> Task2: No sent\n");						//Error report if not sent
		        }else{
		        	vTaskDelay(1000);
		        	if(!(xQueueReceive(DataProcessed_handle,message.dataProcessed,1000))){		//Receive processed data from task2 and save it in the message.DataProcessed entry
		        		uartWriteString(UART_USB,"Server <- Task2: No received\n");				// Error report of nothing  received
		        	}else{																		//Server is putting all back together to send processed data back to Driver
		        		MsgToDriver[1]=*f;
		        	    strcpy(MsgToDriver+2,SizeData);											// Add the data size
		        		strcpy(MsgToDriver+4,message.dataProcessed);
		        		if(!(xQueueSend(MsgHandle_2,(const)MsgToDriver,1000))){					//Send the data to Driver
		        			uartWriteString(UART_USB,"Server -> Driver: No sent\n");			//Error handle if not sent
		        			}else{
				        		printf("Server -> Driver: Sent msg:\n%s\n",MsgToDriver);		//If sent successfully, report  the message that was sent
								InterruptCounter=0;												//Reset the interrupt counter to start from zero the buffer
						        vPortFree(MsgToDriver);											//Clear hte heap using heap_2
								vTaskDelay(2000);
		        			}
		        		}
		        }
        		break;

        	case '1':
    			printf("Server-> Report: Flag  %c\n",*f);										// Print the operation flag
				vTaskDelay(500);
        		if(!(xQueueSend(QeueMinusculizador,message.data,50))){							//Send data to QeueMinusculizador for task2 to read it
        			uartWriteString(UART_USB,"Server-> Task2: No sent\n");						//Error report if not sent
		        }else{
		        	vTaskDelay(1000);
		        	if(!(xQueueReceive(DataProcessed_handle,message.dataProcessed,1000))){		//Receive processed data from task2 and save it in the message.DataProcessed entry
		        		uartWriteString(UART_USB,"Server <- Task2: No received\n");				// Error report of nothing  received
		        	}else{																		//Server is putting all back together to send processed data back to Driver

		        		MsgToDriver[1]=*f;
		        		strcpy(MsgToDriver+2,SizeData);											// Add the data size
		        		strcpy(MsgToDriver+4,message.dataProcessed);							//Finally add the message after processing to the same buffer
		        		if(!(xQueueSend(MsgHandle_2,(const)MsgToDriver,1000))){					//Send the data to Driver
		        			uartWriteString(UART_USB,"Server -> Driver: No sent\n");			//Error handle if not sent
		        			}else{
				        		printf("Server -> Driver: Sent msg:\n%s\n",MsgToDriver);		//If sent successfully, report  the message that was sent
				        		InterruptCounter=0;												//Reset the interrupt counter to be able to start the buffer from 0
						        vPortFree(MsgToDriver);											//Clear  the heap
				        		vTaskDelay(2000);
		        			}
		        		}
		        }
        	    break;
        	case '2':															//Report the operation flag
    			printf("Server-> Report: Flag  %c\n",*f);
        		printf("Server -> Report: Total available stack size is %d\n",Report.DriverEndStack +Report.ServerEndStack+Report.Task1EndStack+Report.Task2EndStack);					// Report the total available stack
        		InterruptCounter=0;
				uartInterrupt(UART_USB, true);									//Enable USB interrupt

        	    break;
        	case '3':
            	printf("Server-> Report: Flag  %c\n",*f);																														//Report the operation flag
                printf("Server -> Report: Total available Heap size is %d\n",Report.DriverEndHeap +Report.ServerEndHeap+Report.Task1EndHeap+Report.Task2EndHeap);				//Report the total Heap size
                InterruptCounter=0;
        		uartInterrupt(UART_USB, true);						    //Enable USB interrupt
                break;
        	case '\0':
        		uartWriteString(UART_USB,"Server -> Report: No Flag \n");		//Error handle if operation flag is not available
        		break;

        	default:
        		break;
        }
    	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);					// Get available information about the stack
    	Report.ServerEndStack=(const)xTaskDetails.usStackHighWaterMark;			//Set  entry for Server start stack of Report state machine and cast type it to read only
    	Report.ServerEndHeap=(const)xPortGetFreeHeapSize();						//Set entry for Server start heap of Report state machine and cast type it to read only
    }

}

void driver(void){
																				/* Inspect our own high water mark on entering the Server. */
	volatile TaskStatus_t xTaskDetails;											//This variable stores the information about the stack and if of type volatile since  it changes and should not optimized
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);						//Get current stack size and save it in the variable declared above
	Report.DriverStartStack=(const)xTaskDetails.usStackHighWaterMark;           // Set  DriverStartStack value of Report machine state and cast type it to read only
	Report.DriverStartHeap=(const)xPortGetFreeHeapSize();						//Set DriverStartHeap value of Report machine state

	static char data_2Server[sizeof(AsciFrame)]="\0";							//Buffer to store data to be sent to Server
	static char data_FromServer[sizeof(AsciFrame)]="\0";						//Buffer to store data received from Server
	static char data_2Port[sizeof(AsciFrame)]="\0";						//Buffer to store data received from Server
	char op[2]="\0";															// String to store the operation flag
	char SOF='{';																// This  variable holds the Start Of Frame (SOF) to validate the frame
	char EnOF='}';																//This variable holds the End Of Frame (EOF) used to validate the frame as well


	while(1){

			if( sizeof(HexFrame)>=109){
				uartInterrupt(UART_USB, false);												//Enable USB interrupt
				ASCI(HexFrame,sizeof(HexFrame),AsciFrame);
				if(!((AsciFrame[0]==SOF) && (AsciFrame[sizeof(AsciFrame)-1]==EnOF))){		// Verify if either start of frame or end of frame is not valid
					uartInterrupt(UART_USB, true);											//Enable USB interrupt
					uartWriteString(UART_USB,"Driver-> Report: Invalid Frame \n");			// Error handle if any of the two is not valid
					}else{
						uartWriteString(UART_USB,"Driver-> Report: Valid Frame \n");		//Report valid if all good
						if(!GetData(data_2Server,AsciFrame,sizeof(AsciFrame))){				//GetData function removes the start and ending keys from the frame before sending to Server
							uartWriteString(UART_USB,"Driver <- GetData: Failed\N");		// Error handle if GetData function did not work
						}
					}
				}
			if(!(xQueueSend(MsgHandle,data_2Server,1000))){						//Sending data to Server via MsgHandle queue
				uartWriteString(UART_USB,"Driver-> Server: No sent\n");			// Error handle if message was not sent
			}else{
				vTaskDelay(3000);
			}
			if(!(xQueueReceive(MsgHandle_2,data_FromServer,1000))){				//Check if any message is received from the Server from MsgHandle_2 queue
				//uartWriteString(UART_USB,"Driver <- Server: No Received\n");	//Error handle if nothing  received
			}else{
				data_FromServer[0]='{';
				strcat(data_FromServer,"}");
				printf("Driver <- Server: Received \n%s\n",data_FromServer);		//Report received if successful
				strcpy(data_FromServer,"\0");
				uartInterrupt(UART_USB, true);									//Enable USB interrupt
				vTaskDelay(1000);

				}
			vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);				//Get current stack size information
		    Report.DriverEndStack=(const)xTaskDetails.usStackHighWaterMark;		//Set Driver entry for end stack size  of Report state machine and cast type it to read only
		    Report.DriverEndHeap=(const)xPortGetFreeHeapSize();					// Get current heap size  and set  Driver entry for end heap size of Report state machine
	}
}

																				//This task converts the message letters to upper case
void task1(void){
	
																				/* Inspect our own high water mark on entering the Server. */
		volatile TaskStatus_t xTaskDetails;										//Variable to store current stack information
		vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);					// Get current stack size and store the variable declared above
		Report.Task1StartStack=(const)xTaskDetails.usStackHighWaterMark;		//Set task1 entry for staring stack size of  Report state machine and cast type to read only
		Report.Task1StartHeap=(const)xPortGetFreeHeapSize();					//Set task1 entry  for starting heap size of Report state machine and cast type to read only
		static char Task1Buffer[sizeof(AsciFrame)]="\0";						//Declaring local buffer to store data to be  sent and received

		while(1){
			if(QeueMayusculizador !=0){													 	 //Verify if QeueMayusculizador was created
				if(!(xQueueReceive(QeueMayusculizador,Task1Buffer,1000))){				 	 // Check if anything was received from the queue
					//uartWriteString(UART_USB," Task1 <- Server : No received\n");		     //Error handle if not
				}else
					if(!UperCase(Task1Buffer)){											 	 //Send message to UpperCase function to set letters to capital
						//uartWriteString(UART_USB,"Task1 -> Report: No lower case\n");	  	 //Error handle if message was not sent
					}else{
						if(xSemaphoreTake(DataProcessed_key,1000)){						 	 //Check if key is available to send processed data to Server
							//uartWriteString(UART_USB,"Task1 -> Server: No key\n");		  //Report if key is not available
						}else{
							if(!(xQueueSend(DataProcessed_handle,Task1Buffer,50))){		  	  //Send processed data if all good
								//uartWriteString(UART_USB,"Task1-> Server: No sent \n");	  // Error handle if data was not sent
							}
							xSemaphoreGive(DataProcessed_key);							  	  //Release key after  sending message processed to Server
						}
					}

			}else{
				EndTask(&Task1Handle,1);												  //End task if no handle was never created
			}
			vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);						  //Get current task stack size
			Report.Task1EndStack=(const)xTaskDetails.usStackHighWaterMark;				  //Set Task1  entry for end stack size of Report state machine and cast type it to  read only
			Report.Task1EndHeap=(const)xPortGetFreeHeapSize();							  //Set Task1  entry for end heap size of Report state machine and cast type it to  read only
			vTaskDelay(3000);
		}
}
																						  //This function converts the message to lower case
void task2(void){
																						  /* Inspect our own high water mark on entering the Server. */
	volatile TaskStatus_t xTaskDetails;													  //Variable to store current stack information
	vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);								  //Get current stack size and store the variable declared above;																																//Set task1 entry  for starting heap size of Report state machine and cast type to read only
	Report.Task2StartStack=(const)xTaskDetails.usStackHighWaterMark;					  //Set task2 entry for staring stack size of  Report state machine and cast type to read only
	Report.Task2StartHeap=(const)xPortGetFreeHeapSize();								  //Set task2 entry for staring heap size of  Report state machine and cast type to read only
	static char Task2Buffer[sizeof(AsciFrame)]="\0";									 //Declaring local buffer to store data to be  sent and received

	while(1){
		if(QeueMinusculizador !=0){														 //Verify if QeueMinusculizador was created
			if(!(xQueueReceive(QeueMinusculizador,Task2Buffer,2000))){					 // Check if anything was received from the queue
				//uartWriteString(UART_USB," Task2 <- Server : No received\n");			 //Error handle if not
			}else
				if(!LwrCase(Task2Buffer)){  											 //convert message letters to lower case and check if if it was successful
					//uartWriteString(UART_USB,"Task2 -> Report: No lower case\n");		 //Error handle if conversion did not happen
				}else{
					if(xSemaphoreTake(DataProcessed_key,1000)){							 //Check if key is available to access queue
						//uartWriteString(UART_USB,"Task2 -> Server: No key\n");		 //Error handle if key was not available
					}else{
						if(!(xQueueSend(DataProcessed_handle,Task2Buffer,50))){			 //Check if message was sent
							//uartWriteString(UART_USB,"Task2-> Server: No sent \n");	 //Error handle if message was not sent
						}
						xSemaphoreGive(DataProcessed_key);								 //Release semaphore  key
					}
				}

		}else{
			EndTask(&Task2Handle,2);													 //End task if handler was never created
		}
		vTaskGetInfo(ServHandle,&xTaskDetails,pdTRUE,eInvalid);							 //Get current task stack size
		Report.Task2EndStack=(const)xTaskDetails.usStackHighWaterMark;					 //Set task2 entry for End stack size of  Report state machine and cast type to read only
		Report.Task2EndHeap=(const)xPortGetFreeHeapSize();							     //Set task2 entry for End stack size of  Report state machine and cast type to read only
		vTaskDelay(3000);

	}
}
/*==================[External function declaration ]====================*/

/*==================[Principal function ]======================================*/
																					      // Principal function that runs after reseting or starting up
int main(void)
{

   boardConfig();																		  //Configure the board
   uartConfig(UART_USB,9600);          													  //This function sets the baud rate and turns on uart interruption
   QeueMayusculizador =xQueueCreate(1, sizeof(AsciFrame));								  //Create a queue and assign to QeueMayusculizador handler
   QeueMinusculizador =xQueueCreate(1, sizeof(AsciFrame));								  //Create a queue and assign to QeueMinusculizador handler
   MsgHandle =xQueueCreate(1, sizeof(AsciFrame));										  // Create a queue and assign to MsgHandle to it
   MsgHandle_2=xQueueCreate(1, sizeof(AsciFrame));
   DataProcessed_handle =xQueueCreate(1, sizeof(AsciFrame));							  // Create a queue and assign DataProcessed_handle to it
   DataProcessed_key=xSemaphoreCreateMutex();											  // Create a mutex and assign  DataProcessed_key to it
   uartInterrupt(UART_USB, true);														  //Enable USB interrupt
   uartCallbackSet(UART_USB, UART_RECEIVE, Port_Interrupt, NULL);                         //Specify the task and when interrupt happens and the interrupt type
#ifdef DRIVER_1
   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	  // Create Driver task
   xTaskCreate(
      driver,                     														  // The name of the function to be executed when the task is called
      (const char *)"driver",     														  // The name of the task given by the user
      configMINIMAL_STACK_SIZE*2, 														  // The amount of stack assigned for this task
      0,                          														  // Task parameters
      tskIDLE_PRIORITY+2,         														  // Task priority
      DrivHandle                 														  // Pointer to the task handler
   );
#endif DRIVER_1


#ifdef SERVER_1
   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	 // Create the Server task
   xTaskCreate(
      server,                     													     // The name of the function to be executed when task is called
      (const char *)"server",     	 													 // The name of the task given by the user
      configMINIMAL_STACK_SIZE*2, 	 													 // The amount of stack assigned for this task
      0,                          														 // Task parameters
      tskIDLE_PRIORITY+1,         														 // Task priority
      0                 																 // Pointer to the task handler
   );

#endif SERVER_1

#ifdef TASK1_1
   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	 // Create Task1
   xTaskCreate(
      task1,                     														 // The name of the function to be executed when task is called
      (const char *)"task1",     														 // The name of the task given by the user
      configMINIMAL_STACK_SIZE*2, 													     // The amount of stack assigned to this task
      0,                          														 // Task parameters
      tskIDLE_PRIORITY+1,        														 // Task priority
      Task1Handle                  														 // Pointers to the task handler
   );
#endif

#ifdef TASK2_1
   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	  // Create Task2
   xTaskCreate(
      task2,                     														 // The name of the function to be executed when task is called
      (const char *)"task2",     														 // The name of the task given
      configMINIMAL_STACK_SIZE*2, 														 // The amount of stack assigned to this task
      0,                          													     // Task parameters
      tskIDLE_PRIORITY+1,         														 // Task priority
      Task2Handle                  														 // Pointer to the task handler
   );
 #endif


   vTaskStartScheduler();

}

/*==================[Internal function definition]=====================*/

/*==================[External function definition]=====================*/


/*==================[End of file]========================================*/
