#include "app_servo.h"
#include "delay.h"
#include "pwm.h"

/* ============================================================
 * ??????
 *   ????:
 *   1. ????? (0.5s)
 *   2. ?????180° (1s)
 *   3. ????? (0.5s)
 *   4. ?????0° (1s)
 * ============================================================ */

void Servo_ClawOpen(void)
{
    TIM_SetCompare2(PIN_SERVO_CLAW_TIM, CLAW_CCR_OPEN);
}

void Servo_ClawClose(void)
{
    TIM_SetCompare2(PIN_SERVO_CLAW_TIM, CLAW_CCR_CLOSE);
}

void Servo_ArmSetAngle(uint16_t ccr_value)
{
    /* ?? CCR ??,?????? */
    if (ccr_value > 200) ccr_value = 200;
    if (ccr_value < 0) ccr_value = 0;
    TIM_SetCompare2(PIN_SERVO_ARM_TIM, ccr_value);
}

void Servo_ArmTo0(void)
{
    Servo_ArmSetAngle(SERVO_CCR_0DEG);
}

void Servo_ArmTo90(void)
{
    Servo_ArmSetAngle(SERVO_CCR_90DEG);
}

void Servo_ArmTo180(void)
{
    Servo_ArmSetAngle(SERVO_CCR_180DEG);
}

void Servo_ExecuteGrabSequence(void)
{
    /* ??1: ????? (????) */
    Servo_ClawClose();
    delay_ms(ACTION_DELAY_GRAB);

    /* ??2: ?????180° (????) */
    Servo_ArmTo180();
    delay_ms(ACTION_DELAY_LIFT);

    /* ??3: ????? (????????) */
    Servo_ClawOpen();
    delay_ms(ACTION_DELAY_GRAB);

    /* ??4: ?????0° (???????) */
    Servo_ArmTo0();
    delay_ms(ACTION_DELAY_LIFT);
}
