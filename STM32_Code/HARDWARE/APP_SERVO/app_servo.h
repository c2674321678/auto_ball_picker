#ifndef __APP_SERVO_H
#define __APP_SERVO_H

#include "sys.h"
#include "board.h"

/* ============================================================
 * ?????? API
 * ============================================================ */

/* ????? */
void Servo_ClawOpen(void);      // ??
void Servo_ClawClose(void);     // ??

/* ?????? */
void Servo_ArmSetAngle(uint16_t ccr_value);   // ?????? (CCR?)
void Servo_ArmTo0(void);        // ??? 0°
void Servo_ArmTo90(void);       // ?? 90°
void Servo_ArmTo180(void);      // ?? 180°

/* ?????? */
void Servo_ExecuteGrabSequence(void);   // ?? -> ?? -> ??

#endif /* __APP_SERVO_H */
