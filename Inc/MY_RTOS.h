/*
 * MY_RTOS.h
 *
 *  Created on: Nov 13, 2025
 *      Author: BOLO
 */

#ifndef MY_RTOS_H_
#define MY_RTOS_H_

//=====================================================================================================================
//=====================================================================================================================


//=====================================================================================================================
#include <inttypes.h>
#include <stdbool.h>
//=====================================================================================================================

//----------------------------
//---  HOW TO USE MY_RTOS  ---
//----------------------------
//------------------------------------------------------------------------
// Default MY_RTOS working on 16MHz CPU internal clock
// and 1 KHz task scheduler. With 4 tasks running for led blinking exaple.
//
//                   *** WITHOUT FPU USAGE !!! ***
//                   *** WITHOUT FPU USAGE !!! ***
//                   *** WITHOUT FPU USAGE !!! ***
//
// If you want run MY_RTOS with the other configurations and purposes
// you should change or sometimes add any declarations/definitions
// also functions of tasks you should declare and do as described below:
//------------------------------------------------------------------------
// in the MY_RTOS.h file (this file) you need:
//--------------------
// 1. Set the definition F_CPU in Hz which you'll use CPU clock frequency
// 2. Set the definition COUNT_VALUE how you want scheduler frequency
// 3. Set the definition PSP_STACK to the end of RAM in used MCU
// 5. Set the definition MANY_TASKS of how many tasks you will use +1 as iddle task
// 5. Set the definition SYSTICK_PRIO systick handler priority, as you need if actual value isn't good for you
// 7. The definition of PENDSV_PRIO should have lowest (15) urgent priority it should be don't changed
//--------------------
// 8. At the definition TASKx_STACK_SIZE where x=No of task's and set desired value of
//    task private stack size for every one task you need
// -  and if you use more tasks than 15 you need add new definitions of TASKx_STACK_SIZE
// -  for example #define TASK5_STACK_SIZE    	1024
// 9. If you need more than 15 own tasks to run, you should add new tasks functions prototypes
// -  for example extern void task16_handler(void);
//---------------------
// In the MY_RTOS.c file you need:
//---
//10. array tasks delay values  ready/blocked state indicator 0 = READY ; != 0 = BLOCKED
// you should populate desire delay periods for used tasks for example 4 tasks:
// task_delay[MANY_TASKS] = {0,0,0,0,0}; - or uncomment dummy variables
// the first possition must be allways 0, it's for iddle_task which CAN NOT be blocked
//
// and set the below definition to lower stack pointer of last task for example:
// #define MSP_STACK           (uint32_t) (TASK5_PSP - TASK5_STACK_SIZE)
//
//---
// 11. in psp_of_tasks[MANY_TASKS] array you should uncoment following values up to used tasks
//     if you use more than 4 tasks or if you use more than 15 tasks you need create next
//     values as in the pattern
// 12. Like before in point 11 for task_handlers[MANY_TASKS] array
//---
//----------------------------------------------------------------------------
// In your main program file you should create a true tasks functions
// and system faults exceptions code if needed
//=====================================================================================================================
//=====================================================================================================================
#define F_CPU         		16000000        // True CPU frequency
#define COUNT_VALUE 	    (F_CPU / 1000)  // 1000 Hz tasks scheduler freq
#define PSP_STACK           ((0x20000000UL) + (128 * 1024) ) // set the end of RAM
// many tasks you need set
#define MANY_TASKS			7  // 6 tasks + 1 iddle thread
//------------------------------------------------------
#define SYSTICK_PRIO        15  // systick ISR priority
#define PENDSV_PRIO         15  // pendSV  ISR priority
#define SVC_PRIO            15  // SVC  ISR priority
//--------------------------------------------
//-- set stack size for everyone your task ---
//--------------------------------------------
#define TASK1_STACK_SIZE    	256  // set task stack size
#define TASK2_STACK_SIZE    	256  // set task stack size
#define TASK3_STACK_SIZE    	256  // set task stack size
#define TASK4_STACK_SIZE    	256  // set task stack size
#define TASK5_STACK_SIZE    	512  // set task stack size
#define TASK6_STACK_SIZE    	256    // set task stack size
#define TASK7_STACK_SIZE    	0    // set task stack size
#define TASK8_STACK_SIZE    	0    // set task stack size
#define TASK9_STACK_SIZE    	0    // set task stack size
#define TASK10_STACK_SIZE    	0    // set task stack size
#define TASK11_STACK_SIZE    	0    // set task stack size
#define TASK12_STACK_SIZE    	0    // set task stack size
#define TASK13_STACK_SIZE    	0    // set task stack size
#define TASK14_STACK_SIZE    	0    // set task stack size
#define TASK15_STACK_SIZE    	0    // set task stack size


