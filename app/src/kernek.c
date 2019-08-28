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
#define TASK1_1												//TUrn off Task1 by setting Task1_0
#define TASK2_1												//Turn off Task2 by setting Task2_0
#define SERVER_1											//Turn off Server by setting Server_0
#define DRIVER_1											//Turn off Driver by setting Driver_0
/*==================[Definition of internal data]=========================*/
static char frame[105]; 									//The Frame will be stored in this buffer
volatile uint8_t InterruptCounter=0;						//This variable stores the interrupt count
Token_pt PerfPt;											// PerFPT is a pointer of type TOken_pt that can only points at Token type of structure
char getlength[sizeof(frame)];								// This buffer stores the frame length

/*==================[Declaring  task handlers]=========================*/

volatile TaskHandle_t ServHandle = NULL;					//This is the Server handler declaration
volatile TaskHandle_t DrivHandle = NULL;					//This is the Driver handler declaration
volatile TaskHandle_t Task1Handle = NULL;					//This is Task1 handler declaration
volatile TaskHandle_t Task2Handle = NULL;					//This is Task2 handler declaration
volatile TaskHandle_t Task3Handle=NULL;						//This is Task4 handler declaration

/*==================[Declaring  Queues]=========================*/

volatile QueueHandle_t QeueMayusculizador;					//Declaring queue handler  for Task1
volatile QueueHandle_t QeueMinusculizador;					//Declaring queue handler for Task2
volatile QueueHandle_t PackageProcessed;					//Declaring queue handler for Queue transmitter
volatile QueueHandle_t queueTransmision;					//Declaring queue handler for processed data
volatile QueueHandle_t QueueMedirPerformance;				//Declaring queue handler for Task3 to send the token

SemaphoreHandle_t xSemaphoreStartDriver = NULL;				//This semaphore is used to trigger the Driver task from ISR
SemaphoreHandle_t xSemaphoreCompleteDriver=NULL;			//This semaphore prevents the Driver from being intervened  during processing the package even when an an interrupt occurs
/*==================[Declaring  structures ]=========================*/
static struct Frame message;								//structure of type Frame to store the package in different fields
static struct MedirPerformance performance;					//This structure is used to store different fields needed to measure performance during the program execution

/*==================[Definition of external data]=========================*/
/*==================[Declaration of internal functions ]====================*/


void Port_Interrupt(void){															//Interrupt handler should be fast and simple lines

	   char c = uartRxRead( UART_USB );												//Save the byte read in this buffer
	   frame[InterruptCounter]=c;											  	    //save character in buffer
	   InterruptCounter++;															//increment the number of interrupts
	   if(InterruptCounter==1){														//Increment
		   xSemaphoreGiveFromISR(xSemaphoreStartDriver,NULL);		   				//Avoid Polling and CPU usage by reactivate task Driver only when an interrupt occurs 															//Notify Driver to start reading after 4 seconds from this moment
	   }

}




