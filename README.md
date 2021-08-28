## Newmaker战队 Robomaster 2021赛季工程代码

### 依赖工具：keil、Stm32CubeMx

### 软件环境：window10 

### 硬件环境：STM32F427 （Robomaster A板）

### 编译方式：C编译

### 文件目录结构及文件用途说明:
CubeMx构建的代码框架，运用FreeRTOS系统进行任务的调配   
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/CubeMx.jpg)

硬件与操作系统通信库。例如遥控器DBUS通信、CAN通信等配置函数
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/BSP.jpg)

通用函数调用库
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/Libraries.jpg)

裁判系统数据以及键鼠
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/Referee.jpg)

传感器调用文件
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/Peripheral.jpg)

任务文件。用于将各功能划分，条例有序集中到Centre中，再由系统进行调度。
![Doc](https://github.com/Yoyo-yi/2021_open_source_code/blob/main/Doc/Task.jpg)


