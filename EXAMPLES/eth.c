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
#include "mdr32f8_eth.h"

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
#define SIZE 65
#define COUNT 48

#define ETH_BUF 0x21008000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PORT_InitTypeDef PORT_InitStructure;
ETH_InitTypeDef  ETH_InitStruct;

volatile uint32_t ch = 0;
volatile uint32_t c = 0;

uint32_t dat;
uint32_t tmp;

uint32_t hasErrorETH = 0;

uint32_t DstBuf[(SIZE-1)/4+7];
uint32_t SrcBuf[(SIZE-1)/4+7];
	
uint32_t *DstBuf_p = DstBuf;
uint32_t *SrcBuf_p = SrcBuf;
			
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void POR_disable();
void KEY_reg_accs();
void BlinkLine(uint32_t Pin);
void Ini_BUF(uint32_t addr,uint32_t size,uint32_t delta);
uint32_t TestETH();
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
		
int main(void)
{	
	uint32_t reg;
	
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
	
	//CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_PLL0);			
	CLKCTRL_MAX_CLKSelection(CLKCTRL_MAX_CLK_HSE0div1);	

	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_ENET0_EN, ENABLE);
	CLKCTRL_PER0_CLKcmd(CLKCTRL_PER0_CLK_MDR_PORTC_EN, ENABLE);

	ETH_ClockDeInit();
	ETH_PHY_ClockConfig(ETH_PHY_CLOCK_SOURCE_PLL0, ETH_PHY_HCLKdiv1);
	ETH_ClockCMD(ENABLE);
	
	/* Init ETH_InitStruct members with its default value */
	//ETH_StructInit((ETH_InitTypeDef * ) &ETH_InitStruct);

	MDR_ETH0->PHY_CONTROL  = (1 << 15)|(0x2 << 6);
	MDR_ETH0->PHY_CNTR_A   = (1 << 15)|(128 << 4)|(4 << 1)|(0 << 0);//phy en//1<<0//0<<1//0<<4
	MDR_ETH0->PHY_CONTROL &=~(1 << 15);
	
		ETH_InitStruct.ETH_Extension_Mode  = ENABLE;
		ETH_InitStruct.ETH_ColWnd 				 = 0x5;
		ETH_InitStruct.ETH_HalfDuplex_Mode = DISABLE;
		ETH_InitStruct.ETH_Buffer_Mode 		 = ETH_BUFFER_MODE_FIFO;
		ETH_InitStruct.ETH_Register_CLR 	 = DISABLE;
		ETH_InitStruct.ETH_Loopback_Mode   = DISABLE;
		ETH_InitStruct.ETH_DBG_XF          = ENABLE;
		ETH_InitStruct.ETH_DBG_RF          = ENABLE;
	
		ETH_InitStruct.ETH_Receive_All_Packets = ENABLE;
		ETH_InitStruct.ETH_Error_CRC_Frames_Reception = ENABLE;
		ETH_InitStruct.ETH_Receiver_Event_Mode = ETH_RECEIVER_EVENT_MODE_PACET_RECEIVED;
		ETH_InitStruct.ETH_Receiver_BE = ETH_RECEIVER_BE_BIG_ENDIAN;
		
		ETH_InitStruct.ETH_Transmitter_BE = ETH_TRANSMITTER_BE_BIG_ENDIAN;
		ETH_InitStruct.ETH_Transmitter_Event_Mode = ETH_TRANSMITTER_EVENT_MODE_XFIFO_ALMOST_EMPTY;
		ETH_InitStruct.ETH_Automatic_Pad_Strip = ENABLE;
		ETH_InitStruct.ETH_Automatic_Preamble = ENABLE;
		ETH_InitStruct.ETH_Automatic_CRC_Strip = ENABLE;
		ETH_InitStruct.ETH_Automatic_IPG = ENABLE;
		ETH_InitStruct.ETH_Retry_Counter = 0xF;	
		
	ETH_Init(MDR_ETH0, (ETH_InitTypeDef *) &ETH_InitStruct);

	MDR_ETH0->PHY_CONTROL|=(0 << 1)|(0 << 0);//after analog
	
	ETH_Start(MDR_ETH0);

 tmp = TestETH();
	
	/*Main cycle*/									
	while (1)
	{
	
	}
}

void Ini_BUF(uint32_t addr,uint32_t size,uint32_t delta)
{
  int i;
    
  //Fill data count
  *((uint32_t *)(addr+0))=16+SIZE;
  //fill adress
  *((uint32_t *)(addr+4))=0xFFFFFFFF;
  //fill adress
  *((uint32_t *)(addr+8))=0xFFFF0020;
  //fill adress
  *((uint32_t *)(addr+12))=0xAF000001;
  //fill length-type + 2 data byte
  *((uint32_t *)(addr+16))=0x08000000;
  //fill data
  for(i=0;i<=size/4;i++)
  {
    if (i==size/4)
    {
      switch ((size+1) % 4)
      {
        case 0:
        {
           *((uint32_t *)(addr+20+i*4)) = i<<24 | i<<16 | i<<8 | i<<0;
           break;
        }
        case 1:
        {
           *((uint32_t *)(addr+20+i*4)) = i<<24;
           break;
        }
        case 2:
        {
           *((uint32_t *)(addr+20+i*4)) = i<<24 | i<<16;
           break;
        }
        case 3:
        {
           *((uint32_t *)(addr+20+i*4)) = i<<24 | i<<16 | i<<8;
           break;
        }
      }
    }
    else
    {
      *((uint32_t *)(addr+20+i*4)) = i<<24 | i<<16 | i<<8 | i<<0;
    }
  }
  //fill word for status
  if (((size+1) % 4)!=0)
  {
    *((uint32_t *)(addr+20+(size/4)*4+4))=0x11111111;
  }
  else
  {
    *((uint32_t *)(addr+20+size+1))=0x11111111;
  }
}

uint32_t TestETH()
{
	int i,j;
	uint32_t var;
	uint32_t tmp;

	hasErrorETH = 0;
	
	for(j=0;j<1;j++)
  {  
	MDR_ETH0->IFR = 0xFFFF;
		
	Ini_BUF((volatile unsigned int)&SrcBuf,SIZE-1,j);
		
	  //fill tx buffer
    for(i=0;i<=((SIZE-1)/4+6);i++)
    {
      var = ETH_BUF+4;
      *((volatile unsigned int *)(var)) = SrcBuf[i];// 3;
    }

    while(((MDR_ETH0->IFR & 0x0001) != 0x0001))
    {

    }	
		
		//read rx data
    for(i=0;i<=((SIZE-1)/4+6);i++)
    {
      var = ETH_BUF;
      DstBuf[i] = *((volatile unsigned int *)(var));// 3;
			
								if (DstBuf[i]!=SrcBuf[i]) hasErrorETH++;
    }
	//DeInit_BUF((volatile unsigned int)&SrcBuf,SIZE-1,j);
	}//for
		
	if(hasErrorETH==0x3) return 0;
	else 							 return 1;

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

