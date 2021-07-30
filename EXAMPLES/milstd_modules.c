/**
  ******************************************************************************
  * @file    main.c
  * @author  Milandr Application Team
  * @version V1.0.0
  * @date    02/09/2015
  * @brief   Main program body.
  ******************************************************************************
  * <br><br>
  *
  *
  * <h2><center>&copy; COPYRIGHT 2015 Milandr</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "cm4ikmcu.h"
#include "MDR32F8_it.h"
#include "spec.h"

#include "mdr32f8_config.h"
#include "mdr32f8_port.h"
#include "mdr32f8_clkctrl.h"
#include "mdr32f8_uart.h"
#include "mdr32f8_ssp.h"
#include "mdr32f8_can.h"
#include "mdr32f8_arinc_rx.h"
#include "mdr32f8_arinc_tx.h"
#include "mdr32f8_milstd.h"

/** @addtogroup __MDR32F8_StdPeriph_Examples
  * @{
  */

/** @addtogroup __MDR32F8_Board_For_MCU
  * @{
  */

/** @addtogroup SPI_Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PORT_InitTypeDef PORT_InitStructure;
ARINC429R_InitChannelTypeDef ARINC429R_InitChannelStruct;
ARINC429T_InitChannelTypeDef ARINC429T_InitChannelStruct;
MIL_STD_1553_InitTypeDef MIL_STD_1553_InitStructure;
MIL_STD_1553_InitTypeDef MIL_STD_1553_InitStructure2;

volatile uint32_t ch = 0;
volatile uint32_t c = 0;
uint32_t i = 0;
uint32_t tmp = 0;
			
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void POR_disable();
void KEY_reg_accs();
void BlinkLine(uint32_t Pin);
void StartTRM(uint32_t TermAddr, uint32_t Format, uint32_t Chnl, uint32_t Code);
	
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
		
int main(void)
{	
	POR_disable();
	KEY_reg_accs();
	
	CLKCTRL_DeInit();
	/*HSE0 as the clk source*/
	RST_CLK_HSEconfig(CLKCTRL_HSEn_CLK_ON);
	if(CLKCTRL_HSEstatus() == ERROR)
	while(1);	
		
	CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_HSE0div1);	
	
	//--------------------------------------------<	
	CLK_CNTR->CPU_CLK = 0;	//Core clock <-> MAX_CLOCK
	
	CLK_CNTR->PLL0_CLK = (2 << 29)|(8 << 8)|(0 << 0);	// PLL_FREQ = (16 MHz * 5) / 1 = 80 MHz
		
	CLK_CNTR->PLL0_CLK |= (1 << 27);	//PLL ON
	
	while((CLK_CNTR->PLL0_STAT & (1 << 20)) == 0){}	//wait while PLL0 not ready
		
	CLK_CNTR->MAX_CLK = 8;	//MAX_CLOK <-- PLL0
	//-------------------------------------------->		

	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTA_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTB_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTC_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTD_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTE_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_MIL0_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_MIL1_EN, ENABLE);
	
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^BLINK
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_16|PORT_Pin_17|PORT_Pin_18|PORT_Pin_19|
																	 PORT_Pin_20|PORT_Pin_21|PORT_Pin_22|PORT_Pin_23);
	
  PORT_InitStructure.PORT_SOE    = PORT_SOE_OUT;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPD = PORT_SPD_OFF;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;

  PORT_Init(PORTC, &PORT_InitStructure);
	//---------------------------------------------------------------------------------------------endBLINK	
	
	/*PORT configuration*/
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_28 | PORT_Pin_29 | PORT_Pin_30);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_13;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTB, &PORT_InitStructure);	
		
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_13;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTC, &PORT_InitStructure);	
		
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_27 | PORT_Pin_28);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_13;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTD, &PORT_InitStructure);	
	//-------------
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
	MIL_STD_1553_CLK_en(MIL_STD_15532, MIL_STD_1553_CLKdiv1);
	

	/*---------- for use MIL 0 need settings ----------*/