//---------------------------------------------
//---
#define TASK_IDLE_STACK_SIZE    128
//---
//=====================================================================================================================
extern void OS_start(void);           // run OS main function
extern void OS_delay(uint32_t delay); // delay OS function
extern void Block_Task(uint32_t task_no);    // function for still lock task
extern void Delay_Task(uint32_t task_no , uint32_t delay); // function for periodic block task
extern void Unblock_Task(uint32_t task_no);  // unblock required task
extern uint32_t handle_Queue1(void);
extern bool put_Queue1_task(uint32_t addrs) ;
extern void OS_schedule(void);

extern void iddle_task(void); // iddle task
extern void task1_handler(void); // tasks handlers 1-4 functions prototypes
extern void task2_handler(void); // need declare other tasks prototypes
extern void task3_handler(void);
extern void task4_handler(void);
//---
extern void task5_handler(void); // dummy prototypes
extern void task6_handler(void);
extern void task7_handler(void);
extern void task8_handler(void);
extern void task9_handler(void);
extern void task10_handler(void);
extern void task11_handler(void);
extern void task12_handler(void);
extern void task13_handler(void);
extern void task14_handler(void);
extern void task15_handler(void);

//=====================================================================================================================
extern uint32_t my_utoa(uint8_t * buf, uint32_t data);
extern uint32_t my_itoa(uint8_t * buf, uint32_t data);
extern void my_htoa32(uint8_t * buf, uint32_t data);
extern void my_htoa64(uint8_t * buf, uint64_t data);
extern void set_LF(uint32_t * buff_ad);
extern void config_IRQ_PRIO(uint8_t irq_no, uint8_t priority_value);
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

typedef struct  {
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t COUNT;
} SYS_TICK;
#define SYSTICK_BASE 		(0xE000E010UL)
#define SYSTICK             ((SYS_TICK   *)     SYSTICK_BASE  )

//---
#define TASK_IDLE_PSP       (uint32_t) PSP_STACK
#define TASK1_PSP           (uint32_t) (TASK_IDLE_PSP - TASK_IDLE_STACK_SIZE)
#define TASK2_PSP           (uint32_t) (TASK1_PSP  - TASK1_STACK_SIZE)
#define TASK3_PSP           (uint32_t) (TASK2_PSP  - TASK2_STACK_SIZE)
#define TASK4_PSP           (uint32_t) (TASK3_PSP  - TASK3_STACK_SIZE)
#define TASK5_PSP           (uint32_t) (TASK4_PSP  - TASK4_STACK_SIZE)
#define TASK6_PSP           (uint32_t) (TASK5_PSP  - TASK5_STACK_SIZE)
#define TASK7_PSP           (uint32_t) (TASK6_PSP  - TASK6_STACK_SIZE)
#define TASK8_PSP           (uint32_t) (TASK7_PSP  - TASK7_STACK_SIZE)
#define TASK9_PSP           (uint32_t) (TASK8_PSP  - TASK8_STACK_SIZE)
#define TASK10_PSP          (uint32_t) (TASK9_PSP  - TASK9_STACK_SIZE)
#define TASK11_PSP          (uint32_t) (TASK10_PSP - TASK10_STACK_SIZE)
#define TASK12_PSP          (uint32_t) (TASK11_PSP - TASK11_STACK_SIZE)
#define TASK13_PSP          (uint32_t) (TASK12_PSP - TASK12_STACK_SIZE)
#define TASK14_PSP          (uint32_t) (TASK13_PSP - TASK13_STACK_SIZE)
#define TASK15_PSP          (uint32_t) (TASK14_PSP - TASK14_STACK_SIZE)

#define MSP_STACK           (uint32_t) (TASK6_PSP - TASK6_STACK_SIZE)
//---
#define DUMMY_XPSR 			0x01000000U // THUMB STATE for Cortex-M
//---
extern uint32_t task_delay[] ;      // tasks delay value and ready/blocked state indicator 0 = READY ; != 0 = BLOCKED
extern uint32_t psp_of_tasks[] ;    // actual stack pointer value of task
extern uint32_t task_handlers[] ;   // PC - continue program counter value of preempted task
//---

//---
extern uint32_t tck_cnt  ;          // tick counter
extern uint32_t LR_VALUE ;          // LR link register temporary value
extern uint32_t current_task ;      // task IDDLE is running
//---
//=====================================================================================================================
//=====================================================================================================================

#endif /* MY_RTOS_H_ */
