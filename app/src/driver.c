/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2019, Issam Almustafa <Lssam.almustafa@gmail.com>
 * All rights reserved.
 * Date: 2019/07/25
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "driver.h"  // <= own header


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
QueueHandle_t handle;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint8_t *msg[99];
uint8_t v;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


//This function copies the data portion of the the message buffer to data  and casts data type to read-only on MsgBuffer inside this block only
int GetData(char *data, const char* buffer,uint8_t size ){

	memcpy(data,buffer+1,size-2);
	return 1;
}

// This functions sets the baud rate of the USB UART
void uart_config(int baud,bool_t state )
{
     uartConfig(UART_USB,baud);
	 //uartInterrupt( UART_USB, state); 				// Enable UART interrupt
}


int Send2Qu(QueueHandle_t *handler,const  char* Msg_Only,const char* Msg_Op){

	strcat(Msg_Op,Msg_Only);
	printf("Message with operation flag   is %s\r\n",Msg_Op);
	return 1;

}
int ASCI(char *frame, uint8_t  size, char *buf){

char buffer[111]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";			//This buffer stores the data received in Hex representation
int newval, j=0;
char msg[60]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";								//This is where the final ASCCI readable letters are stored

    for (int i = 0; i < size; i+=2)
    {
        int firstvalue = frame[i] - '0';
        int secondvalue;

            switch(frame[i+1])																				//if RecvData[i+1] is a letter convert it to integer, otherwise use it.
        {
            case 'A':
            {
                secondvalue = 10;
            }break;
            case 'B':
            {
                secondvalue = 11;
            }break;
            case 'C':
            {
                secondvalue = 12;
            }break;
            case 'D':
            {
                secondvalue = 13;
            }break;

            case 'E':
            {
                secondvalue = 14;
            }break;
            case 'F':
            {
                secondvalue = 15;
            }break;
            default:
                secondvalue = frame[i+1] - '0';
            break;
        }

        newval =  16 * firstvalue + secondvalue;						 									//convert the two values into decimal form
        buffer[i]=(char)newval;																				// cast type newval into character, save in ptrBuffer :buffer[0]='G' only for the first element

    }
	for(int j=0;j<=sizeof(msg);j++){																		//Assign new value for every element in msg array from the converted buffer
		msg[j]=buffer[j*2];
	 }
	strcpy(buf,msg);																						//Copy the converted result into the buffer
    return 0;
}





/*==================[end of file]============================================*/
