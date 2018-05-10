#include "exynos_4412.h"

void process_cmd(void);

void led_init(void)
{
	GPX2.CON = (GPX2.CON & (~(0xf<<28))) | (0x1<<28);
//31:28   27-0
	//1111 0000 0000 0000 0000 0000 0000 0000
	//0001 0000 0000 0000 0000 0000 0000 0000
	GPX1.CON = (GPX1.CON & (~(0xf<<0))) | (0x1<<0);
	GPF3.CON = (GPF3.CON & (~(0xff<<16))) | (0x11<<16);
}
void led_on(int num)
{
	switch(num)
	{
		case 1:
			GPX2.DAT = GPX2.DAT | 0X1<<7;
			break;
		case 2:
			GPX1.DAT = GPX1.DAT | 0X1<<0;
			break;
		case 3:
			GPF3.DAT = GPF3.DAT | 0X1<<4;
			break;
		case 4:
			GPF3.DAT = GPF3.DAT | 0X1<<5;
			break;

	}
}
void led_off(int num)
{
	switch(num)
	{
		case 1:
			GPX2.DAT = GPX2.DAT & (~(0X1<<7));
			break;
		case 2:
			GPX1.DAT = GPX1.DAT & (~(0X1<<0));
			break;
		case 3:
			GPF3.DAT = GPF3.DAT & (~( 0X1<<4));
			break;
		case 4:
			GPF3.DAT = GPF3.DAT & (~(0X1<<5));
			break;
	}
}
void delay_ms(int num)
{
	int i,j;
	for(i=num;i>0;i--)
	{
		for(j=1000;j>0;j--);
	}
}
void beep_init(void)
{
	GPD0.CON = (GPD0.CON &(~(0xf<<0))) | (0x2<<0);
	PWM.TCFG0 = (PWM.TCFG0 &(~(0xff<<0))) | (0xff<<0);
	PWM.TCFG1 = (PWM.TCFG1 &(~(0xf<<0))) | (0x2<<0);
	PWM.TCNTB0 = 500;
	PWM.TCMPB0 = 250;
	PWM.TCON = (PWM.TCON &(~ (0xf<<0)))  | (1<<3|1<<1);
}
void beep_on(void)
{
	PWM.TCON = (PWM.TCON &(~ (0xf<<0)))  | (1<<3 | 1<<0);
}
void beep_off(void)
{
	PWM.TCON = (PWM.TCON &(~ (0xf<<0)))  | (0<<0);
}
#define BEEP_HZ 25000

void beep_set_frequence(unsigned int fre)
{
	if(fre == 0)
	{
		return;
	}

	PWM.TCNTB0 = 25000/fre;
	PWM.TCMPB0 = 25000/(2*fre);
}

const unsigned char dahai[] =
{
	0x13,  0x15 , 0x16, 0x16, 0x16, 0x16, 0x21, 0x16, 0x15, 0x15, 0x16, 0x15,  //����
	0x13,  0x12 , 0x11, 0x11, 0x11, 0x11, 0x12, 0x13,
	0x13,  0x12 , 0x11, 0x11, 0x11, 0x11, 0x21, 0x16, 0x15, 0x15, 0x16, 0x15,
	0x13,  0x15,  0x16, 0x21, 0x21, 0x16, 0x15, 0x15,//ƮԶ
};
const unsigned char happy[] =
{
	0x15,  0x15 , 0x16, 0x15, 0x21, 0x17, 0x15, 0x15, 0x16, 0x15, 0x22, 0x21,  //����
	0x13,  0x12 , 0x11, 0x11, 0x11, 0x11, 0x12, 0x13,
	0x13,  0x12 , 0x11, 0x11, 0x11, 0x11, 0x21, 0x16, 0x15, 0x15, 0x16, 0x15,
	0x13,  0x15,  0x16, 0x21, 0x21, 0x16, 0x15, 0x15,//ƮԶ
};
/*
 * �󺣵ļ���, ����ÿһ������ʱ�� 1Ϊ��ʱ�� 4Ϊ��ʱ��
 */
const unsigned char time[] =
{
	4,		4,   4,     2,    4 ,   2,    4,     4,    4,    2,    4,    2 ,//����
	4,      4,   4,     2,    4,    2,    2,     1,
	4,      4,   4,     2,    4,    2,    4,     4,     4,   2,    4,    2,
	4,      4,   2,      4,   2,     4,   4,     1  //ƮԶ

};
const unsigned int yinyue[3][7]
={
	262,  294,  330,  370,  415,  266,  294,  //����
	523,  578,  659,  698,  784,  880,  988,  //����
	1046, 1174, 1318, 1396, 1567, 1700, 1975  //����
};
int flag = 0;

