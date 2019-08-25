/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2019, Issam Almustafa <Lssam.almustafa@gmail.com>
 * All rights reserved.
 * Date: 2019/07/25
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "service.h"  // <= own header

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
	volatile uint16_t  time1=0;
	volatile uint16_t  time2=0;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int CompileToken(Token_pt pt,char *msg){

	//sprintf(msg,"\0{5ID%dPLD%dT0%dT1%dT2%dT3%dT4%dT5%dLEN%dMEM%d}",pt->id_of_package,pt->payload,
			//	pt->tiempo_de_llegada,pt->tiempo_de_recepcion,pt->tiempo_de_inicio,pt->tiempo_de_fin,pt->tiempo_de_salida,
				//pt->tiempo_de_transmision,pt->package_length,pt->alocated_memory);

	char id[10];
	char pload[20];
	char Tllegar[10];
	char Trecp[10];
	char Tinicio[10];
	char Tfin[10];
	char Tsalida[10];
	char Ttrans[10];
	char len[10];
	char mem[10];

    itoa(pt->id_of_package,id,10);
	itoa(pt->payload,pload,10);
	itoa(pt->tiempo_de_llegada,Tllegar,10);
	itoa(pt->tiempo_de_recepcion,Trecp,10);
	itoa(pt->tiempo_de_inicio,Tinicio,10);
	itoa(pt->tiempo_de_fin,Tfin,10);
	itoa(pt->tiempo_de_salida,Tsalida,10);
	itoa(pt->tiempo_de_transmision,Ttrans,10);
	itoa(pt->package_length,len,10);
	itoa(pt->alocated_memory,mem,10);

	strcat(msg,"\0");
    strcat(msg,"{5ID:");
    strcat(msg,id);

    strcat(msg,"PLoad:");
    strcat(msg,pload);

    strcat(msg,"Tlleg:");
    strcat(msg,Tllegar);

    strcat(msg,"TRec:");
    strcat(msg,Trecp);

    strcat(msg,"TIn:");
    strcat(msg,Tinicio);

    strcat(msg,"TFn:");
    strcat(msg,Tfin);

    strcat(msg,"TSal:");
    strcat(msg,Tsalida);

    strcat(msg,"TTran:");
    strcat(msg,Ttrans);

    strcat(msg,"LEN:");
    strcat(msg,len);

    strcat(msg,"MEM:");
    strcat(msg,mem);

    strcat(msg,"}");


}


int GetMsg(char *data, const char* buffer,uint8_t size ){

	memcpy(data,buffer+2,size-2);
	return 1;
}
int GetOp(char *op, const char* buffer ){

	op[0]=buffer[1];

	return 1;
}
int EndTask(TaskHandle_t *handle,const uint8_t i){										// This function suspends the task, it takes the task handle and the task number, the number to decide the message



      vTaskDelay(500);
      switch (i)																		//Verify the task number
      {
      case 1:
    	  uartWriteString(UART_USB,"Task 1 suspended, no Queue created.  \n");
    	  break;
      case 2:
    	  uartWriteString(UART_USB,"Task 2 suspended, no Queue created.  \n");
    	  break;
      default:
    	  uartWriteString(UART_USB,"No such task.  \n");
      }
      vTaskSuspend(*handle);
	  return 1;
}

int fsmMesurePerformance(Token_pt  t, uint8_t *pload, uint16_t AloMem){



	switch(t->estado)
	{
	case 0:
		t->estado=1;
		t->id_of_package++;
		t->tiempo_de_llegada=time1;
		time2=xTaskGetTickCount();
		t->tiempo_de_recepcion=(uint32_t)(time2-time1);					//Save of receiving last byte
		time1=time2;									   	   	   	   	//Increment the package ID
		time2=0;
		break;
	case 1:
		t->package_length=strlen(pload);
		t->payload=pload;
		t->alocated_memory=AloMem;
		t->estado=2;
		break;

	case 2:
		time2=xTaskGetTickCount();
		t->tiempo_de_inicio=(uint32_t)(time2-time1);					//Update the corresponding time
		time1=time2;
		time2=0;
		t->estado=3;													//R4.3 satisfy requirement 4.3

		break;
	case 3:

		time2=xTaskGetTickCount();
		t->estado=4;												 // R4.4 satisfy requirement 4.4
		t->tiempo_de_fin=(uint32_t)(time2-time1);					 //Update the corresponding time
		time1=time2;
		time2=0;
		//printf("TIME last byte in  is %p\n",t->tiempo_de_fin);

		break;
	case 4:

		time2=xTaskGetTickCount();											//Set time2 to current tick count in ms
		t->estado=5;														// R4.5 Update the FSM state
		t->tiempo_de_salida=(uint32_t)(time2-time1);						//Update the corresponding time
		time1=time2;
		time2=0;
		//printf("TIME last byte in  is %p\n",t->tiempo_de_salida);

		break;
	case 5:																	// R4.6 starts
		time2=xTaskGetTickCount();											//Get current tick count
		t->estado=6;
		t->tiempo_de_transmision=(uint32_t)(time2-time1);					//Update the corresponding time
										//Set package length
		break;

	default:
		t->estado=0;
		break;
	}
	return 0;


}





/*==================[end of file]============================================*/
