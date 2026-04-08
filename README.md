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

## Development Environment

* **IDE:** Keil MDK (uVision)
* **Compiler:** ARM Compiler 6 (AC6)
* **Target Hardware:** STM32F103RB (ARM Cortex-M3)
* **Porting Architecture:** RVDS/ARM_CM3

---
*This project is a personal record of studying and validating the FreeRTOS source code.*
