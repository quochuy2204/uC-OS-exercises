/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*					WIN32 PORT & LINUX PORT
*                          (c) Copyright 2004, Werner.Zimmermann@fht-esslingen.de
*                 (Similar to Example 1 of the 80x86 Real Mode port by Jean J. Labrosse)
*                                           All Rights Reserved
** *****************************************************************************************************
*		Further modified by 	mikael.jakas@puv.fi & 
* 					Jukka.matila@vamk.fi
* *****************************************************************************************************
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        2       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT     *RandomSem;
OS_EVENT     *newSem;


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data);        /* Function prototype of Startup task           */
void function_A_B(void *data);

void Print_to_Screen(INT8U *text_ptr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int  main(void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */
	RandomSem = OSSemCreate(1);
	newSem = OSSemCreate(1);
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
    return 0;
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart(void *pdata)
{
    INT16S key;
    INT8U *strA="Hi, task A here";
	INT8U *strB="Hi, task B here";

    pdata = pdata;                                         /* Prevent compiler warning                 */
    

    OSTaskCreate(function_A_B, (void *)strA, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(function_A_B, (void *)strB, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);

    for (;;) {

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                exit(0);  	                           /* End program                              */
            }
        }

        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/
void function_A_B(void *data){
	INT8U err;
	
#ifdef __WIN32__
    srand(GetCurrentThreadId());
#endif
#ifdef __LINUX__
    srand(getppid());
#endif
	 for (;;) {

	
	
		OSSemPend(newSem, 0, &err);
		Print_to_Screen(((char *)data));
		OSSemPost(newSem);
	//	OSTimeDlyHMSM(0, 0, 1, 0);								
		

                                /* Delay 1 clock tick                                 */
    }
}



/********************************************************************/
/*Resource function for MicroC OS -II exercises						*/
/*																	*/
/* The function prints a text string given by *text_ptr to a 		*/
/* fixed position to the screen. A random delay is included 		*/
/* in order to simulate an external device 							*/
/* The function shall be used as a non-reentrant resource			*/
/*******************************************************************/


void Print_to_Screen(INT8U *text_ptr)
{
 INT8U err;
 INT8U delay;
INT8U symbols[80]= {' '};
delay = -1;

OSSemPend(RandomSem, 0, &err); /*  Acquire semaphore for random number generator numbers      */
while(delay < 1 || delay >3)
	delay = rand();
OSSemPost(RandomSem);

PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); 
OSTimeDlyHMSM(0, 0, delay, 0);
PC_DispStr(33, 15, text_ptr, DISP_FGND_WHITE);
OSTimeDlyHMSM(0, 0, delay, 0);
PC_DispStr(33, 0, symbols, DISP_FGND_GREEN);



}/*function*/




/*$PAGE*/
/*
*********************************************************************************************************
*                                      NON-TASK FUNCTIONS
*********************************************************************************************************
*/
