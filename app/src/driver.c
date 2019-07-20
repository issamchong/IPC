/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
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
int GetMsg(char *data, const char* buffer,uint8_t size ){

	memcpy(data,buffer+2,size-2);
}

// This functions sets the baud rate of the USB UART
void uart_config(int baud,bool_t state )
{
     uartConfig(UART_USB,baud);
	 //uartInterrupt( UART_USB, state); 				// Enable UART interrupt
}

//This function extracts the operation byte from the frame and saves it into a operation buffer
int GetOp(char *op, const char* buffer ){

	op[0]=buffer[1];
}
int Send2Qu(QueueHandle_t *handler,const  char* Msg_Only,const char* Msg_Op){

	strcat(Msg_Op,Msg_Only);
	printf("Message with operation flag   is %s\r\n",Msg_Op);
	//xQueueSend(,MsgBuffer,50)
}
int ASCI(char *frame, uint8_t  size, char *buf){

char buffer[111]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
int newval, j=0;
char msg[60]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    for (int i = 0; i < size; i+=2)
    {
        int firstvalue = frame[i] - '0';
        int secondvalue;
        //if RecvData[i+1] is a letter convert it to integer, otherwise use it.
            switch(frame[i+1])
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
        //convert the two values into decimal form
        newval =  16 * firstvalue + secondvalue;
        //change newval into a character
          // cast type newval into character, save itin ptrBuffer :buffer[0]='G' only for the first element
	buffer[i]=(char)newval;
    }
	for(int j=0;j<=sizeof(msg);j++){
		msg[j]=buffer[j*2];
	 }
	strcpy(buf,msg);
    return 0;
}





/*==================[end of file]============================================*/