void server(void){																	// The server assigns the messages to the correct task based on the operation byte, it also creates queues for each task

																					// The current stack size available is stored in the ServerStartStack of  Report structure and cast type it to read only
	char MsgFromDriver[sizeof(frame)]="\0";											// Declaring variable that will store the data from the driver, this variable of size AssciFrame define above
	char *MsgToDriver=(char *)pvPortMalloc(sizeof(frame));							// Declaring variable that will store the data to be sent to the driver, this variable of size AssciFrame define above
	char *f=MsgFromDriver;															// This pointer holds the first operation byte in order to determine the proper operation
	char SizeData[3]="\0";															// This string is used to store the size bytes of the data

	while(1){

    	if(!(xQueueReceive(PackageProcessed,MsgFromDriver,(TickType_t)portMAX_DELAY))){    // Check if any message was received and store it in MsgFromDriver buffer, important QueueReceive clears buffer  when called again
    		puts("Server <-Driver: No received\n");					   						// Error capture if message was not received
    	}else{
    		fsmMesurePerformance(PerfPt,MsgFromDriver,sizeof(frame));					   //update state and set  R1.4 and R5.1
    		SizeData[0]=MsgFromDriver[1];												   // Save the first byte of the size field in first  element of this buffer
    		SizeData[1]=MsgFromDriver[2];												   //Save the second byte of size field in this second element  of this buffer
    		message.size= atoi(SizeData);												   // Convert the Size string to integer and assign it to the size entry of the message structure
    		message.operation=	atoi(*f);												   // Convert the operation to integer and assign to the operation entry of the message structure
    		strcpy(message.data,MsgFromDriver+3);										   // Copy the data portion only to the data entry of the message structure
    		if(!(message.size==strlen(message.data))){										//Check if data length is correct
    			*f='\0';																	//set operation to empty so no operation is executed and switch to the proper case
    			}
    		switch(*f)
    		{
    		case '0':
    			printf("\nServer: Operation  %c\n",*f);													//Display  the operation number
    			if(!(xQueueSend(QeueMayusculizador,message.data,50))){									//For 50 ms keep Sending data to QeueMinusculizador
    				puts("Server-> Task1: No sent\n");													//Display error if was not successful
    				}else{
    					if(!xQueueReceive(queueTransmision,message.dataProcessed,(TickType_t)portMAX_DELAY)){			//Try reading for ever the processed data and save it in message.DataProcessed field of message structure
    						puts("Server <- Task1: No received\n");														// Error report of nothing  received
    						}else{																						//Server is putting all back together to send processed data back to Driver
    							MsgToDriver[1]=*f;														//Get operation number and save it in f
    							strcpy(MsgToDriver+2,SizeData);											// Add the data size
    							strcpy(MsgToDriver+4,message.dataProcessed);							//Add processed data to buffer
    							if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){				//Send the data to Driver
    								puts("Server -> Driver: No sent\n");								//Display Error if was not successful
    								}else{
    									puts("Server -> Driver: Processed");							//If sent successfully, report  the message that was sent
    									vPortFree(MsgToDriver);											//Clear the heap using heap_2 algorithm
    									bzero(MsgToDriver,strlen(MsgToDriver));							//Clear buffer
    									vTaskDelay(1000);												//Wait 1 second
    									GetStack(MsgToDriver,ServHandle);								//Get current stack size and save it in MsgToDriver
    									if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){		//Send message with stack size
    										puts("Server -> Driver: No sent\n");

    									}else{
    										vPortFree(MsgToDriver);										//Clear the heap using heap_2
    										bzero(MsgToDriver,strlen(MsgToDriver));						//Clear MsgToDriver buffer
    									}
    								}
    						}
    				}
    			break;

    		 case '1':
    			 printf("\nServer: Operation  %c\n",*f);																		//Display the operation number
    			 if(!(xQueueSend(QeueMinusculizador,message.data,50))){															//Keep Sending  data to QeueMinusculizador during 50 ms
    				 uartWriteString(UART_USB,"Server-> Task2: No sent\n");														//Display error if was not possible after 50 passed
    				 }else{
    					 if(!(xQueueReceive(queueTransmision,message.dataProcessed,(TickType_t)portMAX_DELAY))){				//Keep trying to read  from queue for ever  and save it in the message.DataProcessed field when successful
    						 puts("Server <- Task2: No received\n");															// Display error if could not read from queue if time passed and did not read anything
    						 }else{																								//Server is putting all back together to send processed data back to Driver
    							 MsgToDriver[1]=*f;																				//Add operation number
    							 strcpy(MsgToDriver+2,SizeData);																// Add the data size
    							 strcpy(MsgToDriver+4,message.dataProcessed);													//Finally add the message after processing
    							 if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){										//Keep trying to send processed  data to Driver for 50 ms
    								 puts("Server -> Driver: No sent\n");														//Display error if was not successful
    								 }else{
    									puts("Server -> Driver: Processed");													//Display message if successful
    									vPortFree(MsgToDriver);																	//Clear the heap using heap_2
    									bzero(MsgToDriver,strlen(MsgToDriver));													//Clear the buffer
    								    vTaskDelay(1000);																		//Wait 1 second
    									GetStack(MsgToDriver,ServHandle);														//Get stack size of current task and save it in MsgToDriver
    								    if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){								//Keep sending data for 50 ms
    								    	puts("Server -> Driver: No sent\n");												//Display error if not successful
    								    }else{
    								    	vPortFree(MsgToDriver);									   							//Clear the heap using heap_2
    								    	bzero(MsgToDriver,strlen(MsgToDriver));												//Clear buffer
    								    }

    								 }
    						 }
    				 }
    			 break;

    		 case '4':
    			 printf("Server: Operation  %c\n",*f);																		// Display operation number
    			 fsmMesurePerformance(PerfPt,NULL,NULL);																	//update state and set times R4.3:
    			 if(!(xQueueSend(QueueMedirPerformance,message.data,50))){													//Keep trying to Send data to QeueMinusculizador  for 50 ms
    				 puts("Server-> Task4: No sent\n");																		//Error report if not sent
    				 }else{
    					 vTaskDelay(100);																					//TIme of salida depends on this, if no delay it would be zero
    					 if(!(xQueueReceive(queueTransmision,message.dataProcessed,(TickType_t)portMAX_DELAY))){			//Receive processed data from task2 and save it in the message.DataProcessed entry
    						 puts("Server <- Task4: No received\n");														//Display  Error  if nothing  received
    						 }else{																							//Server is putting all back together to send processed data back to Driver
    							 fsmMesurePerformance(PerfPt,NULL,NULL);													//Requirement 4.5 to update the time of salida
    							 MsgToDriver[1]=*f;
    							 strcpy(MsgToDriver+2,SizeData);															// Add the data size
    							 strcpy(MsgToDriver+4,message.dataProcessed);
    							 strcpy(getlength,MsgToDriver+1);															//Copy from allocated memory to buffer to count number of bytes
    							 if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){									//Send the data to Driver
    								 puts("Server -> Driver: No sent\n");													//Display error if not sent
    								 }else{
    									 puts("Server -> Driver: Processed");                      							//If sent successfully, display successful
    									 vPortFree(MsgToDriver);									 						//Clear the  heap using heap_2 algorithm
    									 bzero(MsgToDriver,strlen(MsgToDriver));											//Clear buffer
    									 if(!(xQueueReceive(QueueMedirPerformance,MsgToDriver,(TickType_t)portMAX_DELAY))){
    										 puts("Server -> Task4: No Token");                      						//Display message if sent successfully
    									 }else{
    										 if(!(xQueueSend(PackageProcessed,(const)MsgToDriver,50))){						//Keep trying to Send the data to Driver
    											 puts("Server -> Driver: No sent\n");										//Display error  if not sent
    											 }
    										 vPortFree(MsgToDriver);									 					//Clear the  heap using heap_2 algorithm
    										 bzero(MsgToDriver,strlen(MsgToDriver));										//Clear buffer
    									 }
    								 }
    						 }
    				 }
    			 break;

    		 case '\0':																		//Come here if data found corrupted
    				if(!(xQueueSend(PackageProcessed," Data corrupted ",50))){				//Send the data to Driver
    					puts("Server -> Driver: No sent\n");								//Error handle if not sent
    					}else{
    						vTaskDelay(1000);												//Wait 1 second
    						if(!(xQueueSend(PackageProcessed,"\0",50))){					//Keep trying to send empty message
    							puts("Server -> Driver: No sent\n");						//Display error if not successful
    						}else{
    							bzero(MsgFromDriver,strlen(MsgFromDriver));					//Clear buffer
    							bzero(f,strlen(f));											//Clear byte
    						}
    					}
    				break;

    		 default:																		//Come here if the operation is not valid
    			 if(!(xQueueSend(PackageProcessed," Invalid Operation ",50))){				//Keep trying to send message "Invalid operation" for 50 ms
    				 puts("Server -> Driver: No sent\n");								    //Error handle if not sent
    				 }else{
    					 vTaskDelay(1000);													//Wait 1 second
    					 if(!(xQueueSend(PackageProcessed,"\0",50))){						//Keep trying to Send empty message for 50 ms
    						 puts("Server -> Driver: No sent\n");							//Display error message
    					 }else{
    						 bzero(MsgFromDriver,strlen(MsgFromDriver));					//Clear buffer
    						 bzero(f,strlen(f));											//Clear byte
    					 }
    				 }
    			 break;
    		}
    	}
	}
}

