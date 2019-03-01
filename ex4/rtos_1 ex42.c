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
OS_EVENT     *DispSem, *CommMBox;
char string[100];
int countA=0, countB=0;
void *msg;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data);        /* Function prototype of Startup task           */
void Random_Print(void *data);    /* Function prototypes of Random_Print Task     */
void function_A(void *data);
void function_B(void *data);
void increment(char);
char *complete_string(char *, int *, char *);
void Print_to_Screen(INT8U *);


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
	DispSem = OSSemCreate(1);
	CommMBox = OSMboxCreate((void *)0);
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
    INT8U symbol, err;
	char strA[]="Hi, task A here, task ";
	char strB[]="Hi, task B here, task  ";
	int def_value = 0;

    pdata = pdata;                                         /* Prevent compiler warning                 */
    symbol = 'X';
	err = OSMboxPost(CommMBox, (void *)&def_value);
    //OSTaskCreate(Random_Print, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(function_A, (void *)strA, (void *)&TaskStk[0][TASK_STK_SIZE-1], 1);
	OSTaskCreate(function_B, (void *)strB, (void *)&TaskStk[0][TASK_STK_SIZE-1], 2);
	
    for (;;) {
		//OSSemPend(DispSem, 0, &err);
        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                exit(0);  	                           /* End program                              */
            }
        }
		//err = OSSemPost(DispSem);
        OSTimeDlyHMSM(0, 0, 0, 500);                         /* Wait one second                          */
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void function_A(void *data){
	char *string;
	INT8U err;
	
	for(;;){
		msg = OSMboxPend(CommMBox, 0, &err);
		increment('A');
		string = complete_string((char *)data, (int *)msg, "B");
		Print_to_Screen(string);
		err = OSSemPost(DispSem);
	}
}

void function_B(void *data){
	char *string;
	INT8U err;
	
	for(;;){
		msg = OSMboxPend(CommMBox, 0, &err);
		increment('B');
		string = complete_string((char *)data, (int *)msg, "A");
		Print_to_Screen(string);
		err = OSSemPost(DispSem);
	}
}


void Random_Print(void *data)
{
    INT8U  x,y;
    //INT8U  err;
	INT8U count=0;
#ifdef __WIN32__
    srand(GetCurrentThreadId());
#endif
#ifdef __LINUX__
    srand(getppid());
#endif
    for (;;) {
        x = rand()%80;                        /* Find X position where task number will appear      */
        y = rand()%25;                        /* Find Y position where task number will appear      */

        /* Display the task number on the screen              */
        PC_DispChar(x, y, *((char *)data), DISP_FGND_BLACK + DISP_BGND_GRAY);
		
        OSTimeDlyHMSM(0, 0, 1, 0);                            /* Delay 1 second                     */
		//PC_DispClrScr(DISP_BGND_BLACK);
		if (++count == 10){
			//err = OSSemPost(DispSem);
			OSTaskDel(OS_PRIO_SELF);
		}
		//OSTimeDly(1);
    }
} /*function code*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                      NON-TASK FUNCTIONS
*********************************************************************************************************
*/
void increment(char task){
	INT8U err;
	
	OSSemPend(DispSem, 0, &err);
	if(task=='A') {
		countA++;
		err = OSMboxPost(CommMBox, (void *)&countA);
	}
	else if(task == 'B'){
		countB++;
		err = OSMboxPost(CommMBox, (void *)&countB);
	}
}

char *complete_string(char *data, int *value, char *task){
	char *string = (char *)calloc(1000,sizeof(char));
	char times[100];
	
	_itoa_s((*value), times,sizeof(string), 10);
	strcat(string, data);
	strcat(string, task);
	strcat(string, " ran ");
	strcat(string, times);
	strcat(string, " times.");
	return string;
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
 INT8U delay;
INT8U symbols[80]= {' '};
delay = -1;

//OSSemPend(RandomSem, 0, &err); /*  Acquire semaphore for random number generator numbers      */
while(delay < 1 || delay >3)
	delay = rand();
//OSSemPost(RandomSem);

PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); 
OSTimeDlyHMSM(0, 0, delay, 0);
PC_DispStr(33, 15, text_ptr, DISP_FGND_WHITE);
OSTimeDlyHMSM(0, 0, delay, 0);
PC_DispStr(33, 0, symbols, DISP_FGND_GREEN);



}/*function*/