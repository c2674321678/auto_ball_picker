#include "pwm.h"

/* ============================================================
 * PWM ????
 * ============================================================ */

/* ---------- ?? PWM ??? ---------- */
void PWM_Init(TIM_TypeDef* TIMx, uint8_t channel,
              uint16_t arr, uint16_t psc,
              GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 1. ???? */
    if (TIMx == TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    } else if (TIMx == TIM3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    }

    /* GPIO ???? */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO, ENABLE);

    /* 2. GPIO ????????? */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    /* 3. ??????? */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

    /* 4. PWM ???? */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    switch (channel) {
        case 1:
            TIM_OC1Init(TIMx, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
            break;
        case 2:
            TIM_OC2Init(TIMx, &TIM_OCInitStructure);
            TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
            break;
        case 3:
            TIM_OC3Init(TIMx, &TIM_OCInitStructure);
            TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
            break;
        case 4:
            TIM_OC4Init(TIMx, &TIM_OCInitStructure);
            TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
            break;
        default:
            break;
    }

    /* 5. ????? */
    TIM_Cmd(TIMx, ENABLE);
}

/* ---------- ????? ---------- */
void TIM2_PWM_Init(u16 arr, u16 psc)
{
    PWM_Init(TIM2, 2, arr, psc, GPIOA, GPIO_Pin_1);
}

void TIM3_PWM_Init(u16 arr, u16 psc)
{
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
    PWM_Init(TIM3, 2, arr, psc, GPIOB, GPIO_Pin_5);
}