void do_irq(void)
{
	int irqnum;
	int adc_num;

	irqnum = CPU0.ICCIAR &(0x3ff);
	switch(irqnum)
	{
		case 57:
			printf("----key1-----\n");
			EXT_INT41_PEND = EXT_INT41_PEND | (0x1<<1);
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<25);

			break;
		case 42:
			//��ȡֵ ���ж�
			adc_num = ADCDAT &0xfff;
			//printf("adc = %0.2fV \n",(adc_num*1.8)/4096);
			printf("adc = %d\n",adc_num);

			CLRINTADC = 0; 				  //1 ������ �Լ�Ҫ��Ҫ���ж�
			//IECR2 = IECRn | (0x1<<19);  //2 combiner
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<10);//3 gic  42/32  = 1   10

			break;

		case 75:
			printf("----WDT--\n");

			WDT.WTCLRINT = 0x11;//1

			ICDICPR.ICDICPR2 = ICDICPR.ICDICPR2 |(0x1<<11);//3
			break;
		case 76:
			printf("----alarm-----\n");
			RTCINTP = RTCINTP | 0x1<<1;

			ICDICPR.ICDICPR2 = ICDICPR.ICDICPR2 |(0x1<<12);
			break;

		case 77:
			printf("----tick----count--\n");
			RTCINTP = RTCINTP | 0x1<<0;

			RTCCON =  RTCCON&(~(0x1<<8) );
			ICDICPR.ICDICPR2 = ICDICPR.ICDICPR2 |(0x1<<13);
			break;


	}
	CPU0.ICCEOIR = (CPU0.ICCEOIR &(~(0x3ff<<0))) | irqnum;
}
void key_int_init(void)
{
	GPX1.CON = (GPX1.CON & (~(0XF<<4))) | (0xf<<4);
	EXT_INT41_CON = (EXT_INT41_CON&(~(0xf<<4)))|(0x2<<4);
	EXT_INT41_MASK = EXT_INT41_MASK & (~(0x1<<1));

	ICDDCR = 1;
	ICDISER.ICDISER1 = ICDISER.ICDISER1 | (0x1<<25);
	ICDIPTR.ICDIPTR14 = (ICDIPTR.ICDIPTR14 &(~(0xff<<8))) |(0x1<<8);
	CPU0.ICCPMR = 255;
	CPU0.ICCICR = 1;
}
void adc_int_init(void)
{
	//combiner
	IESR2 = IESR2 | (0x1<<19);
	//gic
	ICDDCR = 1;
	ICDISER.ICDISER1  = ICDISER.ICDISER1 | (0x1<<10);//42/32 = 1  10
	ICDIPTR.ICDIPTR10 = (ICDIPTR.ICDIPTR10 &(~(0xff<<16))) |(0x1<<16);//42/4 = 10 2
	CPU0.ICCPMR = 255;
	CPU0.ICCICR = 1;
}
adc_init()
{
	ADCMUX = 3;
	ADCCON =  (1<<16) | (1<<14) | (0xff <<6) |  (1<<0);
}
void rtc_init(void)
{
	RTCCON = 1;//2018 05 09  15:15:0
	RTC.BCDYEAR = 0x18;
	RTC.BCDMON = 0x5;
	RTC.BCDWEEK = 0x9;
	RTC.BCDHOUR = 0x15;
	RTC.BCDMIN  = 0x15;
	RTC.BCDSEC  = 0x0;
	RTCCON = 0;
}
void rtc_tick(void)
{
	RTCCON = (RTCCON&(~(0xf<<4)))|(0x0<<4) | (0x1<<8) ;
	TICCNT  = 2*32768;

//�ж�
	ICDDCR = 1;
	ICDISER.ICDISER2  = ICDISER.ICDISER2 | (0x1<<13);//77/32 = 2  13
	ICDIPTR.ICDIPTR19 = (ICDIPTR.ICDIPTR19 &(~(0xff<<8))) |(0x1<<8);//77/4 = 19 1
	CPU0.ICCPMR = 255;
	CPU0.ICCICR = 1;

}
void rtc_alarm(void )
{
	RTCALM.ALM = (1<<6)|(1<<1)|(1<<0);//ÿ��Сʱ�� xx�֣�xx�� ����һ���ж�
	RTCALM.MIN = 0x15;
	RTCALM.SEC = 0x20;

	//�ж�
	ICDDCR = 1;
	ICDISER.ICDISER2  = ICDISER.ICDISER2 | (0x1<<12);//76/32 = 2  12
	ICDIPTR.ICDIPTR19 = (ICDIPTR.ICDIPTR19 &(~(0xff<<0))) |(0x1<<0);//76/4 = 19 0
	CPU0.ICCPMR = 255;
	CPU0.ICCICR = 1;

}
void wdt_init(void )
{
//	WDT.WTCON =   249<<8 | 1<<5 | 1<<2 | 1<<0;  //��λ
	WDT.WTCON =   249<<8 | 1<<5 | 1<<2 ;  //��ͨ��ʱ��
	WDT.WTDAT =  25000;//0x8000

	ICDDCR = 1;
	ICDISER.ICDISER2  = ICDISER.ICDISER2 | (0x1<<11);//75/32 = 2  11
	ICDIPTR.ICDIPTR18 = (ICDIPTR.ICDIPTR18 &(~(0xff<<24))) |(0x1<<24);//75/4 = 18 3
	CPU0.ICCPMR = 255;
	CPU0.ICCICR = 1;
}
int flage = 0; //1  ��   0   stop
void play_music(void)
{
	int loop,a,b,delay;
	//ѭ������ ����
	for(loop=0; loop<sizeof(happy)/(sizeof(unsigned char)); loop++)
	{
		if(flage == 0)
		{
			return;
		}

		//��󺣵ļ��ױ�
		a = happy[loop]>>4 ;       		    //������ߵ���
		b = (happy[loop] & 0x0f )-1;  	    //�����DO RE MI FA SO LA XI
		delay = 0x1500/time[loop];
		beep_set_frequence( yinyue[a][b] ); //�����趨Ƶ��
		beep_on();			//����һ����
		delay_ms(delay);

		beep_off();         //�رշ������� ÿ����������ɺ��м����
	//	delay_ms(0x10);   //�رշ�����
		delay_ms(100);
		//��� �ǲ��Ǵ������������
		if(UART2.UTRSTAT2 & 0x1)
		{
			process_cmd();
		}
	}


}
void process_cmd(void)
{
	char cmd[32];
		gets(cmd);

		putc(cmd[0]);

		switch(cmd[0])
		{
			case '1':
				beep_on();
				break;
			case '2':
				beep_off();
				break;
			case '3':
				led_on(1);
				break;
			case '4':
				led_off(1);
				break;
			case '5':
				flage = 1;
				play_music();
				break;
			case '6':
				flage = 0;
				break;
			default:
				printf("invalid cmd\n");
				break;
		}
}
int main(int argc,char **argv)
{
	char cmd[16];


	uart_init();
	led_init();
	beep_init();

	puts("hello farsight\n");

	while(1)
	{
		process_cmd();
	}

#if 0

	rtc_init();
	rtc_tick();
	rtc_alarm();

	while(1)
	{
		printf("%x-%x-%x %02x:%02x:%02x %02x  \n",
				RTC.BCDYEAR ,
					RTC.BCDMON,
					RTC.BCDWEEK,
					RTC.BCDHOUR,
					RTC.BCDMIN,
					RTC.BCDSEC,
					RTC.BCDDAY
					);
		delay_ms(2500);
	}

	wdt_init();

	printf("reset");
	while(1)
	{
		WDT.WTCNT = 25000;//ι������û����0 ��ι����WDT�Ͳ�������ж��źŻ���reset�ź�
		delay_ms(100);
	}


	led_init();
	beep_init();
	printf("11111111\n");
	adc_int_init();
	adc_init();
	key_int_init();
	while(1)
	{
		ADCCON |= 1;    //���������adc��������������ʼת��
		delay_ms(2500);
	}


	key_int_init();

	while(1);


	GPX1.CON = (GPX1.CON & (~(0xf<<4)) ) | (0x0<<0);

	while(1)
	{
		if(!(GPX1.DAT & (0x1<<1)))
		{
			delay_ms(10);
			if(!(GPX1.DAT & (0x1<<1)))
			{
				//	��������
				led_on(1);
				beep_on();
				delay_ms(500);
				led_off(1);
				beep_off();
			}
		}
	}




	int loop,a,b,delay;

	beep_init();
	while(1)
	{
		//ѭ������ ����
		for(loop=0; loop<sizeof(happy)/(sizeof(unsigned char)); loop++)
		{

			//��󺣵ļ��ױ�
			a = happy[loop]>>4 ;       		    //������ߵ���
			b = (happy[loop] & 0x0f )-1;  	    //�����DO RE MI FA SO LA XI
			delay = 0x1500/time[loop];
			beep_set_frequence( yinyue[a][b] ); //�����趨Ƶ��
			beep_on();			//����һ����
			delay_ms(delay);

			beep_off();         //�رշ������� ÿ����������ɺ��м����
		//	delay_ms(0x10);   //�رշ�����
			delay_ms(100);
		}
	}





	int i;
	int ledno;

	led_init();

	for(i=0;i<1000;i++)
	{
		ledno = i%4 + 1;
		led_on(ledno);
		delay_ms(1000);
		led_off(ledno);
	}
#endif
	return 0;
}
