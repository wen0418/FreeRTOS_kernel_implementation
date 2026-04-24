# FreeRTOS Kernel Implementation

This project is a step-by-step study and implementation of the FreeRTOS kernel mechanisms from scratch. By breaking down the FreeRTOS source code into independent modules, it aims to provide a deep understanding of how a Real-Time Operating System (RTOS) works under the hood.

## Project Structure & Milestones

The project divides the core features of FreeRTOS into different stages, with each directory representing a specific learning milestone:

* **`01_list_implementation/`**
  * **Objective:** Implement the most fundamental data structure in FreeRTOS—the Doubly Linked List.
  * **Details:** Includes the initialization of `List_t` and `ListItem_t`, and the item-value-based ascending insertion mechanism. This is the cornerstone for task scheduling and state management in the OS.

* **`02_task_create/`**
  * **Objective:** Implement the Task Control Block (TCB) and static task creation.
  * **Details:** Covers task stack initialization, hardware-specific register saving mechanisms (porting), and the initial definition of the task structure.

* **`03_critical_section/`**
  * **Objective:** Implement Critical Sections to safely protect shared resources and core OS data structures.
  * **Details:** Covers the mechanisms to safely disable and enable hardware interrupts (e.g., `taskENTER_CRITICAL()` and `taskEXIT_CRITICAL()`). This includes implementing nesting counters to prevent premature interrupt re-enabling during nested function calls, ensuring atomic operations within the kernel.

* **`04_idle_task_and_delay/`**
  * **Objective:** Introduce the system Tick interrupt, non-blocking delays, and the background Idle Task.
  * **Details:** Includes setting up the hardware timer (like SysTick) to generate periodic tick interrupts. It covers implementing `vTaskDelay()` to move tasks to a Delayed List for time-management, and creating the Idle Task to ensure the processor always has a valid thread to execute when all user tasks are in a blocked state.

* **`05_priority/`**
  * **Objective:** Upgrade the system to a priority-based preemptive scheduler.
  * **Details:** Involves restructuring the Ready List into an array of lists (one for each priority level). Covers updating the context switch logic (`vTaskSwitchContext`) to always identify and select the highest-priority ready task, and handling preemption mechanics triggered by system ticks or task state changes.

## Development Environment

* **IDE:** Keil MDK (uVision)
* **Compiler:** ARM Compiler 6 (AC6)
* **Target Hardware:** STM32F103RB (ARM Cortex-M3)
* **Porting Architecture:** RVDS/ARM_CM3

---
*This project is a personal record of studying and validating the FreeRTOS source code.*
