/* Includes ------------------------------------------------------------------*/
#include "MDR1986VE8T.h"
#include "MDR32F8_it.h"
#include "spec.h"

#include "mdr32f8_config.h"
#include "mdr32f8_port.h"
#include "mdr32f8_clkctrl.h"
#include "mdr32f8_milstd.h"
#include "mdr32f8_ssp.h"
#include "mdr32f8_tim.h"




PORT_InitTypeDef PORT_InitStructure;
MIL_STD_1553_InitTypeDef MIL_STD_15531_InitStructure;
SSP_InitTypeDef SSP_InitStructure;
TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;



void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}


int main(void)
{	
	uint32_t tmpreg;
	int16_t massShIM[6];
	int16_t massData[4];
	uint16_t snd = 0;
	uint16_t clr = 0;	
	uint16_t ReturneData[1][15];
	float *dataSetUK;
	float *dataGetUK;
	
	int j  = 0;//2 SSP send cycle counter
	int k  = 0;//SSP recieve cycle counter
	int i  = 0;//1 SSP send cycle counter
	int a=0;
	int b=0;
	float x=0;
	float y=0;
	float x1=0;
	float y1=0;
	float  da=0;
	float  db=0;
	float  dda=0;
	float ddb=0;
	float  pa=0;
	float  pb=0;
	float  da1=0;
	float  db1=0;
	float g=0;
	float v=0;
	float g1=0;
	float v1=0;
	float ia=0;
	float ib=0;
	
	
	uint16_t massAdrData[11][3]=
		{
			{0x8040,0x00fb,0xc041},
			{0x8080,0x8f00,0xc081},
			{0x80c1,0x005f,0xc0c0},
			{0x82c0,0x83fe,0xc2c1},
			{0x8301,0xa210,0xc300},
			{0x8340,0x0282,0xc341},
			{0x89c1,0x3e80,0xc9c0},
			{0x8ac1,0x83fe,0xcac0},
			{0x8b00,0xa210,0xcb01},
			{0x8b41,0x0282,0xcb40},
			{0x91c1,0x3e80,0xd1c0}
		};
	
	
	
	for(i = 0 ; i < 4 ; i++)
		{
			massShIM[i] = 0;
		}	
	
	CLKCTRL_DeInit();
	/* Enable HSE0 clock */
	CLKCTRL_HSEconfig(CLKCTRL_HSE0_CLK_ON);
	
	/* Check HSE success, enable PLL0, check PLL0 success, select MAX_CLK src */
	while(CLKCTRL_HSEstatus(CLKCTRL_HSEn_STAT_HSE0_RDY) != SUCCESS){}
		
	/* Select PLL0 clk src, PLL0_N, PLL0_Q to get FINT = FIN*(PLLn_N)/(PLLn_Q+1) */
	CLKCTRL_CPU_PLLconfig(PLL0, CLKCTRL_PLLn_CLK_SELECT_HSE0div1, 3, 8);//PLLn, SRC, Q, N
		while(CLKCTRL_CPU_PLLstatus(0) != SUCCESS){}
			CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_PLL0);	

	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTA_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTB_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTC_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTD_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTE_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_MIL0_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_SSP0_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_TMR0_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_TMR3_EN, ENABLE);
	
	KEY_reg_accs();
	
	
	//MIL_STD
	/* PORT configuration */	
	/*****************************XP22*****************************/
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_13;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTB, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin = (PORT_Pin_25 | PORT_Pin_26 | PORT_Pin_27 | PORT_Pin_28 | PORT_Pin_29);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_12;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTA, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_22);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_12;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTA, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_30 | PORT_Pin_31);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_13;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTA, &PORT_InitStructure);
		
  MIL_STD_1553_CLK_en(MIL_STD_15531, MIL_STD_1553_CLKdiv1);

	/*-- Terminal device configuration -----------------------------------------*/
	MIL_STD_1553xStructInit(&MIL_STD_15531_InitStructure);
	
	/* Initialize MIL_STD_1553_InitStructure */
	MIL_STD_15531_InitStructure.MIL_STD_1553_Mode = MIL_STD_1553_ModeTerminal;
	MIL_STD_15531_InitStructure.MIL_STD_1553_RERR = DISABLE;
	MIL_STD_15531_InitStructure.MIL_STD_1553_DIV = 20;//80
	MIL_STD_15531_InitStructure.MIL_STD_1553_RTA = 0x01;
	MIL_STD_15531_InitStructure.MIL_STD_1553_TRA = ENABLE;
	MIL_STD_15531_InitStructure.MIL_STD_1553_TRB = ENABLE;
	/* Configure MIL_STD_15531 parameters */
	MIL_STD_1553_Init(MIL_STD_15531, &MIL_STD_15531_InitStructure);
	
	MIL_STD_1553_Cmd(MIL_STD_15531, ENABLE);
	

	//SPI
	/*PORT configuration*/
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_26 | PORT_Pin_25);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_6;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTD, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_23);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_6;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTE, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_24);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_6;
	PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
	PORT_Init(PORTB, &PORT_InitStructure);
	
	SSP_CLK_en(SSP_CLKSRC_MAX_CLK, SSP_CLKdiv1);
	
	SSP_StructInit (&SSP_InitStructure);

	SSP_InitStructure.SSP_SCR  = 0;//0
	SSP_InitStructure.SSP_CPSDVSR = 2;//2
	SSP_InitStructure.SSP_Mode = SSP_ModeMaster;//master_slave
	SSP_InitStructure.SSP_WordLength = SSP_WordLength16b;//dlinna slova
	SSP_InitStructure.SSP_SPH = SSP_SPH_1Edge;// faza
	SSP_InitStructure.SSP_SPO =SSP_SPO_Low;//polyarnost'
	SSP_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;//interfeis
	SSP_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	SSP_Init (MDR_SSP0,&SSP_InitStructure);

	/* Enable SSP0 */
	SSP_Cmd(MDR_SSP0, ENABLE);
	
	
	 //init 1310
		for(j = 0 ; j < 11 ; j++)
		{
			for(i = 0 ; i < 2 ; i++)
		{		
			    snd = massAdrData[j][i];
					SSP_SendData(MDR_SSP0, snd);
					clr  = SSP_ReceiveData(MDR_SSP0);
		}
		}
		
		clr  = SSP_ReceiveData(MDR_SSP0);
		

		for(k = 0 ; k < 13 ; k++)
			{
					SSP_SendData(MDR_SSP0, massAdrData[k][2]);
					ReturneData[0][k] = SSP_ReceiveData(MDR_SSP0);	
			}
		
		clr  = SSP_ReceiveData(MDR_SSP0);
			
			
		Delay(1000);
	

	//tim_PWM
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_8 | PORT_Pin_10);//tmr0
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_7;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTA, &PORT_InitStructure);
	
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1| PORT_Pin_3 | PORT_Pin_5);//tmr3
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_9;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTC, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_13);//tmr3
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_8;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTE, &PORT_InitStructure);
	
	
	//TIMER3
	/* Initializes the TIMERx Counter ------------------------------------*/
  sTIM_CntInit.TIMER_Prescaler                = 0;
  sTIM_CntInit.TIMER_Period                   = 40000;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TMR3,&sTIM_CntInit);
	
	/* Initializes the TIMER3 Channel 0,0N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR3, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER3 Channel 0,0N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL1;
  TIMER_ChnOutInit(MDR_TMR3, &sTIM_ChnOutInit);


	/* Initializes the TIMER3 Channel 1,1N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR3, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER3 Channel 1,1N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL2;
  TIMER_ChnOutInit(MDR_TMR3, &sTIM_ChnOutInit);
	
	/* Initializes the TIMER3 Channel 2,2N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR3, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER3 Channel 2,2N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL3;
  TIMER_ChnOutInit(MDR_TMR3, &sTIM_ChnOutInit);


	/* Initializes the TIMER3 Channel 3,3N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL4;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR3, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER3 Channel 3,3N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL4;
  TIMER_ChnOutInit(MDR_TMR3, &sTIM_ChnOutInit);
	

	
	//TIMER0	
	/* Initializes the TIMERx Counter ------------------------------------*/
  sTIM_CntInit.TIMER_Prescaler                = 0;
  sTIM_CntInit.TIMER_Period                   = 40000;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TMR0,&sTIM_CntInit);
	
	/* Initializes the TIMER0 Channel 0,0N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR0, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER0 Channel 0,0N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL1;
  TIMER_ChnOutInit(MDR_TMR0, &sTIM_ChnOutInit);


	/* Initializes the TIMER0 Channel 1,1N-------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena            = ENABLE;
  TIMER_ChnInit(MDR_TMR0, &sTIM_ChnInit);
	
	
	/* Initializes the TIMER0 Channel 1,1N-------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL2;
  TIMER_ChnOutInit(MDR_TMR0, &sTIM_ChnOutInit);
	
	
  /* Enable TIMERS */

	TIMER_Cmd(MDR_TMR0,ENABLE);
	TIMER_Cmd(MDR_TMR3,ENABLE);
	
	tmpreg = CLK_CNTR->TIM0_CLK;
	tmpreg &= ~TIM_DIV_Msk;
	tmpreg |=  TIM_CLKdiv1;
	CLK_CNTR->TIM0_CLK = tmpreg;
	
	tmpreg = CLK_CNTR->TIM3_CLK;
	tmpreg &= ~TIM_DIV_Msk;
	tmpreg |=  TIM_CLKdiv1;
	CLK_CNTR->TIM3_CLK = tmpreg;
	
	CLK_CNTR->TIM0_CLK =  CLK_CNTR->TIM3_CLK |=  TIM_CLK_EN;
	
	
		TIMER_SetChnCompare(MDR_TMR0, TIMER_CHANNEL1, 0);
		TIMER_SetChnCompare1(MDR_TMR0, TIMER_CHANNEL1, 19800);
		TIMER_SetChnCompare(MDR_TMR0, TIMER_CHANNEL2, 20000);
		TIMER_SetChnCompare1(MDR_TMR0, TIMER_CHANNEL2, 39800);
		

	while (1)
	{
		//Shim upr
		MIL_STD_1553_ReceiveData(MIL_STD_15531, 2, 6, (uint16_t *) massShIM);
		dataSetUK=(float *)massShIM;
		dataGetUK=(float *)massData;
		x=dataSetUK[0];
		y=dataSetUK[1];
		
		
		//a=massShIM[4]*100;		
		//b=massShIM[5]*100;
		
		
		SSP_SendData(MDR_SSP0, 0xc201);
		v1 = SSP_ReceiveData(MDR_SSP0);
		
		SSP_SendData(MDR_SSP0, 0xca00);
		g1 = SSP_ReceiveData(MDR_SSP0);
		
		if(x==x1)
			ia=ia;
		else
			ia=0;
		
		if(y==y1)
			ib=ib;
		else
			ib=0;
		
		if(g1<36864)
			g=(g1-4164)/793;
		else
			g=(g1-65535-4164)/793;
		
		v=(20928-v1)/966;
		
		dataGetUK[0]=g;
		dataGetUK[1]=v;
		
		MIL_STD_1553_WiteDataToSendBuffer(MIL_STD_15531,1,4,(uint16_t *) massData);
		
		da=g-x;
		db=v-y;
		dda=(da-da1)/0.001;
		ddb=(db-db1)/0.001;
		da1=da;
		db1=db;
		pa=10*da+0.3*dda;//+0.02*ia;
		pb=10*db+0.3*ddb;//+0.02*ib;
		ia=ia+da*0.001;
		ib=ib+db*0.001;
		if (pa>0)
			a=pa*1000;
		else
			a=-pa*1000;
		
		if (a>10000)
			a=10000;
		else
			a=a;
		
		if (pb>0)
			b=pb*1000;
		else
			b=-pb*1000;
		
		if (b>10000)
			b=10000;
		else
			b=b;
		
		x1=x;
		y1=y;
		
		
//dv1
	if(x<g)
				{
					MDR_TMR3->CH1_CNTRL1 |= TIMER_CH_CNTRL1_NINV;
					TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL1, a);
					TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL1, 40000-a);
					
					MDR_TMR3->CH2_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
					TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL2, 20000-a);
					TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL2, 20000+a);
					
					
				}
	
					else
							{
								if(x>g)
											{
												MDR_TMR3->CH1_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
												TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL1, 19800-a);
												TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL1, 19800+a);
											
												if(a>200)
																{
																	MDR_TMR3->CH2_CNTRL1 |= TIMER_CH_CNTRL1_NINV;
																	TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL2, a-200);
																	TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL2, 39800-a);
																}
																else
																		{
																			MDR_TMR3->CH2_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
																			TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL2, 39800-a);
																			TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL2, 39800+a);
																		}
												
											}
											else
													{
													MDR_TMR3->CH1_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;				
													TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL1, 0);
													TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL1, 0);
													MDR_TMR3->CH2_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
													TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL2, 20000);
													TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL2, 20000);
													}
							}
							
							
	//dv2
	if(y<v)
				{
				MDR_TMR3->CH3_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
				TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL3, 20000-b);
				TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL3, 20000+b);
				
				MDR_TMR3->CH4_CNTRL1 |= TIMER_CH_CNTRL1_NINV;
				TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL4, b);
				TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL4, 40000-b);
						
					
				}
	
				else
							{
								if(y>v)
											{
			MDR_TMR3->CH4_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
												TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL4, 19800-b);
												TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL4, 19800+b);
											
												if(b>200)
																{
																	MDR_TMR3->CH3_CNTRL1 |= TIMER_CH_CNTRL1_NINV;
																	TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL3, b-200);
																	TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL3, 39800-b);
																}
																	else
																			{
																				MDR_TMR3->CH3_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
																				TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL3, 39800-b);
																				TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL3, 39800+b);
																			}
												
											}
											else
													{
													MDR_TMR3->CH3_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;				
													TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL3, 0);
													TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL3, 0);
													MDR_TMR3->CH4_CNTRL1 &=  ~TIMER_CH_CNTRL1_NINV;
													TIMER_SetChnCompare(MDR_TMR3, TIMER_CHANNEL4, 20000);
													TIMER_SetChnCompare1(MDR_TMR3, TIMER_CHANNEL4, 20000);
													}
							}
		
		 
	}
}

