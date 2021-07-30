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
SSP_InitTypeDef SSP_InitStructure;

volatile uint32_t ch = 0;
volatile uint32_t c = 0;

uint32_t dat;
			
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
	
	CLKCTRL_CPU_PLLconfig(0, CLKCTRL_PLLn_CLK_SELECT_HSE0div1, 0, 8);//PLL, SRC, Q, N
  if(CLKCTRL_CPU_PLLstatus(0) == ERROR)
  while(1);
	
	CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_PLL0);			
	//CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_HSE0div1);	

	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTB_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTC_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTD_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTE_EN, ENABLE);
	CLKCTRL_PER1_CLKcmd(CLKCTRL_PER1_CLK_MDR_SSP0_EN, ENABLE);
	
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

  SSP_InitStructure.SSP_SCR  = 0x10;
  SSP_InitStructure.SSP_CPSDVSR = 2;
  SSP_InitStructure.SSP_Mode = SSP_ModeMaster;
  SSP_InitStructure.SSP_WordLength = SSP_WordLength16b;
  SSP_InitStructure.SSP_SPH = SSP_SPH_1Edge;
  SSP_InitStructure.SSP_SPO = SSP_SPO_Low;
  SSP_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
  SSP_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
  SSP_Init (MDR_SSP0,&SSP_InitStructure);

	/* Enable SSP0 */
  SSP_Cmd(MDR_SSP0, ENABLE);
	
	/*Main cycle*/									
	while (1)
	{
		while (SSP_GetFlagStatus(MDR_SSP0, SSP_FLAG_TFE) == RESET);
		SSP_SendData(MDR_SSP0, 0x35);
		
		dat = 0;
		BlinkLine(PORT_Pin_17);
		BlinkLine(PORT_Pin_18);

		while (SSP_GetFlagStatus(MDR_SSP0, SSP_FLAG_RNE) == RESET);
		dat = SSP_ReceiveData(MDR_SSP0);
		
		BlinkLine(PORT_Pin_17);
		BlinkLine(PORT_Pin_18);
	}
}

void BlinkLine(uint32_t Pin)
{
	volatile uint32_t t = 0;
	volatile uint32_t inc = 0;
	
		PORT_SetBits(PORTC, Pin);
		for (t=0;t<10000;t++) inc=inc+1;
		PORT_ResetBits(PORTC, Pin);
		for (t=0;t<10000;t++) inc=inc+1;
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

