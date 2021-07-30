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

volatile uint32_t ch = 0;
volatile uint32_t c = 0;
uint32_t i = 0;

uint32_t TestData[32] = { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
            						 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8 };

												 
					 uint32_t ReadArr_ch4[32];		
					 uint32_t ReadArr_ch5[32];				
					 uint32_t ReadArr_ch6[32];		
					 uint32_t ReadArr_ch7[32];	

uint32_t dat;
uint32_t mes_l;
uint32_t mes_h;
			
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void POR_disable();
void KEY_reg_accs();
void BlinkLine(uint32_t Pin);

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

	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTB_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTC_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTD_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTE_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_ARCRX0_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_ARCTX0_EN, ENABLE);
	
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
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_14);//out
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_11;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTE, &PORT_InitStructure);	
		
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_15);
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_11;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTB, &PORT_InitStructure);	
		
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_12 | PORT_Pin_13);//in
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_11;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTE, &PORT_InitStructure);	
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_10 | PORT_Pin_11);//in
	PORT_InitStructure.PORT_SFUNC  = PORT_SFUNC_11;
  PORT_InitStructure.PORT_SANALOG  = PORT_SANALOG_DIGITAL;
	PORT_InitStructure.PORT_SPWR = PORT_SPWR_10;
  PORT_Init(PORTE, &PORT_InitStructure);
		
	ARINC_CLK_en(ARC0_CLKdiv1);

	/* ARINC429R channel 1 configuration */
	ARINC429R_InitChannelStruct.ARINC429R_CLK = ARINC429R_CLK_100_KHz;
	ARINC429R_InitChannelStruct.ARINC429R_SD = DISABLE;
	ARINC429R_InitChannelStruct.ARINC429R_LB = DISABLE;
	ARINC429R_InitChannelStruct.ARINC429R_SDI1 = RESET;
	ARINC429R_InitChannelStruct.ARINC429R_SDI2 = RESET;
	ARINC429R_InitChannelStruct.ARINC429R_DIV = 5;
	ARINC429R_ChannelInit(ARINC429R_CHANNEL7, &ARINC429R_InitChannelStruct);
	/* Enables ARINC429R channel 1 */
	ARINC429R_ChannelCmd(ARINC429R_CHANNEL7, ENABLE);
	
	/* ARINC429T channel 1 configuration */
	ARINC429T_InitChannelStruct.ARINC429T_CLK = ARINC429T_CLK_100_KHz;
	ARINC429T_InitChannelStruct.ARINC429T_DIV = 5;
	//ARINC429T_InitChannelStruct.ARINC429T_EN_PAR = ENABLE;
	//ARINC429T_InitChannelStruct.ARINC429T_ODD = ARINC429T_ODD_ADD_TO_ODD;
	ARINC429T_ChannelInit(ARINC429T_CHANNEL4, &ARINC429T_InitChannelStruct);
	
			ARINC429R1->CONTROL8=0x00000000;	

	/* Enables ARINC429T channel 1 */
	ARINC429T_ChannelCmd(ARINC429T_CHANNEL4, ENABLE);
	
	
	for(i = 0; i < 32; i++){
	//	while(((ARINC429T1->STATUS)&0x00000008)!=1){}
	//HWREG(ARINC429TD1)=TestData[i];
		
//	ARINC429T1->DATA3_T=TestData[i];
	ARINC429T1->DATA4_T=TestData[i];
		
	BlinkLine(PORT_Pin_17);
			BlinkLine(PORT_Pin_17);
		
					ARINC429R1->CHANNEL=0x00000006;
						ReadArr_ch6[i]=ARINC429R1->DATA_R;
		
//					ARINC429R1->CHANNEL=0x00000007;
//						ReadArr_ch7[i]=ARINC429R1->DATA_R;
	}
	/*Main cycle*/									
	while (1);
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

