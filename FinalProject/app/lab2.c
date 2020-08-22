#include "includes.h"
#define F_CPU   16000000UL   // CPU frequency = 16 Mhz

#define  TASK_STK_SIZE  OS_TASK_DEF_STK_SIZE            
#define  N_TASKS        4

#define   MBOX   0
#define   GVAR   1
#define   METHOD   MBOX

#define   DEBUG   1
#define ON   1
#define OFF  0

OS_STK       TaskStk[N_TASKS][TASK_STK_SIZE];
OS_FLAG_GRP *sync;
OS_EVENT     *Mbox;
OS_EVENT     *Mutex;

volatile INT8U   FndNum;
volatile INT8U chk;
volatile int state = OFF;


void  AdcTask(void *data);
void  LedTask(void *data);
void  FndTask(void *data);
void  FndDisplayTask(void *data);

ISR(INT4_vect)		//스위치1 누르면 led 다 켜기
{
	INT8U err;
	PORTC = 0x00;
	_delay_ms(100);
	chk = 1;
	OSFlagPost(sync, 0x01, OS_FLAG_SET, &err);
	
}
ISR(INT5_vect) {	//스위치2 누르면 led 다 끄기
	INT8U err;
	PORTC = 0x00;
	_delay_ms(100);
	chk = 2;
	OSFlagPost(sync, 0x02, OS_FLAG_SET, &err);


}

int main(void)
{
	OSInit();

	OS_ENTER_CRITICAL();
	TCCR0 = 0x07;
	TIMSK = _BV(TOIE0);
	TCNT0 = 256 - (CPU_CLOCK_HZ / OS_TICKS_PER_SEC / 1024);
	OS_EXIT_CRITICAL();

	INT8U err;
	EIMSK = 0x30;		//INT4,5 interrupt enable			 0x0011 0000
	SREG |= 0x80;		//interrupt enable					 0x 1000 0000
	DDRE = 0xcf;		//4,5번 인터럽트는 입력				 0x 1100 1111
	EICRB = 0x0a;		//4,5번은 하강 엣지에서 인터럽트 발생   0x 0000 1010
	sync = OSFlagCreate(0, &err);


	OSTaskCreate(AdcTask, (void *)0, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(LedTask, (void *)0, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);
	OSTaskCreate(FndTask, (void *)0, (void *)&TaskStk[2][TASK_STK_SIZE - 1], 3);
	OSTaskCreate(FndDisplayTask, (void *)0, (void *)&TaskStk[3][TASK_STK_SIZE - 1], 4);

	OSStart();

	return 0;
}

void LedTask(void* data) {

	INT8U err;
	//스위치1 또는 스위치2를 누르면 플래그를 post 한다.
	OSFlagPend(sync, 0x03, OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME, 0,&err);

	if (chk==1) {		//스위치 1 눌렀을 때
		PORTA = 0xff;
		_delay_ms(100);
	}
	else if(chk==2){	//스위치2 눌렀을 때
		PORTA = 0x00;
		_delay_ms(100);
	}

}
unsigned short read_adc() {   //변환된 값 읽어오는 함수

	unsigned char adc_low, adc_high;
	unsigned short val;
	ADCSRA |= 0x40;                     //adc start conversion, adsc = '1'
	while ((ADCSRA & 0x10) != 0x10)         //adc 변환 완료를 검사한다
		;
	adc_low = ADCL;                     //변환된 low 값 읽어오기
	adc_high = ADCH;                  //변환된 high값 읽어오기
	val = (adc_high << 8) | adc_low;      //value는 high,low 합친 16비트값

	return val;
}

void AdcTask(void *data)
{
	data = data;

	int   value;
	INT8U   err;
	ADMUX = 0x00;
	ADCSRA = 0x87;            


	Mbox = OSMboxCreate(0);
	Mutex = OSMutexCreate(0, &err);

	DDRA = 0xFF;
	while (1) {
		value = read_adc();

		OSMboxPost(Mbox, &value);

		DDRB = 0x10;
		int i;

		if (value >= 871) {
			for (i = 0; i < 10; i++)
			{
				PORTB = 0x10;
				_delay_ms(1);
				PORTB = 0x00;
				_delay_ms(1);
			}
		}
		else {
			PORTA = 0b10101010;
		}
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

void FndTask(void *data)
{
	INT8U   err;

	data = data;

	while (1) {
		FndNum = *(int *)OSMboxPend(Mbox, 0, &err);
	}
}

void FndDisplayTask(void *data)
{
	unsigned char FND_DATA[] = {
	   0x3f, // 0
	   0x06, // 1 
	   0x5b, // 2 
	   0x4f, // 3 
	   0x66, // 4 
	   0x6d, // 5 
	   0x7d, // 6 
	   0x27, // 7 
	   0x7f, // 8 
	   0x6f, // 9 
	   0x77, // A 
	   0x7c, // B 
	   0x39, // C 
	   0x5e, // D 
	   0x79, // E 
	   0x71, // F 
	   0x80, // . 
	   0x40, // - 
	   0x08  // _
	};
	unsigned int num0, num1, num2, num3;
	INT8U   err;

	data = data;

	DDRC = 0xff;
	DDRG = 0x0f;

	while (1) {
		OSMutexPend(Mutex, 0, &err);
		num3 = (FndNum / 1000) % 10;
		num2 = (FndNum / 100) % 10;
		num1 = (FndNum / 10) % 10;
		num0 = FndNum % 10;
		OSMutexPost(Mutex);
		PORTC = FND_DATA[num3];
		PORTG = 0x08;
		_delay_ms(2);
		PORTC = FND_DATA[num2];
		PORTG = 0x04;
		_delay_ms(3);
		PORTC = FND_DATA[num1];
		PORTG = 0x02;
		_delay_ms(2);
		PORTC = FND_DATA[num0];
		PORTG = 0x01;
		_delay_ms(3);
	}
}
