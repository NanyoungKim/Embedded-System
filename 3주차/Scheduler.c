/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include <string.h>
#include <time.h>
#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define TASK_STK_SIZE 512 /* Size of each task's stacks (# of WORDs)            */
#define N_TASKS 1         /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; /* Tasks stacks                                  */
OS_STK TaskStartStk[TASK_STK_SIZE];
OS_EVENT *RandomSem;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void Task(void *data);      /* Function prototypes of tasks                  */
void TaskStart(void *data); /* Function prototypes of Startup task           */
static void TaskStartCreateTasks(void);
static void TaskStartDispInit(void);
static void TaskStartDisp(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/
char cmd;
void main(void) {
	printf("Command 1: mandatory, 2: optional\n");
	scanf("%c", &cmd);
	if (cmd != '1' && cmd != '2') exit(1);

	srand(time(NULL));
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); /* Clear the screen                         */

	OSInit(); /* Initialize uC/OS-II                      */

	PC_DOSSaveReturn();        /* Save environment to return to DOS        */
	PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */
	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
	OSStart(); /* Start multitasking                       */
	RandomSem = OSSemCreate(1);		//cfdfdfdsfdsf
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void TaskStart(void *pdata) {
#if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
	OS_CPU_SR cpu_sr;
#endif
	char s[100];
	INT16S key;
	pdata = pdata; /* Prevent compiler warning                 */

	TaskStartDispInit(); /* Initialize the display                   */

	OS_ENTER_CRITICAL();
	PC_VectSet(0x08, OSTickISR);      /* Install uC/OS-II's clock tick ISR        */
	PC_SetTickRate(OS_TICKS_PER_SEC); /* Reprogram tick rate                      */
	OS_EXIT_CRITICAL();

	OSStatInit(); /* Initialize uC/OS-II's statistics         */

	TaskStartCreateTasks(); /* Create all the application tasks         */

	for (;;) {
		TaskStartDisp(); /* Update the display                       */

		if (PC_GetKey(&key) == TRUE) { /* See if key has been pressed              */
			if (key == 0x1B) {         /* Yes, see if it's the ESCAPE key          */
				PC_DOSReturn();        /* Return to DOS                            */
			}
		}

		OSCtxSwCtr = 0;            /* Clear context switch counter             */
		OSTimeDlyHMSM(0, 0, 1, 0); /* Wait one second                          */
	}
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static void TaskStartDispInit(void) {
	/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
	/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
	PC_DispStr(0, 0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
	PC_DispStr(0, 1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 3, "                                    EXAMPLE #1                                  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
	/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
	/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static void TaskStartDisp(void) {
	char s[80];

	sprintf(s, "%5d", OSTaskCtr); /* Display #tasks running               */
	PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
	sprintf(s, "%3d", OSCPUUsage); /* Display CPU usage in %               */
	PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

	sprintf(s, "%5d", OSCtxSwCtr); /* Display #context switches per second */
	PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */
	PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	switch (_8087) { /* Display whether FPU present          */
	case 0:
		PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 1:
		PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 2:
		PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 3:
		PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;
	}
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static void TaskStartCreateTasks(void) {
	OSTaskCreate(Task, (void *)&cmd, &TaskStk[0][TASK_STK_SIZE - 1], 1);
}

// Define maximum random number and tables
INT8U const myMapTbl[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
INT8U const myUnMapTbl[] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x00 to 0x0F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x10 to 0x1F                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x20 to 0x2F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x30 to 0x3F                             */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x40 to 0x4F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x50 to 0x5F                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x60 to 0x6F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x70 to 0x7F                             */
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x80 to 0x8F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x90 to 0x9F                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xA0 to 0xAF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xB0 to 0xBF                             */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xC0 to 0xCF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xD0 to 0xDF                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xE0 to 0xEF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0  /* 0xF0 to 0xFF                             */
};

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void Task(void *pdata) {
	// Variable declaration section
	INT8U myRdyTbl[8];
	INT8U myRdyGrp;
	INT8U final;
	INT8U endCount = 0;
	INT8U dispX = 0;
	INT8U dispY = 5;



	INT8U i;
	INT8U err;
	INT8U number[4];
	INT8U minValue;
	INT8U j;
	INT8U check = 0;
	// For compiler warning OR Command 1: mandatory, 2: optional
	char mode = *(char *)pdata;
	// Your code would be here




	final = 64;
	while (endCount < 4) {

		myRdyGrp = 0;		//초기화
		for (i = 0; i < 8; i++) {
			myRdyTbl[i] = 0;		//초기화
		}

		


		OSSemPend(RandomSem, 0, &err);
		number[0] = random(64);			//랜덤한 테스크 네개 생성
		number[1] = random(64);
		number[2] = random(64);
		number[3] = random(64);
		OSSemPost(RandomSem);


		for (i = 0; i < 4; i++) {	//랜덤 값 네개 출력

			myRdyGrp |= myMapTbl[number[i] >> 3];
			myRdyTbl[number[i] >> 3] |= myMapTbl[number[i] & 0x07];

			PC_DispChar(dispX++, dispY, (number[i] / 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);	//글자색,배경색
			PC_DispChar(dispX++, dispY, (number[i] % 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispChar(dispX++, dispY, ' ', DISP_BGND_LIGHT_GRAY);
		}

		minValue = (myUnMapTbl[myRdyGrp] << 3) + myUnMapTbl[myRdyTbl[myUnMapTbl[myRdyGrp]]];
		//최솟값 찾기






		
			if (minValue < final) {			//찾은 최소값이 final보다 작을때만 출력하기

				check++;

				if ((check%4) == 1) {
					PC_DispChar(dispX++, dispY, (minValue / 10) + '0', DISP_FGND_WHITE + DISP_BGND_RED);
					PC_DispChar(dispX++, dispY, (minValue % 10) + '0', DISP_FGND_WHITE + DISP_BGND_RED);
				}
				else if ((check%4) == 2) {
					PC_DispChar(dispX++, dispY, (minValue / 10) + '0', DISP_FGND_WHITE + DISP_BGND_BLUE);
					PC_DispChar(dispX++, dispY, (minValue % 10) + '0', DISP_FGND_WHITE + DISP_BGND_BLUE);
				}
				else if ((check%4) == 3) {
					PC_DispChar(dispX++, dispY, (minValue / 10) + '0', DISP_FGND_WHITE + DISP_BGND_GREEN);
					PC_DispChar(dispX++, dispY, (minValue % 10) + '0', DISP_FGND_WHITE + DISP_BGND_GREEN);
				}
				else if ((check%4) == 0) {
					PC_DispChar(dispX++, dispY, (minValue / 10) + '0', DISP_FGND_WHITE + DISP_BGND_BROWN);
					PC_DispChar(dispX++, dispY, (minValue % 10) + '0', DISP_FGND_WHITE + DISP_BGND_BROWN);
					check = 0;
				}

				final = minValue;
				dispX -= 14;
				dispY++;

				OSTimeDlyHMSM(0, 0, 0, 300);
			}




			else  {
				PC_DispChar(dispX++, dispY, '0', DISP_FGND_LIGHT_GRAY + DISP_BGND_LIGHT_GRAY);
				PC_DispChar(dispX++, dispY, '0', DISP_FGND_LIGHT_GRAY + DISP_BGND_LIGHT_GRAY);
				dispX -= 14;
				dispY++;
				OSTimeDlyHMSM(0, 0, 0, 300);
			}
		





		// Change to next column
		if (dispY > 20) {
			dispY = 5;
			dispX += 16;
		}
		// Display Clear
		if (dispX > 79) {
			for (dispX = 0; dispX < 80; dispX++)
				for (dispY = 5; dispY < 21; dispY++)
					PC_DispChar(dispX, dispY, ' ', DISP_BGND_LIGHT_GRAY);
			dispX = 0;
			dispY = 5;
			OSTimeDlyHMSM(0, 0, 0, 3000);
		}

		// final value reset
		if (mode == '1') {
			if (final == 0) {
				final = 64;
				
				endCount++;
			}
		}
		else {
			if (final == 63) {
				final = 0;
				endCount++;
			}
		}
	}



	// Kill the task itself. Without this, the task is run 2 times.
	OSTaskDel(OS_PRIO_SELF);

}
