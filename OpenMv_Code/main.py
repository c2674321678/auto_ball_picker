from machine import UART
import sensor, image, time
import car
from pid import PID

# ============================================================
# 常量定义
# ============================================================
CMD_FORWARD  = 49   # '1' - 前进
CMD_LEFT     = 50   # '2' - 左转
CMD_RIGHT    = 51   # '3' - 右转
CMD_BACKWARD = 52   # '4' - 后退
CMD_GRAB     = 53   # '5' - 抓取
CMD_AUTO     = 54   # '6' - 切换到自动模式
CMD_STOP     = 55   # '7' - 停止

MODE_MANUAL = 0
MODE_AUTO   = 1

# ============================================================
# 初始化
# ============================================================
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(10)
sensor.set_auto_whitebal(False)

clock = time.clock()

# 串口: uart1 -> STM32, uart2 -> 蓝牙
uart_stm32 = UART(3, 9600)
uart_bt = UART(1, 9600)

# 颜色阈值 (绿色网球)
green_threshold = (36, 87, -34, -13, 17, 127)
size_threshold = 2000

# PID 控制器
x_pid = PID(p=0.5, i=0.01, imax=50)   # 方向PID
h_pid = PID(p=0.05, i=0.01, imax=30)  # 距离PID

# ============================================================
# 辅助函数
# ============================================================
def find_max(blobs):
    """找出面积最大的色块"""
    max_size = 0
    max_blob = None
    for blob in blobs:
        area = blob[2] * blob[3]
        if area > max_size:
            max_blob = blob
            max_size = area
    return max_blob


def do_auto_mode():
    """自动寻球模式 - 返回 True 表示切换到遥控模式"""
    grab_count = 0   # 用于计数，判断小车是否稳定到达目标

    while True:
        # ---- 检查是否收到切换命令 ----
        if uart_bt.any():
            cmd = uart_bt.readchar()
            print("BT CMD:", cmd)
            if cmd == CMD_STOP:        # '7' - 停止并切换回遥控
                car.run(0, 0)
                return MODE_MANUAL

        # ---- 视觉识别 ----
        clock.tick()
        img = sensor.snapshot()
        blobs = img.find_blobs([green_threshold])

        if blobs:
            max_blob = find_max(blobs)
            if max_blob is None:
                continue

            # 计算偏差
            x_error = max_blob[5] - img.width() / 2
            h_error = max_blob[2] * max_blob[3] - size_threshold

            # 图像标记
            img.draw_rectangle(max_blob[0:4])
            img.draw_cross(max_blob[5], max_blob[6])

            # PID 计算
            x_output = x_pid.get_pid(x_error, 1)
            h_output = h_pid.get_pid(h_error, 1)

            print("x_err:", x_error, "h_err:", h_error, "h_out:", h_output)

            # 驱动电机
            car.run(-h_output - x_output, -h_output + x_output)

            # 判断是否到达目标 (稳定在目标位置)
            if (abs(h_output) < 5) and (abs(x_error) < 3):
                grab_count += 1
                print("稳定计数:", grab_count)
            else:
                grab_count = 0  # 偏离目标则重置计数

            # 连续稳定 55 帧后触发抓取
            if grab_count >= 55:
                print("触发抓取!")
                uart_stm32.write("1")
                grab_count = 0
                time.sleep_ms(2000)  # 等待 STM32 完成抓取
                car.run(0, 0)        # 抓取时停车

        else:
            # 没看到网球，原地旋转搜索
            car.run(18, -18)
            grab_count = 0  # 没看到球则重置计数


def do_manual_mode():
    """遥控模式 - 返回 True 表示切换到自动模式"""
    if uart_bt.any():
        cmd = uart_bt.readchar()
        print("BT CMD:", cmd)

        if cmd == CMD_FORWARD:     # '1' - 前进
            car.run(30, 30)
        elif cmd == CMD_LEFT:      # '2' - 左转
            car.run(20, -20)
        elif cmd == CMD_RIGHT:     # '3' - 右转
            car.run(-20, 20)
        elif cmd == CMD_BACKWARD:  # '4' - 后退
            car.run(-25, -25)
        elif cmd == CMD_GRAB:      # '5' - 抓取
            uart_stm32.write("1")
            time.sleep_ms(2000)    # 等待 STM32 完成抓取
        elif cmd == CMD_AUTO:      # '6' - 切换到自动模式
            car.run(0, 0)
            return MODE_AUTO
        elif cmd == CMD_STOP:      # '7' - 停止
            car.run(0, 0)

    return MODE_MANUAL


# ============================================================
# 主循环
# ============================================================
mode = MODE_MANUAL

while True:
    if mode == MODE_MANUAL:
        mode = do_manual_mode()
    else:  # MODE_AUTO
        mode = do_auto_mode()