#ifndef __BOARD_H
#define __BOARD_H

#include "sys.h"

/* ============================================================
 * ???????
 * ============================================================ */

/* ?????? (STM32 ??) */
#define PIN_SERVO_CLAW          GPIO_Pin_1      // PA1  - TIM2_CH2 (???)
#define PIN_SERVO_CLAW_GPIO     GPIOA
#define PIN_SERVO_CLAW_TIM      TIM2

#define PIN_SERVO_ARM           GPIO_Pin_5      // PB5  - TIM3_CH2 (????)
#define PIN_SERVO_ARM_GPIO      GPIOB
#define PIN_SERVO_ARM_TIM       TIM3

/* ???? (STM32 UART1 ? OpenMV) */
#define PIN_USART1_TX           GPIO_Pin_9      // PA9
#define PIN_USART1_RX           GPIO_Pin_10     // PA10
#define PIN_USART1_GPIO         GPIOA

/* ============================================================
 * PWM ???? (20ms, 50Hz)
 *   T = (ARR+1) * (PSC+1) / 72MHz
 *   199,7199 -> 20ms
 * ============================================================ */
#define PWM_ARR                 199
#define PWM_PSC                 7199

/* ============================================================
 * ???? -> CCR ???
 *   ARR=199, ??=20ms, CCR ??: 0~199
 *   ?? 0°   -> 0.5ms ??? -> CCR = 5
 *   ?? 180° -> 2.5ms ??? -> CCR = 25
 *   (??:?????,????????????)
 * ============================================================ */
#define SERVO_CCR_0DEG          5       // 0.5ms  -> 0°
#define SERVO_CCR_45DEG         10      // 1.0ms  -> 45°
#define SERVO_CCR_90DEG         15      // 1.5ms  -> 90°
#define SERVO_CCR_135DEG        20      // 2.0ms  -> 135°
#define SERVO_CCR_180DEG        25      // 2.5ms  -> 180°

/* ????? (PWM2??,?????) */
#define CLAW_CCR_OPEN           5       // ??
#define CLAW_CCR_CLOSE          15      // ??

/* ============================================================
 * ?????? (? OpenMV ????)
 *   OpenMV ?? '1' ????
 * ============================================================ */
#define CMD_GRAB                '1'     // ASCII 49 - ????

/* ============================================================
 * ?????? (??)
 * ============================================================ */
#define ACTION_DELAY_GRAB       500     // ????/????
#define ACTION_DELAY_LIFT       1000    // ????/????

#endif /* __BOARD_H */
