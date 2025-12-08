/*
 * MY_RTOS.c
 *
 *  Created on: Nov 13, 2025
 *      Author: BOLO
 */

#include "MY_RTOS.h"
#include <inttypes.h>

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

//----------------------------------------------------------------
//--- below 3 variables arrays should be modified according to ---
//---      MANY_TASKS definition in RTOS.h file value          ---
//--- also should be created if need tasks functions and prot. ---
//----------------------------------------------------------------

// tasks delay value and ready/blocked state indicator 0 = READY ; != 0 = BLOCKED
uint32_t task_delay[MANY_TASKS] = {0,
		                           0,0,0,0, 0xFFFFFFFF, 0xFFFFFFFF,
//								   0,0,0,
//								   0,0,0,0,
//								   0,0,
};
// actual stack pointer value of task
uint32_t psp_of_tasks[MANY_TASKS] = { TASK_IDLE_PSP,
		                              TASK1_PSP,
									  TASK2_PSP,
									  TASK3_PSP,
									  TASK4_PSP,
		                              TASK5_PSP,
									  TASK6_PSP,
//									  TASK7_PSP,
//									  TASK8_PSP,
//		                              TASK9_PSP,
//									  TASK10_PSP,
//									  TASK11_PSP,
//									  TASK12_PSP,
//		                              TASK13_PSP,
//									  TASK14_PSP,
//									  TASK15_PSP,
};
// PC - continue program counter value of preempted task
uint32_t task_handlers[MANY_TASKS] = {  (uint32_t) iddle_task,
										(uint32_t) task1_handler,
										(uint32_t) task2_handler,
										(uint32_t) task3_handler,
										(uint32_t) task4_handler,
										(uint32_t) task5_handler,
										(uint32_t) task6_handler,
//										(uint32_t) task7_handler,
//										(uint32_t) task8_handler,
//										(uint32_t) task9_handler,
//										(uint32_t) task10_handler,
//										(uint32_t) task11_handler,
//										(uint32_t) task12_handler,
//										(uint32_t) task13_handler,
//										(uint32_t) task14_handler,
//										(uint32_t) task15_handler,

};
//---
//

const char inf3[] = { " Ticks: "};


#define  QUEUE1_LEN 10
#define  TASK_QUEUE1 5 // thread no handling Queue1
uint32_t Queue_1[QUEUE1_LEN];
uint32_t Queue_1_head = 0;
uint32_t Queue_1_tail = 0;
uint32_t Queue_1_Tasks = 0;

