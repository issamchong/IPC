/*============================================================================
 * License: BSD-3-Clause
 * Copyright 2018, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * Date: 2018/10/04
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "operations.h"  // <= own header
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


int UperCase(char *data){

	strcpy(data,strupr(data));   							 				// convert to lower case
}
int LwrCase(char *data){

	strcpy(data,strlwr(data));   			 								// convert to lower case
	return 1;
}



/*==================[end of file]============================================*/
