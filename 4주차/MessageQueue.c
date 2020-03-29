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

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define TASK_STK_SIZE 512 /* Size of each task's stacks (# of WORDs)            */
#define RANDOM_TASKS 4
#define DECISION_TASK 1
#define MSG_QUEUE_SIZE 4
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK TaskStk[RANDOM_TASKS + DECISION_TASK][TASK_STK_SIZE];
OS_STK TaskStartStk[TASK_STK_SIZE];
INT8U TaskData[RANDOM_TASKS]; /* Parameters to pass to each task               */

char cmd;
OS_EVENT *RandomSem;

// Mail Box
OS_EVENT *Mbox[RANDOM_TASKS];

// Message Queue
OS_EVENT *MsgQ;
void *MsgQarr[MSG_QUEUE_SIZE];

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
    printf("Command 1: mailbox, 2: message queue\n");
    scanf("%c", &cmd);
    if (cmd != '1' && cmd != '2') exit(1);

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


/*void* OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *err) {
	void *msg;

	if (OSIntNesting > 0) {
		*err = OS_ERR_PEND_ISR;
		return ((void *)0);
	}
#if OS_ARG_CHK_EN>0
	if (pevent == (OS_EVENT *)0) {
		*err = OS_ERR_PEVENT_NULL;
		return ((void *)0);
		}
#endif
	OS_ENTER_CRITICAL();
	msg = pevent->OSEventPtr;
	if (msg != (void *)0) {
		pevent->OSEventPtr = (void *)0;
	}


}*/

static void TaskStartCreateTasks(void) {
    INT8U i;

    // Mail Box 초기화
	for (i = 0; i < 4; i++) {
		Mbox[i] = OSMboxCreate((void*)0);
	}
    


    // Message Queue 초기화
	MsgQ = OSQCreate(&MsgQarr[0], MSG_QUEUE_SIZE);
       

    // Create RANDOM_TASKS & DECISION_TASK tasks
		for (i = 0; i < 4; i++) {
	
			//PC_DispStr(0+16, 5, "Test", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			OSTaskCreate(RandomTask, (void *)&i, &TaskStk[i][TASK_STK_SIZE - 1], i+1);
			OSTimeDlyHMSM(0, 0, 0, 50);

			}
		OSTaskCreate(DecisionTask, (void *)0, &TaskStk[i][TASK_STK_SIZE - 1], 5);
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
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x50 to 0x5F                              */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x60 to 0x6F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x70 to 0x7F                             */
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x80 to 0x8F                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x90 to 0x9F                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xA0 to 0xAF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xB0 to 0xBF                             */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xC0 to 0xCF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xD0 to 0xDF                             */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xE0 to 0xEF                             */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0  /* 0xF0 to 0xFF                               */
};

void RandomTask(void *pdata)  //task number를 인자로 받음
{


	INT8U err;
	INT8U number;
	INT8U taskNumber = *(INT8U *)pdata;

	INT8U x, y;
	INT8U i;

	char *a;

	while (1) {

		//PC_DispChar(16, 7, (taskNumber)+'0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

		OSSemPend(RandomSem, 0, &err); /* Acquire semaphore to perform random numbers        */
		number = random(64);
		OSSemPost(RandomSem); /* Release semaphore                                  */



		//PC_DispChar(0, 10, (taskNumber-1)+'0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);-1
		PC_DispStr(0 + (taskNumber) * 16, 5, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
		PC_DispChar(4 + (taskNumber) * 16, 5, (taskNumber + 1) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
		PC_DispStr(8 + (taskNumber) * 16, 5, "[ ]", DISP_FGND_WHITE + colors[taskNumber]);

		// 화면에 생성된 숫자를 출력


		PC_DispChar(6 + (taskNumber) * 16, 5, (number / 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
		PC_DispChar(7 + (taskNumber) * 16, 5, (number % 10) + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);


		// Mail Box, Massege Queue 각각의 경우 Random Number를 보내고 결과를 기다림


		if (cmd == '1') {	//메세지 박스


			OSMboxPost(Mbox[taskNumber], (void*)&number);
			OSTimeDlyHMSM(0, 0, 1, 0);
			a = (char *)OSMboxPend(Mbox[taskNumber], 0, &err);


		}
		else {				//메세지 큐

			OSQPost(MsgQ, (void *)&number);
			OSTimeDlyHMSM(0, 0, 1, 0);
			a = (char *)OSQPend(MsgQ, 0, &err);
		}
		PC_DispChar(9 + taskNumber * 16, 5, *a, DISP_FGND_WHITE + colors[taskNumber]);		//문자 출력





		if (*a == 'w') {			//색깔 칠하기
			for (x = 0; x < 80; x++) {
				for (y = 8; y < 20; y++) {
					PC_DispChar(x, y, ' ', colors[taskNumber]);
				}
			}
		}

		//taskNumber++;


		// 결과 출력


		OSTimeDlyHMSM(0, 0, 3, 0);
	}

}




void DecisionTask(void *pdata) {
	INT8U err;
	INT8U arr[4];
	INT8U i;
	INT8U myRdyTbl[8];
	INT8U myRdyGrp;
	INT8U minValue = 0;
	char *ny;
	char *ww = 'w';
	char *ll = 'l';


	while (1) {
		myRdyGrp = 0;		//초기화
		for (i = 0; i < 8; i++) {
			myRdyTbl[i] = 0;		//초기화
		}

		if (cmd == '1') {		//메세지박스
			

			for (i = 0; i < 4; i++) {


				ny = (char*)OSMboxPend(Mbox[i], 0, &err);

				//PC_DispChar(16, 27, *arr[i], DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

				arr[i] = (INT8U)*ny;
				myRdyGrp |= myMapTbl[arr[i] >> 3];
				myRdyTbl[arr[i] >> 3] |= myMapTbl[arr[i] & 0x07];


			}

			minValue = (myUnMapTbl[myRdyGrp] << 3) + myUnMapTbl[myRdyTbl[myUnMapTbl[myRdyGrp]]];
			

			for (i = 0; i < 4; i++) {
				//arr[i] = OSMboxPend(Mbox[i], 0, &err);
				if (minValue == arr[i]) {
					OSMboxPost(Mbox[i], (void*)&ww);
					for (i = i + 1; i < 4; i++)
						OSMboxPost(Mbox[i], (void *)&ll);
					break;

				}
				else {
					OSMboxPost(Mbox[i], (void*)&ll);
				}
			}

			
		}//cmd==1

		else {		//메세지큐

			for (i = 0; i < 4; i++) {
				ny = (char*)OSQPend(MsgQ, 0, &err);

				//PC_DispChar(16, 27, *arr[i], DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

				arr[i] = (INT8U)*ny;
				myRdyGrp |= myMapTbl[arr[i] >> 3];
				myRdyTbl[arr[i] >> 3] |= myMapTbl[arr[i] & 0x07];
			}

			minValue = (myUnMapTbl[myRdyGrp] << 3) + myUnMapTbl[myRdyTbl[myUnMapTbl[myRdyGrp]]];

			for (i = 0; i < 4; i++) {
				//arr[i] = OSMboxPend(Mbox[i], 0, &err);
				if (minValue == arr[i]) {
					OSQPost(MsgQ, (void*)&ww);
					for (i = i + 1; i < 4; i++)
						OSQPost(MsgQ, (void *)&ll);
					break;

				}
				else {
					OSQPost(MsgQ, (void*)&ll);
				}
			}

		}








		OSTimeDlyHMSM(0, 0, 2, 0);

	}	//while
}