//=====================================================================================================================
bool put_Queue1_task(uint32_t addrs) {


	if(Queue_1_Tasks == QUEUE1_LEN) {return false;};
	__asm volatile("CPSID I");

	Queue_1[Queue_1_tail] = addrs;
	Queue_1_Tasks ++;
	Queue_1_tail ++;
	if(Queue_1_tail == QUEUE1_LEN ) { Queue_1_tail = 0;}
	Unblock_Task((uint32_t) TASK_QUEUE1);
	__asm volatile("CPSIE I");
	return true;
}
//=====================================================================================================================
uint32_t handle_Queue1(void) {

//---
	uint32_t * pUART2_Base     = (uint32_t *)0x40004400;
//---


	__asm volatile("CPSID I");
	if(Queue_1_Tasks == 0) {
		__asm volatile("CPSIE I");
		return 0xffffffff;
	}

	uint32_t many_task = Queue_1_Tasks;
	Queue_1_Tasks = QUEUE1_LEN;
	__asm volatile("CPSIE I");



//===================================================================================================================================
//===================================================================================================================================
//===================================================================================================================================
//===================================================================================================================================
#define  DBG

#ifdef DBGx

	uint8_t idx4[20];
	uint8_t idx5[20];
	uint32_t ctrl;
	const char inf4[] = { " Queue: "};
	ctrl = 0;
	my_utoa(&idx5[0], many_task);
	// print " Queue: " text
    while(inf4[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){
    		__asm volatile("NOP");
    	}
    	pUART2_Base[1] = inf4[ctrl];
    	ctrl++;
    }

    ctrl = 0;
    // print many_task decimal value
    while(idx5[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){
    		__asm volatile("NOP");
    	}
    	pUART2_Base[1] = idx5[ctrl];
    	ctrl++;
    }
	while((pUART2_Base[0] & (1 << 6))== 0){
		__asm volatile("NOP");
	}
	pUART2_Base[1] = 0x20;

	ctrl = 0;
	// print " Ticks: " text
    while(inf3[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){
    		__asm volatile("NOP");
    	}
    	pUART2_Base[1] = inf3[ctrl];
    	ctrl++;
    }

	ctrl = 0;
    my_utoa(&idx4[0], tck_cnt); // convert tck_cnt to decimal value
//    my_htoa32(&idx4[0], pUART2_Base[2]); // convert BRR to hex value
    // print tck_cnt decimal value
    while(idx4[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){
    		__asm volatile("NOP");
    	}
    	pUART2_Base[1] = idx4[ctrl];
    	ctrl++;
    }
	while((pUART2_Base[0] & (1 << 6))== 0){
		__asm volatile("NOP");
	}
	pUART2_Base[1] = 0x20;
#endif
//===================================================================================================================================
//===================================================================================================================================
//===================================================================================================================================
//===================================================================================================================================

	uint8_t * pAddrs = (uint8_t*) Queue_1[Queue_1_head];
	Queue_1_head ++;
	if(Queue_1_head == QUEUE1_LEN) {
		Queue_1_head = 0;
	}

	uint32_t i = 0;

    while(pAddrs[i] != 0) {
    	while((pUART2_Base[0] & (1 << 6))== 0){
    		__asm volatile("NOP");
//    		OS_schedule(); // switch the context instead wait for complete transmition
    	}
    	pUART2_Base[1] = pAddrs[i];
    	i++;

    }

    many_task --;
	Queue_1_Tasks = many_task;

    return 0;
}
//=====================================================================================================================
//=====================================================================================================================

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//---
uint32_t tck_cnt = 0;     // tick counter
uint32_t current_task = 0; // task IDDLE is running
//---
//=====================================================================================================================
//=====================================================================================================================
//---
//----------------------------------------
//--- __attribute__((naked)) FUNCTIONS ---
//----------------------------------------
void change_sp_to_psp(uint32_t psp);
void OS_start(void);
void set_MSP(uint32_t data);
uint32_t get_LR_VALUE_address(uint32_t * data);
//---
//---------------------------
//--- regular C FUNCTIONS ---
//---------------------------
void OS_delay(uint32_t delay);
void Task_delay(uint8_t task_no , uint32_t delay );
void enable_processor_faults(void);
void sysTick_init(void);
void init_tasks_stacks(void);
void save_psp_value(uint32_t current_psp_value);
uint32_t get_psp_value(void);
void update_next_task(void);
void unblock_tasks(void);
void OS_schedule(void);
void set_priorites_of_exc(void);
void config_IRQ_PRIO(uint8_t irq_no, uint8_t priority_value);
void set_tick_and_pendsv_prio(uint8_t tick_prio, uint8_t pend_prio, uint8_t svc_prio);
//-
void iddle_task(void); // iddle task
//---
//=====================================================================================================================
//=====================================================================================================================

