# ESP-detect-Apple-Watch
Using an ESP32S3 BLE to detect apple watch,auto screen locker (ESP32 is also ok).If applewatch Present then turn on the light.Else turn off the light and lock the computer screen.
用ESP32S3的BLE检测applewatch 如果检测到则开灯 否则关灯 同时给电脑锁屏。采用BLE扫描applewatch的Manufacturer数据包。
#  使用方法
继电器接38引脚。BOOT按钮按下后可以强制关灯，再次按下可以强制开灯（超时两分钟）。LED通过颜色指示状态。绿色：开灯 红色：检测不到手表并且关灯 蓝色：已强制关灯。
