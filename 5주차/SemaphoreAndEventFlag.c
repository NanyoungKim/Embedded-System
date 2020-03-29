/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*********************************************************************************************************
*/

#include <time.h>
#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define TASK_STK_SIZE 512 /* Size of each task's stacks (# of WORDs)            */
#define RANDOM_TASKS 4
#define DECISION_TASK 1
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK RandomTaskStk[RANDOM_TASKS][TASK_STK_SIZE];
OS_STK DecisionTaskStk[TASK_STK_SIZE];
OS_STK TaskStartStk[TASK_STK_SIZE];

INT8U TaskData[RANDOM_TASKS]; /* Parameters to pass to each task               */

OS_EVENT *RandomSem;

/* Semaphore										*/
OS_EVENT *Sem;

/* Event Flag */
OS_FLAG_GRP  *r_grp;
OS_FLAG_GRP  *s_grp;



INT8U send_array[4];
char receive_array[4];

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void TaskStart(void *data); /* Function prototypes of Startup task           */
static void TaskStartCreateTasks(void);
static void TaskStartDispInit(void);
static void TaskStartDisp(void);
static void RandomTask(void *pdata);
static void DecisionTask(void *pdata);

/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void main(void) {
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); /* Clear the screen                         */

    OSInit(); /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();        /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */

    RandomSem = OSSemCreate(RANDOM_TASKS);

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart(); /* Start multitasking                       */
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
static void TaskStart(void *pdata) {
#if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
    OS_CPU_SR cpu_sr;
#endif
    char s[100];
    INT16S key;
    INT8U i;

    pdata = pdata; /* Prevent compiler warning                 */

    TaskStartDispInit(); /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);      /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC); /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit(); /* Initialize uC/OS-II's statistics         */

    srand(time(0));

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

/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static void TaskStartDispInit(void) {
    /*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
    /*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr(0, 0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr(0, 1, "                                Embedded System 2                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 3, "                                     Lab. #6                                    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 4, "                                                                                ", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 5, "                                                                                ", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(0, 6, "                                                                                ", DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY);
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

/* You can write your own program here... */
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static void TaskStartCreateTasks(void) {
    INT8U i;
    INT8U err;

    /* Semaphore 초기화 */
	Sem = OSSemCreate(1);

    /* Event Flag 초기화 */
	s_grp = OSFlagCreate(0x00, &err);
	r_grp = OSFlagCreate(0x00, &err);


    /* Create RANDOM_TASKS & DECISION_TASK tasks           */
	for (i = 0; i < 4; i++) {
		OSTaskCreate(RandomTask, (void *)&i, &RandomTaskStk[i][TASK_STK_SIZE - 1], i + 1);
		OSTimeDlyHMSM(0, 0, 0, 50);

	}
	OSTaskCreate(DecisionTask, (void *)0, &DecisionTaskStk[TASK_STK_SIZE - 1], 5);
	OSTimeDlyHMSM(0, 0, 1, 0);

}




/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

//Colors
INT8U const colors[4] = {DISP_BGND_RED,
                         DISP_BGND_CYAN,
                         DISP_BGND_BLUE,
                         DISP_BGND_GREEN};

// Define maximum random number and tables
INT8U const myMapTbl[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
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

void RandomTask(void *pdata)  //task number를 인자로 받음
{
    INT8U err;

    INT8U number;

    INT8U dispX;
    INT8U dispY;

    INT8U taskNumber = *((INT8U *)pdata);

    // 테스크별 생성된 숫자와 결과값을 출력할 영역 -> 한번만 실행하면 됨
    PC_DispStr(0 + taskNumber * 16, 5, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispChar(4 + taskNumber * 16, 5, (taskNumber + 1) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr(8 + taskNumber * 16, 5, "[ ]", DISP_FGND_WHITE + colors[taskNumber]);

    while (1) {
        OSSemPend(RandomSem, 0, &err); /* Acquire semaphore to perform random numbers        */
        number = random(64);
        OSSemPost(RandomSem); /* Release semaphore                                  */

        // 화면에 생성된 숫자를 출력
        PC_DispChar(6 + taskNumber * 16, 5, (number / 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
        PC_DispChar(7 + taskNumber * 16, 5, (number % 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

        OSSemPend(Sem, 0, &err);

        // send_array에 random number 넣기
		send_array[taskNumber] = number;
        

        OSSemPost(Sem);

        // Flag Post

		OSFlagPost(s_grp, 0x01<<taskNumber,  OS_FLAG_SET, &err);
		OSTimeDlyHMSM(0, 0, 0,100);


        // Flag Pend receive_array에 4개의 결과를 받을 때 까지 기다림, OS_FLAG_CONSUME사용 0x0f => 0x00
		OSFlagPend(r_grp, 0x01<<taskNumber, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);



        // 결과 출력
        PC_DispChar(9 + taskNumber * 16, 5, receive_array[taskNumber], DISP_FGND_WHITE + colors[taskNumber]);

        if (receive_array[taskNumber] == 'W') {
            for (dispX = 0; dispX < 80; dispX++)
                for (dispY = 1; dispY < 16; dispY++)
                    PC_DispChar(dispX, dispY + 5, ' ', DISP_FGND_BLACK + colors[taskNumber]);
        }
    
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}

void DecisionTask(void *pdata) {
    INT8U err;

    INT8U minValue;

    INT8U i,j;

    INT8U myRdyTbl[8];
    INT8U myRdyGrp;
	INT8U check = 0;

    while (1) {
        myRdyGrp = 0x00;
        memset(myRdyTbl, 0x00, sizeof(myRdyTbl));


        // Flag Pend 4개의 Random Number를 받을 때 까지 기다림, OS_FLAG_CONSUME사용 0x0f => 0x00 
		OSFlagPend(s_grp, 0x0F, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);

        //  가장 작은 값을 찾음
        for (i = 0; i < RANDOM_TASKS; i++) {
            myRdyGrp |= myMapTbl[send_array[i] >> 3];
            myRdyTbl[send_array[i] >> 3] |= myMapTbl[send_array[i] & 0x07];
        }
        minValue = myUnMapTbl[myRdyGrp];  //상위 3bit
        minValue = (minValue << 3) + myUnMapTbl[myRdyTbl[minValue]];



        // receive_array에 값 저장 Flag Post => 0x0F

		for (i = 0; i < RANDOM_TASKS; i++) {

			if (minValue == send_array[i]) {
				receive_array[i] = 'W';
				check = i;

			}
			else continue;
			
		}

		for (j = 0; j < RANDOM_TASKS; j++) {

			if (check == j) { continue; }
			else {
				receive_array[j] = 'L';
			}
		}
		OSFlagPost(r_grp, 0x0F, OS_FLAG_SET, &err);


		OSTimeDlyHMSM(0, 0, 2, 0);


         
    }
}