//=====================================================================================================================
void enable_processor_faults(void)
{
    uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

    // enable mem manage , bus fault and usage fault
    pSHCSR[0] |= (( 1 << 16) | ( 1 << 17) | ( 1 << 18));
}
//=====================================================================================================================
void init_tasks_stacks(void)
{
	uint32_t * pPSP ;
	task_delay[0] = 0;

	for(int i = 0; i < MANY_TASKS ; i++) {
		pPSP = (uint32_t*) psp_of_tasks[i];

		pPSP--;
		pPSP[0] = DUMMY_XPSR;

		pPSP--;  // PC
		pPSP[0] = task_handlers[i];

		pPSP--;  // LR
		pPSP[0] = 0xFFFFFFFD; // ISR RETURN to thread code with PSP

		for(int x = 0; x < 13; x++){
			pPSP--;
			pPSP[0] = 0xDEADBEEF ;
		}

		psp_of_tasks[i] = (uint32_t) pPSP;
	}
}
//===============================================================================================
//---   set the ISR no of priority if needed   ---
//------------------------------------------------
void config_IRQ_PRIO(uint8_t irq_no, uint8_t priority_value)
{

	uint32_t *pNVIC_IPRBASE  = (uint32_t*) 0xE000E400;

	// find iprx
	uint8_t   iprx = irq_no >> 2 ; // irq_no / 4
	uint32_t *ipr  = pNVIC_IPRBASE + iprx;

	// calc possition in iprx
	uint8_t pos = (irq_no % 4) * 8;

	// set the priority
	priority_value = priority_value << 4;
	ipr[0] &= ~(0xFF << pos);
	ipr[0] |= (priority_value << pos);

}
//=====================================================================================================================
void set_priorites_of_exc(void)
{
	// dummy configure priority
	//config_IRQ_PRIO(x , 4); // configure IRQ X ,  priority value  in MY_RTOS
}
//=====================================================================================================================
void set_tick_and_pendsv_prio(uint8_t tick_prio, uint8_t pend_prio, uint8_t svc_prio)
{
	uint32_t * pSHPR2 = (uint32_t*) 0xE000ED1C; // pointer to SHPR2
	uint32_t * pSHPR3 = (uint32_t*) 0xE000ED20; // pointer to SHPR3
	uint32_t PRIO;
	pSHPR3[0] = 0;

	tick_prio = tick_prio << 4;
	pend_prio = pend_prio << 4;
	svc_prio = svc_prio << 4;
	PRIO = tick_prio;
	PRIO = PRIO << 8;
	PRIO |= pend_prio;
	PRIO = PRIO << 16;
	pSHPR3[0] = PRIO; // set the systick priority as 15 and pendSV priority as 15

	pSHPR2[0] = 0;
	PRIO = svc_prio;
	PRIO = PRIO << 24;
	pSHPR2[0] = PRIO; // set the SVC priority as 15
}
//=====================================================================================================================
void sysTick_init(void)
{
	__asm volatile("CPSID I");
//----------------------------
//---  init SYSTICK TIMER  ---
//----------------------------
	SYSTICK->CTRL = 0;
	SYSTICK->COUNT = 0;
	SYSTICK->LOAD = (COUNT_VALUE - 1);
	SYSTICK->CTRL = 7;
//----------------------------
	// set the systick , SVC , pendSV priority as 15
	set_tick_and_pendsv_prio(SYSTICK_PRIO, PENDSV_PRIO, SVC_PRIO);

	__asm volatile("CPSIE I");
}
//=====================================================================================================================
void OS_schedule(void)
{
	uint32_t * pICSR = (uint32_t*) 0xE000ED04; // pointer to ICSR
    pICSR[0] = (1 << 28); // make PendSV exception
}
//=====================================================================================================================
void Task_delay(uint8_t task_no , uint32_t delay ) {
	__asm volatile("CPSID I");
	if(task_no != 0){
		if(task_no < MANY_TASKS) {
			task_delay[task_no] = delay;
		}
	}
	__asm volatile("CPSIE I");
}
void Unblock_Task(uint32_t task_no) {
	Delay_Task(task_no, 0);
}
void Block_Task(uint32_t task_no) {
		Delay_Task(task_no, 0xFFFFFFFF);
}
void Delay_Task(uint32_t task_no , uint32_t delay) {
	if((task_no < MANY_TASKS) && (task_no != 0)) {
		task_delay[task_no] = delay;
	}
}
void OS_delay(uint32_t delay) {
	task_delay[current_task] = delay;
	OS_schedule();  // return time of CPU from current task to the next
}
//=====================================================================================================================
__attribute__((naked)) void change_sp_to_psp(uint32_t psp)
{
    __asm volatile("MOVS  R1,      #0x02");       // change stack value for CONTROL register
    __asm volatile("MSR   CONTROL, R1");          // switch stack to PSP used value
    __asm volatile("MOV   SP,      R0");          // initialize PSP stack value
    __asm volatile("BX    LR");                   // go back to invoked place
}
//=====================================================================================================================
__attribute__((naked)) void set_MSP(uint32_t data) {
//---
	// in R0 new MSP value
	__asm volatile("MOV   SP,     R0");          // initialize MSP stack value
	__asm volatile("BX    LR");                  // return
}
//=====================================================================================================================