/*---------- Terminal Mode ----------*/

	MIL_STD_15531->CONTROL = 1;	//reset MIL controller
	for(tmp = 0; tmp < 20; tmp++){}
		
	MIL_STD_15531->StatusWord1 = 0x0800;
		
		/*      CONTROL REG  | ENFILTER |  INVTR  | DIV[6:0] | ADDRESS|  TRB   |  TRA   |TERMINAL|WORKING */
	MIL_STD_15531->CONTROL = (1 << 20)|(1 << 19)|(80 << 11)|(1 << 6)|(1 << 5)|(1 << 4)|(2 << 2)|(0 << 0);
		
	for(tmp = 0; tmp < 1024; tmp++)
	{
		MIL_STD_15531->DATA[tmp] = 0x0;
	}
/*-------------------------------------------------*/
	/*---------- for use MIL 1 need settings ----------*/
/*---------- Bus Controller Mode ----------*/

	MIL_STD_15532->CONTROL = 1;	//reset MIL controller
	for(tmp = 0; tmp < 20; tmp++){}
	MIL_STD_15532->CONTROL = (1 << 20)|(1 << 19)|(80 << 11)|(1 << 2);	//Bus Controller Mode////////0<<19
		
//		MIL_STD_15532->StatusWord1 = 0x0800;
//		MIL_STD_15532->CONTROL = (1 << 20)|(1 << 19)|(80 << 11)|(1 << 6)|(1 << 5)|(1 << 4)|(2 << 2)|(0 << 0);	//Terminal Mode
		
	for(tmp = 0; tmp < 1024; tmp++)
	{
		MIL_STD_15532->DATA[tmp] = 0;
	}
/*-------------------------------------------------*/	

	/*Main cycle*/									
	while(1)
	{
		
		StartTRM(1, 1, 1, 1);
		
		while((MIL_STD_15532->CONTROL & 2) != 0)
		{
			for(tmp = 0; tmp < 16; tmp++){}
		}
			
		tmp = MIL_STD_15532->STATUS;
		if((tmp & 4) != 0)	//Valmess flag set
		{
			PORTC->CRXTX = (1 << 23);	//reset error
			PORTC->SRXTX = (1 << 16);	//set valmess
		}
		if((tmp & 8) != 0)	//Error flag set
		{
			PORTC->CRXTX = (1 << 16);	//reset valmess
			PORTC->SRXTX = (1 << 23);	//set error
		}
	
		for(tmp = 0; tmp < 100000; tmp++){}	//pause
		
	}
}

void BlinkLine(uint32_t Pin)
{
	volatile uint32_t t = 0;
	volatile uint32_t inc = 0;
	
		PORT_SetBits(PORTC, Pin);
		for (t=0;t<100000;t++) inc=inc+1;
		PORT_ResetBits(PORTC, Pin);
		for (t=0;t<100000;t++) inc=inc+1;
}

/*
TermAddr	- terminal address to rcv/trm data
Format		- packet format to trm/rcv
Chnl			- channel (main/reserve) to trm/rcv
Code			- command or number of words in transaction
*/
void StartTRM(uint32_t TermAddr, uint32_t Format, uint32_t Chnl, uint32_t Code)
{
	uint32_t t;
	
	for(t = 0; t < 32; t++)
	{
		MIL_STD_15532->DATA[32+t] = t;
	}
	
	MIL_STD_15532->CommandWord1 = 0x0825;
//	MIL_STD_15532->CommandWord1 = 0x0820;
	MIL_STD_15532->CommandWord2 = 0;
	
	MIL_STD_15532->CONTROL |= (1 << 4)|(1 << 1);
}

//-----------------------------assert_param--------------------------------
#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

/**
  * @}
  */ 	

/**
  * @}
  */ 	

/**
  * @}
  */ 	

/******************* (C) COPYRIGHT 2015 Milandr *********/

/* END OF FILE main.c */

