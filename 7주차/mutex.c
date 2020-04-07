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
#define VOTING_TASKS 3    /* Number of identical tasks                          */
#define DECISION_TASKS 1  /* Decision Task의 수								   */
#define MSG_Q_SIZE 2      /* Message Queue의 크기							   */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
OS_EVENT *RandomSem;



OS_STK TaskVotingStk[VOTING_TASKS][TASK_STK_SIZE]; /* Tasks stacks                                  */
OS_STK TaskDecisionStk[TASK_STK_SIZE];
OS_STK TaskStartStk[TASK_STK_SIZE];

OS_EVENT *mutex;    /* Semaphore										*/
OS_EVENT *MsgQueue; /* Message Queue									*/

/* Event Flag */
OS_FLAG_GRP *s_grp; /* 'O' 또는 'X'의 Count를 체크했는지 여부를 기록하는 Flag */
OS_FLAG_GRP *m_grp; /* Voting Task에서 'O'와 'X'를 임의로 생성한 여부를 기록하는 Flag */
OS_FLAG_GRP *n_grp; /* Decision Task에서 Master Task를 결정한 여부를 알려주기 위한 Flag */

void *MsgQTbl[VOTING_TASKS];  /* Message Queue Table							*/
INT8U TaskData[VOTING_TASKS]; /* Task의 인자 값이 저장							*/
INT8U selectMasterTask;       /* 결정 된 Master Task의 번호가 저장				*/

/* 생성 된 'O'와 'X'의 숫자가 저장 될 변수 */
INT8U o_cnt;
INT8U x_cnt;
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void VotingTask(void *pdata);   /* Function prototypes of tasks                  */
void DecisionTask(void *pdata); /* Function prototypes of tasks                  */
void TaskStart(void *pdata);    /* Function prototypes of Startup task           */
static void TaskStartCreateTasks(void);
static void TaskStartDispInit(void);
static void TaskStartDisp(void);

