# Idle task & Delay

## IdleTask create

<img width="1226" height="405" alt="image" src="https://github.com/user-attachments/assets/a89cca1b-8862-48b0-9b11-5f5301f80a48" />

## Delay implementation

<img width="1278" height="559" alt="image" src="https://github.com/user-attachments/assets/1384aa22-1b3e-4a32-8d68-a8804991630d" />

## Why the `portNVIC_SYSTICK_LOAD_REG` set like this?
```
#define configCPU_CLOCK_HZ	 ((unsigned long) 25000000)
#define configTICK_RATE_HZ	 ((TickType_t) 100)
portNVIC_SYSTICK_LOAD_REG = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL; // the value put into LOAD_REG will be 249999
```
1. How do clock and rate work?\
`configCPU_CLOCK_HZ (CPU Clock)` = 25,000,000 Hz\
This is the heartbeat speed of the CPU. The microcontroller "ticks" 25,000,000 times per second (25MHz).\
`configTICK_RATE_HZ (OS Tick Rate)` = 100 Hz\
This is the desired "interrupt frequency" for the FreeRTOS operating system. The OS wants a SysTick interrupt to occur 100 times per second (which means checking for a context switch every 10 milliseconds).

2. Why devide and subtract by one?\
Systick is a hardware "count down" timer, with every single CPU tick, its value decreate by 1.When it reaches 0, it triggers an interrupt and automatically reloads the value stored in the LOAD_REG to start the next countdown cycle.\
Because The hardware Because Thecounts down from N to 0 (inclusive of 0).\
If you want it to count 3 ticks, you should set it to 2 (the countdown process: 2 -> 1 -> 0, which is exactly 3 actions).\
Therefore, if we want it to trigger after 250,000 ticks, we must set the Load register to 250,000 - 1 = 249,999.

## Final result

<img width="1142" height="543" alt="image" src="https://github.com/user-attachments/assets/1066fbfd-7805-45ea-9eaa-00749394499f" />