void driver(void){

	static char data_2Server[sizeof(frame)]="\0";								//Buffer to store data to be sent to Server
	static char data_FromServer[sizeof(frame)]="\0";							//Buffer to store data received from Server
	static char data_2Port[sizeof(frame)]="\0";									//Buffer to store data received from Server
	char op[2]="\0";															// String to store the operation flag
	char SOF='{';																// This  variable holds the Start Of Frame (SOF) to validate the frame
	char EnOF='}';																//This variable holds the End Of Frame (EOF) used to validate the frame as well


	while(1){

			if(xSemaphoreTake( xSemaphoreStartDriver, (TickType_t)portMAX_DELAY )){			//Keep trying to take key for ever, until ISR  has released it
				vTaskDelay(2000);															//Wait 2 seconds
				xSemaphoreTake(xSemaphoreCompleteDriver,(TickType_t)portMAX_DELAY);			//Keep trying to take mutex key to complete processing the package, if a previous package is being processed it will not continue
				if(!((frame[0]==SOF) && (frame[strlen(frame)-1]==EnOF))){					// Verify if either start of frame or end of frame is not valid
					puts("\n");
					puts("Driver: Invalid Frame");											//Display error if frame does not have "{" or "}"
					}else{
						fsmMesurePerformance(PerfPt,NULL,NULL);								//update state and set times R1.1,R4.1:,R4.2
						puts("\n");
						puts("Driver: Valid Frame ");										//Display  valid if frame has starting and ending keys
						if(!GetData(data_2Server,frame,strlen(frame))){						//GetData function removes the start and ending keys from the frame before sending to Server
							puts("Driver: GetData: Failed");								// Error handle if GetData function did not work
						}else{
							if(!(xQueueSend(PackageProcessed,data_2Server,50))){			//Sending data to Server via PackageProcessed queue
								puts("Driver-> Server: No sent\n");							// Error handle if message was not sent
							}else{
								vTaskDelay(1000);
								if(!(xQueueReceive(PackageProcessed,data_FromServer,(TickType_t)portMAX_DELAY))){				//Keep attempting to read from queue for ever and save in buffer if successful
									puts("Driver: Nothing received");															//Display error if not successful
								}else{
									data_FromServer[0]='{';																		//Set first element to be the start of frame
									strcat(data_FromServer,"}");																//Add end of frame to the end of the received data
									//puts("Driver <- Server: Received data");
									puts(data_FromServer);																		//Display final processed package to be sent back
									bzero(data_FromServer,strlen(data_FromServer));												//Clear buffer
									if(!(xQueueReceive(PackageProcessed,data_FromServer,(TickType_t)portMAX_DELAY))){			 //Keeo trying to read from queue the the second message or possible token for ever until it does
										puts("Driver <- Server: No Token");														 //Display error
									}else{
										//("Driver <- Server: Received Token");
										puts(data_FromServer);											//Display  received token or message
										bzero(data_FromServer,strlen(data_FromServer));					//Clear buffer
										fsmMesurePerformance(PerfPt,NULL,NULL);								//Important performance->estado must be set to zero in order for the parameters values to be captured correctly at their corresponding times
										//printf("state is %d\n",PerfPt-> estado);
										}
								}
							}
						}
					}																//Come here directly if it was false and after finishing process if true
				vTaskDelay(6000);													// Wait 8 seconds before clearing the screen
				xSemaphoreGive(xSemaphoreCompleteDriver);							//Release mutex semaphore to allow next cycle to be completed
				InterruptCounter=0;													//Reset counter
				ClearScreen();														//Clear screen
				puts("Driver: Ready for new package...");							//Display message

			}else{
				vTaskDelay(100);
			}
	}
}

																													//This task converts the message letters to upper case
