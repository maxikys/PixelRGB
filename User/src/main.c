/**
  ******************************************************************************
  * @file    main.c
  * @author  Ткаченко Максим
  * @version V1.0.0
  * @date    02.05.19
  * @brief   
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include  "main.h"




/* Definitions of varable */


const TPin DIR          =   {GPIOA,1}; // направление для драйвера RS485, 0 - прием, 1 - передача
const TPin PinGreen     =   {GPIOA,6}; // ножка шим TIM3_CH1 - зеленый
const TPin PinRed       =   {GPIOA,7}; // ножка шим TIM3_CH2 - красный
const TPin PinBlue      =   {GPIOB,1}; // ножка шим TIM3_CH1 - синий

const TPin TX1          =   {GPIOA,9};
const TPin RX           =   {GPIOA,10};

const TPin A0           =   {GPIOA,0}; // для отладки
const TPin A2           =   {GPIOA,2}; // для отладки
const TPin A3           =   {GPIOA,3}; // для отладки
const TPin A4           =   {GPIOA,4}; // для отладки

const TPin* TestPort[]  =   {&A0, &A2, &A3, &A4};
/*-------------------------------------------------------------*/


void Mimi(void)
{
   TogglePin(&A4);
}

int main(void)
{
   ClockInit(HSI,8000000,48000000);
   SystickInit(1000);                                                         // Инициилизация Systick
   ConfigVirualPort(TestPort,4,OUT,_2MHz,NOPULL,PP);
   
   pwm_Init(0); // красный
   pwm_Set(0,100);
   
   pwm_Init(1); // зеленый
   pwm_Set(0,200);
   
   pwm_Init(2); // синий
   pwm_Set(0,300);
   
   SystickAddFunction(5, &Mimi);
   
   while (1)
   {
    SystickScanEvent(); //функция сканирования событий Systick
   }
}

