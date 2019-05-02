#include "DMX_receive.h"
/*---------------Extern difinition------------------*/

extern TPin A0;
extern TPin A2;
extern TPin A3;
extern TPin A4;

/*--------------------------------------------------*/
union DMX_Flag {
    struct {
        unsigned char dmx__wasBREAK: 1;
        unsigned char dmx__wasSCode: 1;
        unsigned char dmx_syncBREAK: 1;
        unsigned char dmx_error: 1;
           } bit;
    unsigned char byte;
};

// обьявляем
union DMX_Flag Flag;
// 

uint8_t tempISRhi,dmx__chNum=0,maxChNum=3,i=0;
uint16_t dmx__curAdr=0,dmx_baseAdr=4,count_break=0,DMX_Array[3];
uint32_t del=0;
uint8_t TxBuffer[3]={85,255,15}, i_ch=0,i_pl=0;


/* настройка вектора прерывания */
void NVIC_ini(void)
{  
   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}

/*-----------------------------------Обработка прерывания----------------------------------*/
void USART1_IRQHandler()
{
   if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)       // пришел ли байт
   {
      TogglePin(&A0);                                             // Отладка
      if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == RESET)   // отсутствует ошибка переполнения
      {
         if (USART_GetFlagStatus(USART1, USART_FLAG_FE) == RESET) // Отсутствие флага обнаружения Break
         {
            tempISRhi=USART_ReceiveData(USART1);
            if(Flag.bit.dmx__wasBREAK == 1)
            {
               if(Flag.bit.dmx__wasSCode == 1)
               {
                  if(dmx__curAdr >= dmx_baseAdr)
                  {
                     DMX_Array[dmx__chNum] = tempISRhi;
                     dmx__chNum++;
                     if(dmx__chNum>=maxChNum)
                     {
                        Flag.bit.dmx__wasBREAK=0;
                        Flag.bit.dmx_syncBREAK=1;
                        Flag.bit.dmx__wasSCode=0 ;
                        //Enable_output();
//                        for(i_pl = 0;i_pl < 16; i_pl++)
//                        {
//                           SPI_I2S_SendData16(SPI1, 0);// шлем данные в 32 канал 
//                           while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET){}
//                           for(i_ch = 30;i_ch >=16; --i_ch)// шлем данные с 30 по  
//                           {
//                              SPI_I2S_SendData16(SPI1, (uint16_t)DMX_Array[i_ch-1]);
//                              while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET){}		 
//                           }
//                           LE_high();
//                           SPI_I2S_SendData16(SPI1, 0);
//                           while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET){}	
//                           for(i_ch = 15; i_ch >= 1; --i_ch)
//                           {
//                              SPI_I2S_SendData16(SPI1, (uint16_t)DMX_Array[i_ch-1]);
//                              while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET){}		 
//                           }
//                           LE_low(); 
//                        }
                     };
                  }
                  else
                     {
                        dmx__curAdr++;
                     };
               }
               //Проверка на стартовый код
               else
                  {
                     if(tempISRhi == 0x00)
                     {
                        TogglePin(&A2);               // Отладка
                        Flag.bit.dmx__wasSCode=1;
                        Flag.bit.dmx_error=0;
                     }
                     else Flag.bit.dmx__wasBREAK=0;
                  };
            };
         }
         // Сброс флага обнаружения Break
         else
            {
               USART_ClearFlag(USART1,  USART_FLAG_FE);
               Flag.bit.dmx__wasBREAK=1;
               Flag.bit.dmx__wasSCode=0;
               dmx__curAdr=0;
               dmx__chNum=0;
               TogglePin(&A3);               // Отладка
               count_break++;
            };
      }
      // Сброс флага переполнения
      else
         {
            USART_ClearFlag(USART1,  USART_FLAG_ORE);
            Flag.bit.dmx__wasBREAK=0;
            Flag.bit.dmx_error=1;
            tempISRhi=USART_ReceiveData(USART1);
         };
   }
}

void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//RCC_AHBPeriph_GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	//GPIO configuration for Rx and Tx
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	 
  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 250000 baud
        - Word Length = 8 Bits
        - two Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive  enabled
  */
  USART_InitStructure.USART_BaudRate = 250000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
 
  USART_Init(USART1, &USART_InitStructure);
 
  USART_Cmd(USART1, ENABLE);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//разрешение прерывания USART1 заболнению буфера
   
   NVIC_ini();                                   // прерывания
}