/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void main(void)
{
    INT8U err;

    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); /* Clear the screen                         */

    OSInit(); /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();        /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart(); /* Start multitasking                       */
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
static void TaskStart(void *pdata)
{
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

    for (;;)
    {
        TaskStartDisp(); /* Update the display                       */

        if (PC_GetKey(&key) == TRUE)
        { /* See if key has been pressed              */
            if (key == 0x1B)
            {                   /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn(); /* Return to DOS                            */
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

static void TaskStartDispInit(void)
{
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

static void TaskStartDisp(void)
{
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

    switch (_8087)
    { /* Display whether FPU present          */
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

static void TaskStartCreateTasks(void)
{
    INT8U i;
    INT8U err;

    /* Message Queue 생성 및 초기화 */
	MsgQueue = OSQCreate(&MsgQTbl[0], MSG_Q_SIZE);

    /* Semaphore 생성 및 초기화 */
	mutex = OSSemCreate(1);

    /* Event Flag 생성 및 초기화 */
	s_grp = OSFlagCreate(0x00, &err);
	m_grp = OSFlagCreate(0x00, &err);
	n_grp = OSFlagCreate(0x00, &err);

    // Task Create
    for (i = 0; i < VOTING_TASKS; i++)
    {
        TaskData[i] = i;
		OSTaskCreate(VotingTask, (void *)&i, &TaskVotingStk[i][TASK_STK_SIZE - 1], i + 1);
		OSTimeDlyHMSM(0, 0, 0, 50);
        
    }
	OSTaskCreate(DecisionTask, (void *)0, &TaskDecisionStk[TASK_STK_SIZE - 1], 5);
	OSTimeDlyHMSM(0, 0, 1, 0);


}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void VotingTask(void *pdata)
{
    /* 변수 선언부 */
	INT8U  taskNumber;			// Voting Task의 Task 번호 저장
	INT8U  err;					// err 정보 저장
	INT8U  makeRandomNumber;	// Random Number 생성 후 저장
	INT8U  i;
	char  vote;					// 'O' 또는 'X' 저장
	char  voteResult[3];		// Master Task가 정해지는 경우, Master Task로 전달되는 값 저장

	taskNumber = *(INT8U *)pdata;	// 인자 저장

	for (;;) {
		/* Random으로 0 또는 1을 생성하고, 0인 경우 'O', 1인 경우 'X'의 값을 생성해줌 */

		OSSemPend(RandomSem, 0, &err); /* Acquire semaphore to perform random numbers        */
		makeRandomNumber = random(2);
		OSSemPost(RandomSem);
		
		if (makeRandomNumber == 0) {
			vote = 'O';
		}
		else {
			vote = 'X';
		}

		/* Voting Task 번호 출력 */
		PC_DispStr(taskNumber * 20, 4, "Task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
		PC_DispChar(taskNumber * 20 + 4, 4, taskNumber + '0', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

		/* Voting Task가 모두 값을 생성하였으면, 이를 Decision Task에 전달하여 Master Task를 결정하도록 해줌 */

		OSFlagPost(m_grp, 0x01 << taskNumber, OS_FLAG_SET, &err);
		OSTimeDlyHMSM(0, 0, 0, 100);


		/* 이후 Decision Task가 Master Task를 결정할 때 까지 Waiting 상태로 진입 */
		OSFlagPend(n_grp, 0x01 << taskNumber, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);




		/* Master Task가 결정되었을 때, 해당 Task의 Number가 결정 된 Master Task와
		같은 경우 아래의 조건문으로 들어가며, 다른 경우, else문 으로 들어감 */
		if (taskNumber == selectMasterTask) {
			/* Master Task는 Cyan Color로 색칠해 줌. */
			PC_DispChar(taskNumber * 20 + 6, 4, vote, DISP_FGND_WHITE + DISP_BGND_CYAN);
		

			/* Master Task가 아닌 Voting Task들로 부터,
			Master Task로 Message Queue를 통해서 값('O' 또는 'X')을 전달받음 */		
			for (i = 0; i < 2; i++) {
				//OSQPost(MsgQueue, (void *)&taskNumber);
			//	OSTimeDlyHMSM(0, 0, 1, 0);
			//	voteResult[i] = (char *)OSQPend(MsgQueue, 0, &err);
				voteResult[i] = *(char*) OSQPend(MsgQueue, 0, &err);
			}

			voteResult[2] = vote;


			/* Master Task는 자신이 생성한 값과 나머지 Task들로 부터 받은 값을 가지고 'O'와 'X'의 수를 계산함. */
			for (i = 0; i < 3; i++) {
				/* Semaphore를 사용하여 공유 변수 보호 */
				OSMutexPend(mutex, 0, &err);
				if (voteResult[i] == 'O')
					o_cnt++;
				else
					x_cnt++;
				/* 이후 하나의 값을 체크(카운트 증가) 할 때 마다 Event Flag의 Flag bit를 하나씩 Set 해줌 */
				OSFlagPost(s_grp, (1 << i), OS_FLAG_SET, &err);
				OSMutexPost(mutex);
			}
		}
		else {
			/* Master Task가 아닌 Voting Task는 검은색 배경으로 출력 */
			PC_DispChar(taskNumber * 20 + 6, 4, vote, DISP_FGND_WHITE + DISP_BGND_BLACK);
			/* 이후 Master Task로 생성한 값을 전달하기 위해서 Message Queue로 값을 Post 해줌 */
			OSQPost(MsgQueue, (void *)&vote);

		}
		OSTimeDly(500);
	}
}

void DecisionTask(void *pdata)
{
    /* 변수 선언부 */
	INT8U  err;
	INT8U  x, y;
	INT8U  i;


	// avoid compiler warning prevention
	pdata = pdata;

	for (;;) {
		/* Voting Task에서 생성된 'O'와 'X'의 개수를 저장하는 변수 초기화 */
		o_cnt = 0x00;
		x_cnt = 0x00;

		/* Voting Task에서 O와 X가 모두 생성될 때 까지 OSFlagPend를 통해서 Waiting 상태에 들어감 */
		OSFlagPend(m_grp, 0x07, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);


		/* Voting Task에서 O와 X가 모두 생성되면, Master Task를 결정하기 위해서 Random Number 생성 */
		OSSemPend(mutex, 0, &err);        
		selectMasterTask = random(3);
		OSSemPost(mutex);

		/* Master Task가 결정되면, Waiting 상태의 Voting Task들에게 신호를 전달하여 Ready 상태로 만들어줌 */
		OSFlagPost(n_grp, 0x07, OS_FLAG_SET, &err);


		/* Master Task가 'O'와 'X'의 수를 모두 체크하였으면, Decision Task는 이에 따라서 화면을 색칠 */
		OSFlagPend(s_grp, 0x07, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);

		if (o_cnt > x_cnt) {
			/* O가 더 많이 생성 된 경우 파란색으로 색칠 */
			for (x = 0; x < 80; x++)
				for (y = 0; y < 16; y++)
					PC_DispChar(x, y + 6, ' ', DISP_BGND_BLUE);
		}
		else {
			/* X가 더 많이 생성 된 경우 빨간색으로 색칠 */
			for (x = 0; x < 80; x++)
				for (y = 0; y < 16; y++)
					PC_DispChar(x, y + 6, ' ', DISP_BGND_RED);
		}
	}
}