void task1(void){

	static char Task1Buffer[sizeof(frame)]="\0";						    										//Declaring local buffer to store data to be  sent and received

		while(1){
			if(QeueMayusculizador !=0){													 	 					   //Verify if QeueMayusculizador was created
				while(1){
				if(!(xQueueReceive(QeueMayusculizador,Task1Buffer,(TickType_t)portMAX_DELAY))){				 	   // Check if anything was received from the queue
					//uartWriteString(UART_USB," Task1 <- Server : No received\n");		     					   //Error handle if not
				}else
					if(!UperCase(Task1Buffer)){											 	 					   //Send message to UpperCase function to set letters to capital
						puts("Task1: No lower case\n");	  	 					   									//Error handle if message was not sent
					}else{
						if(!(xQueueSend(queueTransmision,Task1Buffer,50))){		  	  								//Send processed data if all good
							puts("Task1-> Server: No sent \n");	  	  												// Display error
							}
						vTaskDelay(1000);
						}
				}


			}else{
				EndTask(&Task1Handle,1);														  	 //End task if no handle was never created
			}

		}
}


void taskMedirPerformance(void){

	char *Task4Buffer=(char *)pvPortMalloc(sizeof(frame));												// Declaring variable that will store the data to be sent to the driver, this variable of size AssciFrame define above
	while(1){
		if(QueueMedirPerformance !=0){																	//Verify if QeueMayusculizador was created
			if(!(xQueueReceive(QueueMedirPerformance,Task4Buffer,(TickType_t)portMAX_DELAY))){			// Check if anything was received from the queue
				puts(" Task3 <- Server : No received\n");		   					 					//Display error  if not received after long waiting
				}else{
					if(!UperCase(Task4Buffer)){											 	  			//Send message to UpperCase function to set letters to capital
						puts("Task3: No Uppercase");
					}else{
						vTaskDelay(500);													  			//R4.4 time of fin depends on this, if no delay it would be zero
						fsmMesurePerformance(PerfPt,NULL,NULL);											//Requirement 4.5 to update the time of salida
						if(!(xQueueSend(queueTransmision,Task4Buffer,50))){		  	  					//Send processed data if all good
							puts("Task3-> Server: No sent "); 						   					//Display error  if data was not sent
							}else{
								fsmMesurePerformance(PerfPt,NULL,NULL);						   			//Requirement 4.6 to update the time of transmission
								vTaskDelay(1000);														//This waiting time is crucial because it allows  the capture of processing time at different points to be completed
								vPortFree(Task4Buffer);													//Clear dynamic memory
								bzero(Task4Buffer,strlen(Task4Buffer));
								CompileToken(PerfPt,Task4Buffer);
								if(!(xQueueSend(QueueMedirPerformance,Task4Buffer,50))){				//Send Token to Server at QueueMedirPerformance
									puts("Task4:Could not send");
								}else{
									vPortFree(Task4Buffer);											    //Clear dynamic memory
									bzero(Task4Buffer,strlen(Task4Buffer));
									vTaskDelay(500);												   //This delay is crucial, otherwise you would exit without having send() completed
									}
							}
					}
				}
		}else{
			EndTask(&Task3Handle,1);												   					//End task if no handle was never created
			vTaskDelay(3000);																			//Wait 3 seconds
		}
	}
}


