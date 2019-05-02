/**
  ******************************************************************************
  * @file    main.c
  * @author  �������� ������
  * @version V1.0.0
  * @date    02.05.19
  * @brief   
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include  "main.h"




/* Definitions of varable */


const TPin DIR          =   {GPIOA,1}; // ����������� ��� �������� RS485, 0 - �����, 1 - ��������
const TPin PinGreen     =   {GPIOA,6}; // ����� ��� TIM3_CH1 - �������
const TPin PinRed       =   {GPIOA,7}; // ����� ��� TIM3_CH2 - �������
const TPin PinBlue      =   {GPIOB,1}; // ����� ��� TIM3_CH1 - �����

const TPin TX1          =   {GPIOA,9};
const TPin RX           =   {GPIOA,10};

const TPin A0           =   {GPIOA,0}; // ��� �������
const TPin A2           =   {GPIOA,2}; // ��� �������
const TPin A3           =   {GPIOA,3}; // ��� �������
const TPin A4           =   {GPIOA,4}; // ��� �������

const TPin* TestPort[]  =   {&A0, &A2, &A3, &A4};
/*-------------------------------------------------------------*/


void Mimi(void)
{
   TogglePin(&A4);
}

int main(void)
{
   ClockInit(HSI,8000000,48000000);
   SystickInit(1000);                                                         // ������������� Systick
   ConfigVirualPort(TestPort,4,OUT,_2MHz,NOPULL,PP);
   
   pwm_Init(0); // �������
   pwm_Set(0,100);
   
   pwm_Init(1); // �������
   pwm_Set(0,200);
   
   pwm_Init(2); // �����
   pwm_Set(0,300);
   
   SystickAddFunction(5, &Mimi);
   
   while (1)
   {
    SystickScanEvent(); //������� ������������ ������� Systick
   }
}

