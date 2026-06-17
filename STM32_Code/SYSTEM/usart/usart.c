#include "sys.h"
#include "usart.h"
#include "app_servo.h"
#include "board.h"   // ??:?? CMD_GRAB ?

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  

/* ============================================================
 * ??????
 *   ?? OpenMV ??? '1' ???????
 * ============================================================ */

#if 1
#pragma import(__use_no_semihosting)

struct __FILE {
    int handle;
};
FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);
    USART1->DR = (u8)ch;
    return ch;
}
#endif

#if EN_USART1_RX

/* ???? */
u8 USART_RX_BUF[USART_REC_LEN];
u16 USART_RX_STA = 0;

/* ???????? (??????????) */
static uint8_t grab_in_progress = 0;

void uart_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ???? */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO, ENABLE);

    /* TX ??: PA9 - ?????? */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* RX ??: PA10 - ???? */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ???? */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART ?? */
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/* ???????? */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t res = USART_ReceiveData(USART1);

        /* ---- ????:?? '1' ????? ---- */
        if (res == CMD_GRAB) {
            /* ???????????????? */
            if (grab_in_progress == 0) {
                grab_in_progress = 1;
                Servo_ExecuteGrabSequence();
                grab_in_progress = 0;
            }
        }

        /* ---- ??????? (?? 0x0d 0x0a ???) ---- */
        if ((USART_RX_STA & 0x8000) == 0) {
            if (USART_RX_STA & 0x4000) {
                if (res != 0x0a) {
                    USART_RX_STA = 0;
                } else {
                    USART_RX_STA |= 0x8000;
                }
            } else {
                if (res == 0x0d) {
                    USART_RX_STA |= 0x4000;
                } else {
                    USART_RX_BUF[USART_RX_STA & 0x3FFF] = res;
                    USART_RX_STA++;
                    if ((USART_RX_STA & 0x3FFF) >= USART_REC_LEN - 1) {
                        USART_RX_STA = 0;
                    }
                }
            }
        }
    }
}

#endif /* EN_USART1_RX */
