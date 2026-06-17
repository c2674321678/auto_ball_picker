from pyb import Pin, Timer

inverse_left = False
inverse_right = False

# 电机A控制引脚 (左轮)
ain1 = Pin('P6', Pin.OUT_PP)
ain2 = Pin('P9', Pin.OUT_PP)

# 电机B控制引脚 (右轮)
bin1 = Pin('P2', Pin.OUT_PP)
bin2 = Pin('P3', Pin.OUT_PP)

# 初始化所有引脚为低电平
ain1.low()
ain2.low()
bin1.low()
bin2.low()

# PWM引脚
pwma = Pin('P7')
pwmb = Pin('P8')

# PWM定时器配置 (频率 1000Hz)
tim = Timer(4, freq=1000)
ch1 = tim.channel(1, Timer.PWM, pin=pwma)
ch2 = tim.channel(2, Timer.PWM, pin=pwmb)

# 初始占空比 0%
ch1.pulse_width_percent(0)
ch2.pulse_width_percent(0)


def run(left_speed, right_speed):
    """
    电机控制函数
    参数: left_speed  - 左轮速度 (-100 ~ 100)
          right_speed - 右轮速度 (-100 ~ 100)
    正数 = 正转, 负数 = 反转
    """
    # 方向反转 (如果需要)
    if inverse_left:
        left_speed = -left_speed
    if inverse_right:
        right_speed = -right_speed

    # ---- 左轮控制 ----
    if left_speed > 0:
        ain1.low()
        ain2.high()
    else:
        ain1.high()
        ain2.low()
    ch1.pulse_width_percent(int(abs(left_speed)))

    # ---- 右轮控制 ----
    if right_speed > 0:
        bin1.low()
        bin2.high()
    else:
        bin1.high()
        bin2.low()
    ch2.pulse_width_percent(int(abs(right_speed)))