from pyb import millis
from math import pi, isnan

class PID:
    def __init__(self, p=0, i=0, d=0, imax=0, cutoff_freq=20):
        self._kp = float(p)
        self._ki = float(i)
        self._kd = float(d)
        self._imax = abs(imax)
        self._integrator = 0
        
        # 低通滤波系数（用于微分项降噪）
        self._RC = 1 / (2 * pi * cutoff_freq) if cutoff_freq > 0 else 0
        
        self._last_error = 0
        self._last_derivative = float('nan')
        self._last_t = 0

    def get_pid(self, error, scaler):
        tnow = millis()
        dt = tnow - self._last_t
        self._last_t = tnow

        # 如果 dt 太大或刚启动，跳过本次积分/微分，只给比例
        if dt == 0 or dt > 100:
            return error * self._kp * scaler

        delta_time = dt / 1000.0  # 秒

        # 1. 比例项
        output = error * self._kp

        # 2. 微分项（带低通滤波）
        if abs(self._kd) > 0:
            if isnan(self._last_derivative):
                derivative = 0
            else:
                derivative = (error - self._last_error) / delta_time
            
            # 一阶低通滤波 (防止高频噪声放大)
            alpha = delta_time / (self._RC + delta_time) if self._RC > 0 else 1
            self._last_derivative = self._last_derivative + alpha * (derivative - self._last_derivative)
            output += self._kd * self._last_derivative

        # 3. 积分项（带抗积分饱和）
        if abs(self._ki) > 0:
            self._integrator += error * self._ki * delta_time
            # 钳位
            if self._integrator > self._imax: self._integrator = self._imax
            elif self._integrator < -self._imax: self._integrator = -self._imax
            output += self._integrator

        # 更新误差（移到这里，保证微分和积分用的是同一个误差）
        self._last_error = error

        # 输出限幅 (通过 scaler)
        output *= scaler
        if output > 100: output = 100
        elif output < -100: output = -100
        return output

    def reset_I(self):
        self._integrator = 0
        self._last_derivative = float('nan')
        self._last_error = 0  # ✅ 重置，防止微分冲击