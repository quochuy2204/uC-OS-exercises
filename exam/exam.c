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
*		Examination file
* *****************************************************************************************************
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
char          TaskData[N_TASKS];  
OS_EVENT     *RandomSem;



/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void print_values(INT8U num1, INT8U num2, INT8U num3);
void  TaskStart (void *pdata);
void task1(void *data);
void task2(void *data);
void task3(void *data);
void task4(void *data);
INT8U num1;
INT8U num2;
INT8U num3;

/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */
	RandomSem = OSSemCreate(1);
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart();                                             /* Start multitasking                       */

    return 0;
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
	INT16S key;
	
	
	 INT8U symbol;
    pdata = pdata;                                         /* Prevent compiler warning                 */
   

    //OSTaskCreate(Random_Print, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(task1, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE-1], 1);
	OSTaskCreate(task2, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE-1], 2);
	OSTaskCreate(task3, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE-1], 3);
	OSTaskCreate(task4, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE-1], 4);

	
	
   
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
void task1(void *data){
	
	INT8U err;
#ifdef __WIN32__
    srand(GetCurrentThreadId());
#endif
#ifdef __LINUX__
    srand(getppid());
#endif	
	OSSemPend(RandomSem, 0, &err);
	num1=rand()%12+1 ;	
	
	INT8U number1[100];
	INT8U name1[] = "Task 1";
		for(;;){
		PC_DispStr(33, 15, name1, DISP_FGND_WHITE);
		sprintf(number1, "%d", num1);
		PC_DispStr(34, 17, number1, DISP_FGND_WHITE);	
		OSTimeDlyHMSM(0, 0, 1, 0);  
		err=OSSemPost(RandomSem);
		}
	OSTaskDel(1);
}
void task2(void *data){
	
	INT8U err;
#ifdef __WIN32__
    srand(GetCurrentThreadId());
#endif
#ifdef __LINUX__
    srand(getppid());
#endif
	OSSemPend(RandomSem, 0, &err);
	num2=rand()%12+1;	
	INT8U number2[100];
	INT8U name2[] = "Task 2";
	for(;;){
		PC_DispStr(40, 15, name2, DISP_FGND_WHITE); 
		sprintf(number2, "%d", num2);
		PC_DispStr(41, 17, number2, DISP_FGND_WHITE);
		OSTimeDlyHMSM(0, 0, 1, 0);  
		err=OSSemPost(RandomSem);
	}
	OSTaskDel(1);
}

void task3(void *data){
	
	INT8U err;
#ifdef __WIN32__
    srand(GetCurrentThreadId());
#endif
#ifdef __LINUX__
    srand(getppid());
#endif
	OSSemPend(RandomSem, 0, &err);
	num3=rand()%12+1;	
	INT8U number3[100];
	INT8U name3[] = "Task 3";
	for(;;){
		PC_DispStr(47, 15, name3, DISP_FGND_WHITE);
		sprintf(number3, "%d", num3);
		PC_DispStr(48, 17, number3, DISP_FGND_WHITE);
		OSTimeDlyHMSM(0, 0, 1, 0);  
		err=OSSemPost(RandomSem);
	}
	OSTaskDel(1);
}
void task4(void *data){
	INT8U err;
	OSSemPend(RandomSem, 0, &err);	
	if((num1>=num2)&&(num1>=num3)){
		INT8U string[100] = "And the winner is:  Task 1";
		
		for(;;){
			PC_DispStr(48, 20, string, DISP_FGND_WHITE);
			OSTimeDlyHMSM(0, 0, 2, 0);  
			err=OSSemPost(RandomSem);
		
		}
	}
	else if((num2>=num1)&&(num2>=num3)){
	INT8U string2[100] = "And the winner is:  Task 2";
	
		for(;;){
			PC_DispStr(48, 20, string2, DISP_FGND_WHITE);
			OSTimeDlyHMSM(0, 0, 2, 0);  
			err=OSSemPost(RandomSem);
		
		}
	}
	else if((num3>=num1)&&(num3>=num2)){
	INT8U string3[100] = "And the winner is:  Task 3";
	
	
		for(;;){
			PC_DispStr(33, 20, string3, DISP_FGND_WHITE);
			OSTimeDlyHMSM(0, 0, 2, 0);  
			err=OSSemPost(RandomSem);
			
		}
	}
	
	OSTaskDel(1);
}

/*
*********************************************************************************************************
*                                      NON-TASK FUNCTIONS 
*********************************************************************************************************
*/

void print_values(INT8U num1, INT8U num2, INT8U num3)
{
INT8U number[10];
INT8U name1[] = "Task 1";
INT8U name2[] = "Task 2";
INT8U name3[] = "Task 3";

		PC_DispStr(33, 15, name1, DISP_FGND_WHITE);
		sprintf(number, "%d", num1);
		PC_DispStr(34, 17, number, DISP_FGND_WHITE);

		PC_DispStr(40, 15, name2, DISP_FGND_WHITE); 
		sprintf(number, "%d", num2);
		PC_DispStr(41, 17, number, DISP_FGND_WHITE);

		PC_DispStr(47, 15, name3, DISP_FGND_WHITE);
		sprintf(number, "%d", num3);
		PC_DispStr(48, 17, number, DISP_FGND_WHITE);

}

