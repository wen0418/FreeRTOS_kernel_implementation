# Task Create
## Task structure & Task create
<img width="878" height="570" alt="image" src="https://github.com/user-attachments/assets/e8269dc0-2a89-432d-aa50-8637c8d29254" />

## Task scheduler
<img width="1251" height="562" alt="image" src="https://github.com/user-attachments/assets/f311b1b1-761c-42c7-81f1-23e2a9cdfbc3" />

### PendSV detail
PendSV is a kind of interrupt. so, if we want to trigger it. We have to set the register: \
(NOTE:portNVIC_INT_CTRL_REG is belong to SCB->ICSR)

<img width="428" height="140" alt="image" src="https://github.com/user-attachments/assets/eca69729-ddcc-43d9-85be-c928637c1147" />

### What is NVIC & SCB?

* NVIC

NVIC (Nested Vectored Interrupt Controller)\
The NVIC is essentially the traffic cop for all external interrupts coming into the microcontroller. It is tightly integrated with the processor core to ensure\extremely low latency (delays) when handling events.

Key Features of the NVIC:

Vectored Interrupts : When an interrupt occurs, the NVIC automatically fetches the memory address of the corresponding Interrupt Service Routine (ISR) from a "Vector Table." The software doesn't need to poll or execute complex logic to figure out what caused the interrupt.

Nested Interrupts : "Nested" means that if a higher-priority interrupt occurs while a lower-priority interrupt is currently being processed, the CPU will pause the lower-priority one, run the higher-priority one, and then return. The NVIC handles this preemption and context saving automatically in hardware.

Tail-Chaining : This is a brilliant performance optimization. If two interrupts are pending, the processor doesn't completely restore the system state (pop registers) after the first one finishes, only to immediately save the state (push registers) again for the second one. Instead, it skips the restore/save steps and jumps directly to the second ISR, saving significant clock cycles.

Late Arrival : If a higher-priority interrupt arrives exactly when the CPU is already in the process of saving the state for a lower-priority interrupt, the NVIC will seamlessly switch to fetching the ISR for the higher-priority interrupt without starting the context save process over again.

* SCB

SCB (System Control Block)\
While the NVIC handles external peripheral interrupts (like a button press, UART, or SPI data reception), the SCB acts as the master control panel for the internal processor core itself. It manages system exceptions, core configuration, and fault handling.

Key Features of the SCB:

System Exceptions Control : The SCB manages internal CPU exceptions. This includes System Service Calls (SVC), the system timer (SysTick), RTOS context switching requests (PendSV), and various critical system faults (HardFault, MemManage, BusFault, UsageFault).

Vector Table Offset Register (VTOR) : This register allows you to move the Vector Table to a different location in memory (Flash or RAM). This is absolutely crucial for writing Bootloaders (Over-The-Air updates, for example), where the bootloader and the main application have completely different interrupt vector tables.

Application Interrupt and Reset Control Register (AIRCR) : This register is used to trigger a software system reset. It is also used to configure "Priority Grouping," which decides how many bits of the interrupt priority level are used for preemption priority versus sub-priority.

Power Management : The SCB contains registers (like the System Control Register - SCR) that control the low-power modes of the microcontroller. It determines whether the CPU goes into a standard "Sleep" or a deeper "Deep Sleep" when instructions like Wait For Interrupt (WFI) or Wait For Event (WFE) are executed.

## Final result

<img width="1142" height="543" alt="image" src="https://github.com/user-attachments/assets/f0716f6e-e55d-4d7e-af72-832d7cbf6d53" />