//This function converts the message to lower case
void task2(void){

	static char Task2Buffer[sizeof(frame)]="\0";										 			//Declaring local buffer to store data to be  sent and received

	while(1){
		if(QeueMinusculizador !=0){														 				//Verify if QeueMinusculizador was created
			if(!(xQueueReceive(QeueMinusculizador,Task2Buffer,(TickType_t)portMAX_DELAY))){				// Check if anything was received from the queue
				puts(" Task2 <- Server : No received\n");								 				//Error handle if not
				}else{
					if(!LwrCase(Task2Buffer)){  											 			//convert message letters to lower case and check if if it was successful
						puts("Task2: No lower case\n");		 											//Error handle if conversion did not happen
						}else{
							if(!(xQueueSend(queueTransmision,Task2Buffer,50))){			 				//Check if message was sent
								puts("Task2-> Server: No sent \n");										//Error handle if message was not sent
								}
							vTaskDelay(1000);
						}
				}
		}else{
			EndTask(&Task2Handle,2);													//End task if handler was never created
			}
	}
}
/*==================[External function declaration ]====================*/

/*==================[Principal function ]======================================*/
																					   // Principal function that runs after reseting or starting up
int main(void)
{
   PerfPt=&performance;
   PerfPt->estado=0;
   PerfPt->id_of_package=0;

   boardConfig();																		//Configure the board
   uartConfig(UART_USB,9600);          													//This function sets the baud rate and turns on uart interruption
   uartInterrupt(UART_USB, true);														//Enable USB interrupt
   uartCallbackSet(UART_USB, UART_RECEIVE, Port_Interrupt, NULL);                       //Specify the task and when interrupt happens and the interrupt type
   ClearScreen();

   puts("\n");
   GetHeap(frame);																	   //This function sends the operation indicated in the second argument
   puts("Welcome to RTOSII IPC,send a package....\n");									//Display welcome message

   QeueMayusculizador =xQueueCreate(1, sizeof(frame));								    //This queue is used to send data to Task1
   QeueMinusculizador =xQueueCreate(1, sizeof(frame));								    //This queue is used to send data to Task2
   PackageProcessed =xQueueCreate(1, sizeof(frame));									//This queue is used to send and receive data from/to Driver
   queueTransmision=xQueueCreate(1, sizeof(frame));										//This queue is used to receive processed data from any task
   QueueMedirPerformance=xQueueCreate(1, sizeof(frame));								//This queue is used to receive measured performance only from  task3

   xSemaphoreStartDriver = xSemaphoreCreateBinary();
   xSemaphoreCompleteDriver = xSemaphoreCreateMutex();

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
	  ServHandle                 														 // Pointer to the task handler
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

   xTaskCreate(
		taskMedirPerformance,                     										 	// The name of the function to be executed when task is called
        (const char *)"taskMedirPerformance",      											 // The name of the task given
        configMINIMAL_STACK_SIZE*2, 														 // The amount of stack assigned to this task
        0,                          													     // Task parameters
        tskIDLE_PRIORITY+1,         														 // Task priority
		Task3Handle                  														 // Pointer to the task handler
     );


   vTaskStartScheduler();

}

/*==================[Internal function definition]=====================*/

/*==================[External function definition]=====================*/


/*==================[End of file]========================================*/
