#ifndef __PWM_H
#define __PWM_H

#include "sys.h"
#include "board.h"

/* ============================================================
 * PWM ????
 *   ?? TIM2_CH2 ? TIM3_CH2 ? PWM ??
 * ============================================================ */

/* ?? PWM ????? */
void PWM_Init(TIM_TypeDef* TIMx, uint8_t channel,
              uint16_t arr, uint16_t psc,
              GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* ??????? (????) */
void TIM2_PWM_Init(u16 arr, u16 psc);
void TIM3_PWM_Init(u16 arr, u16 psc);

#endif /* __PWM_H */