//=====================================================================================================================
__attribute__((naked)) uint32_t get_LR_VALUE_address(uint32_t * data)
{
	// in R0 address to variable with LR value
	// just go back with a R0 address of variable
	__asm volatile("BX    LR");
}
//---
uint32_t LR_VALUE;        // LR link register temporary value
//---
//=====================================================================================================================
__attribute__((naked)) void OS_start(void) {
//---
//	__asm volatile("PUSH  {LR}");             // SAVE return address on the stack
//	get_LR_VALUE_address(&LR_VALUE);          // get variable address into R0
//	__asm volatile("POP   {LR}");             // restore LR value
//	__asm volatile("STR   LR,     [R0]");     // store LR value into the LR_VALUE variable
//---
	__asm volatile("CPSID I");     // disable interrupts for eliminate any race conditions
//---
	set_MSP(MSP_STACK);
	init_tasks_stacks();
	enable_processor_faults();
	change_sp_to_psp(PSP_STACK);              // init MSP, PSP and switch to PSP
	sysTick_init();
//---

//---
	__asm volatile("LDR   R1, = iddle_task ");
	__asm volatile("BX    R1");           // iddle task invoke
//	get_LR_VALUE_address(&LR_VALUE);          // get variable address into R0
//	__asm volatile("LDR   R1,     [R0]");     // get into R1 old LR value from the LR_VALUE variable
//	__asm volatile("BX    R1");               // and now go back to invoked place
//---
}
//=====================================================================================================================
void save_psp_value(uint32_t current_psp_value)
{
	psp_of_tasks[current_task] = current_psp_value; // save current task actual preempted SP value
}
//=====================================================================================================================
uint32_t get_psp_value(void)
{
	return psp_of_tasks[current_task]; // take preeampted SP (PSP) value and return in R0 register
}
//=====================================================================================================================
void update_next_task(void)
{
	uint32_t register delay_state;
	uint32_t register delay_task;

	//	unblock_tasks();
	for(int i = 1 ; i < MANY_TASKS ; i++){
		delay_task = task_delay[i];
		if((delay_task != 0) &&  (delay_task != 0xffffffff)) { task_delay[i] --; }
	}

	for(int i= 0 ; i < (MANY_TASKS) ; i++){
		current_task++;
		current_task = current_task % MANY_TASKS;
		delay_state = task_delay[current_task];
		if( (delay_state == 0) && (current_task != 0) ) { break; }
	}
    if(delay_state != 0){ current_task = 0; }
}
//=====================================================================================================================
void unblock_tasks(void)
{
	for(int i = 1 ; i < MANY_TASKS ; i++){
		if(task_delay[i] != 0) { task_delay[i] --; }
	}
}
//=====================================================================================================================
void SysTick_Handler(void)
{
	uint32_t * pICSR = (uint32_t*) 0xE000ED04; // pointer to ICSR
	tck_cnt++;
    pICSR[0] = (1 << 28); // make PendSV exception
}
//=====================================================================================================================
__attribute__((naked)) void PendSV_Handler(void)
{
	__asm volatile("CPSID  I ");			   // disable interrupts for eliminate any race conditions
	__asm volatile("PUSH {LR}");			   // save Link Register {LR}
//---
	__asm volatile("MRS R0, PSP");             // take actual task stack pointer
	__asm volatile("STMDB R0!, {R4-R11}");     // save R4-R11 registers into task stack pointer
	__asm volatile("BL save_psp_value");       // save current task actual preempted SP value
//---
	__asm volatile("BL update_next_task");     // scheduler job for running next task
	__asm volatile("BL get_psp_value");        // get scheduled task preempted before SP value into R0
	__asm volatile("LDMIA R0!, {R4-R11}");     // restore for next scheduled tasks context
	__asm volatile("MSR PSP, R0");             // set MSP stack pointer of new scheduled task
//---
	__asm volatile("CPSIE  I ");			   // enable interrupts
	__asm volatile("POP {PC}");			   	   // restore Link Register (LR) and immediatelly send into {PC} Program Couner
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
// Implement the fault handlers
__attribute__((weak)) void HardFault_Handler(void) { while(1) {;}}
__attribute__((weak)) void MemManage_Handler(void) { while(1) {;}}
__attribute__((weak)) void BusFault_Handler(void) { while(1) {;}}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
void iddle_task(void)
{
	__asm volatile("CPSIE I");  // enable interrupts
	while(1){
		__asm volatile("WFI");
	}
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

//------------------------------------
//--- dummy task hanlers functions ---
//------------------------------------
__attribute__((weak)) void task1_handler(void)  { while(1){ OS_delay(0xffffffff); }}
//=====================================================================================================================
__attribute__((weak)) void task2_handler(void)  { while(1){ OS_delay(0xffffffff); }}
//=====================================================================================================================
__attribute__((weak)) void task3_handler(void)  { while(1){ OS_delay(0xffffffff); }}
//=====================================================================================================================
__attribute__((weak)) void task4_handler(void)  { while(1){ OS_delay(0xffffffff); }}
//=====================================================================================================================
// dummy tasks functions
//---
__attribute__((weak)) void task5_handler(void)  { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task6_handler(void)  { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task7_handler(void)  { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task8_handler(void)  { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task9_handler(void)  { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task10_handler(void) { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task11_handler(void) { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task12_handler(void) { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task13_handler(void) { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task14_handler(void) { while(1){ OS_delay(0xffffffff); }}
__attribute__((weak)) void task15_handler(void) { while(1){ OS_delay(0xffffffff); }}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
