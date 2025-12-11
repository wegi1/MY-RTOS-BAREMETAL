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
void enable_processor_faults(void);
void sysTick_init(void);
void init_tasks_stacks(void);
void save_psp_value(uint32_t current_psp_value);
uint32_t get_psp_value(void);
void update_next_task(void);
void unblock_tasks(void);
void set_priorites_of_exc(void);
void set_tick_and_pendsv_prio(uint8_t tick_prio, uint8_t pend_prio, uint8_t svc_prio);
//-

//---
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

	__asm volatile("CPSID I");
	if(Queue_1_Tasks == QUEUE1_LEN) {
		__asm volatile("CPSIE I");
		return false;
	}


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
	volatile uint32_t * pUART2_Base     = (volatile uint32_t *)0x40004400;
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
    	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
    	pUART2_Base[1] = inf4[ctrl];
    	ctrl++;
    }

    ctrl = 0;
    // print many_task decimal value
    while(idx5[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
    	pUART2_Base[1] = idx5[ctrl];
    	ctrl++;
    }
	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
	pUART2_Base[1] = 0x20;

	ctrl = 0;
	// print " Ticks: " text
    while(inf3[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
    	pUART2_Base[1] = inf3[ctrl];
    	ctrl++;
    }

	ctrl = 0;
    my_utoa(&idx4[0], tck_cnt); // convert tck_cnt to decimal value
//    my_htoa32(&idx4[0], pUART2_Base[2]); // convert BRR to hex value
    // print tck_cnt decimal value
    while(idx4[ctrl] != 0){
    	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
    	pUART2_Base[1] = idx4[ctrl];
    	ctrl++;
    }
	while((pUART2_Base[0] & (1 << 6))== 0){ ; }
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
    	while((pUART2_Base[0] & (1 << 6))== 0){ ;
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
//======================================================
// HEX 64BIT value convert to string
//====================================
// INPUT:
//    R0, =  ; POINTER TO OUTPUT char DATA BUFFER
//    R2, =  ; VALUE TO HEXADECIMAL CONVERT LO 4BYTES
//    R3, =  ; VALUE TO HEXADECIMAL CONVERT HI 4BYTES
//---
// output: no output, data in buffer are output
//=======================================================
//=========================
extern __attribute__((naked)) void my_htoa64(uint8_t * buf, uint64_t data) {

extern void HT64CNT(void); // declaration for find subfunction

__asm volatile("PUSH    {R2, LR}");
__asm volatile("MOVS    R1, R3");
__asm volatile("BL      my_htoa32   ");     // CONVERT HI 4 BYTES
__asm volatile("POP     {R1}");
__asm volatile("ADDS    R0, R0, #8 ");    // MOVE POINTER OUTPUT BUFFER
__asm volatile("BL      HT64CNT ");       // NOW CONVERT LOW 4 BYTES

__asm volatile("POP     {PC}");
//---
}
//=====================================================================================================================

//======================================================
// HEX 32BIT value convert to string
//====================================
// INPUT:
//    R0, =  ; POINTER TO OUTPUT char DATA BUFFER
//    R1, =  ; VALUE TO HEXADECIMAL CONVERT
//---
// output: no output, data in buffer are output
//=======================================================
//=========================

extern __attribute__((naked)) void my_htoa32(uint8_t * buf, uint32_t data){

extern void HT64CNT(void); // declaration for find subfunction

__asm volatile("MOV    R3, 0x7830");     // '0x' ");
__asm volatile("STRH   R3, [R0]");       // STORE '0x' TO BUFFER");
__asm volatile("B HT64CNT");      // ");
}
extern __attribute__((naked)) void HT64CNT(void) {

__asm volatile("MOVS   R3, #2");         // output data 2 bytes after start buffer
//---
__asm volatile("LP01:");
__asm volatile("LSRS   R2, R1, #28");    // R2 = OUR DATA TO STRING CONVERT 1 BYTE = NYBBLE 0..F");
__asm volatile("LSLS   R1, R1, #4");     // R1 * 16 = R1 << 4");
__asm volatile("ADDS   R2, R2, #0x30");  // CONVERT TO NUMBER");
__asm volatile("CMP    R2, #0x3A");
__asm volatile("BCC.N  LP01A");          // if data < than 0x3a don't  add #7");
__asm volatile("ADDS   R2,R2, 0x07");    // CONVER TO LETTER");
__asm volatile("LP01A:");
__asm volatile("STRH   R2, [R0, R3]");   // STRH store byte and zero terminated");
__asm volatile("ADDS   R3, R3, #1");
__asm volatile("CMP    R3, #10");
__asm volatile("BNE.N  LP01");
//---
__asm volatile("BX     LR");
//=============================================================
}
//=====================================================================================================================
//======================================================
// store LineFEED and CR value to pointed buffer
//======================================================
// INPUT:
//    R0, =  ; POINTER TO OUTPUT char DATA BUFFER
//---
// output: no output, data in buffer are output
//=======================================================
extern __attribute__((naked))  void set_LF(uint32_t * buff_ad){
	__asm volatile("MOV    R1,  #0x00000A0D");
	__asm volatile("STR    R1, [R0]");
	__asm volatile("BX     LR");
//----------------
}
//=====================================================================================================================
//========================================================
// DECIMAL CONVERSION OF ANY UNSIGNED VALUE UP TO 32 BIT
//========================================================
// INPUT:
//    R0, =  ; POINTER TO OUTPUT char DATA BUFFER
//    R1, =  ; VALUE TO DECIMAL CONVERT
//---
// OUTPUT: R0 = LENGTH OF PRODUCED BYTES OF STRING
//========================================================
extern __attribute__((naked))  uint32_t my_utoa(uint8_t * buf, uint32_t data) {

__asm volatile("PUSH {R4,R5,LR}");  // store registers");


__asm volatile("MOV   R5, #0");  // LENGTH OF STRING");
__asm volatile("STR   R5, [R0]"); // CLEAR BUFFER");
__asm volatile("STR   R5, [R0, #4]");
__asm volatile("STR   R5, [R0, #8]");
__asm volatile("CALC01:");
//------------------
//- R4 = R1 DIV 10 -
//------------------
__asm volatile("LDR   R3, = 0xCCCCCCCD"); // MAGIC VALUE (!!!)");
__asm volatile("UMULL R4,R3,R3,R1");      // this three lines looks a bit strange");
__asm volatile("LSRS  R4,R3, #3");        // but here we gotta divide by 10 (seriously!)");
//----------------------------------------------
__asm volatile("MOVS  R2, R4");         // R2 = R1 div by 10 without the rest");
__asm volatile("MOVS  R3, #0x0A");      // R3 = 10");
__asm volatile("MUL   R4, R4, R3");     // R4 = R4 * 10");

// mod 10 calculate
// R4 = R1 mod 10
__asm volatile("SUBS  R4, R1, R4");     // CALCULATE THE REST r4 = r1 - r4");

__asm volatile("ORR   R4, R4, #0x30");  // CHANGE TO ASCII NUMBER VALUE '0..9'");
__asm volatile("STRH  R4, [R0,R5]");    // store next decimal row and ZERO terminate string");
__asm volatile("ADDS  R5, R5, #1");     // R5 = length of string");
__asm volatile("MOV   R1, R2");         // R1 = before stored value in R2 = (R1 div 10)");
__asm volatile("CMP   R1, #0");         // R1 = 0? (that was last one operation?)");
__asm volatile("BNE.N CALC01");         // if R1 != 0 then continue CALC01 loop");

__asm volatile("PUSH  {R5}");           // TEMPORARY STORE LENGTH OF STRING");
__asm volatile("SUBS  R5, R5, #1");     // SET OFFSET AT THE END OF STRING (BACKWARD POSSITION)");

// R0 = POINTER TO OUTPUT NULL TERMINATED STRING
// R5 = OFFSET TO THE END OF STRING (BACKWARD POSSITION)
// R1 = OFFSET TO THE START OF STRING (FORWARD POSSITION) R1 = 0 AT THE END CALC01 ROUTINE
// R4 = BACKWARD BYTE (BYTE FROM 'RIGHT SIDE')
// R2 = FORWARD  BYTE (BYTE FROM 'LEFT  SIDE')
__asm volatile("CALC02:");
__asm volatile("LDRB  R4, [R0, R5]"); // GET DATA FROM THE END (FROM RIGT SIDE)");
__asm volatile("LDRB  R2, [R0, R1]"); // GET DATA FROM THE START (LEFT SIDE)");
__asm volatile("STRB  R2, [R0, R5]"); // GET DATA FROM THE 'LEFT  SIDE INTO THE RIGHT SIDE'");
__asm volatile("STRB  R4, [R0, R1]"); // GET DATA FROM THE 'RIGHT SIDE INTO THE LEFT  SIDE'");
__asm volatile("ADDS  R1, R1, #1");   // ACTUALIZE STRING FORWARD POSSITION");
__asm volatile("SUBS  R5, R5, #1");   // ACTUALIZE STRING BACKWARD POSSITION");
__asm volatile("CMP   R5, R1");       // R5 =< R1 ?");
__asm volatile("BEQ.N END_CALC");     // if R5 = R1 go to finish");
__asm volatile("BGT.N CALC02");       // if R5 > R1 continue loop - otherway finish (R5 < R1)");

__asm volatile("END_CALC:");
// acording declaration this functin is 'int' and
// acordind AAPCS should be returned int value in R0
// simple 'return datalength;' in C
// OUTPUT: R0 = LENGTH OF PRODUCED BYTES OF STRING R0 = R5

__asm volatile("POP {R0, R4, R5, PC}        // restore registers // ADIOS :)");
// OUTPUT: R0 = LENGTH OF PRODUCED STRING BYTES
//=====================================================
}
//=====================================================================================================================
//========================================================
// DECIMAL CONVERSION OF ANY SIGNED VALUE UP TO 32 BIT
//========================================================
// INPUT:
//    R0, =  ; POINTER TO OUTPUT char DATA BUFFER
//    R1, =  ; VALUE TO DECIMAL CONVERT
// OUTPUT: R0 = LENGTH OF PRODUCED STRING BYTES
//========================================================

extern  __attribute__((naked)) uint32_t my_itoa(uint8_t * buf, uint32_t data) {

__asm volatile("CMP   R1, 0");       // IS OUR VALUE NEGATIVE?");
__asm volatile("BGE.N my_utoa");     // IF IS POSSITIVE GO TO my_sprintf");
__asm volatile("PUSH  {LR}");        // PC address go back to the main function");

__asm volatile("MOVS  R2, '-'");
__asm volatile("STRB  R2, [R0]");    // STORE '-' SIGN AT THE START TEXT BUFFER");
__asm volatile("ADDS  R0, R0, #1");  // MOVE UP 1 BYTE POINTER TO OUTBUFFER");

__asm volatile("NEG   R1, R1");      // CHANGE VALUE TO POSSITIVE NUMBER");
__asm volatile("BL    my_utoa");     // CONVERT VALUE TO DECIMAL STRING");

	                        // now in R0 is datalength from my_sprintf
__asm volatile("ADDS  R0, R0, #1");  // INCREASSE LENGTH OF STRING MORE 1 BYTE OF SIGN '-'");
__asm volatile("POP   {PC}");        // GO BACK TO THE MAIN FUNCTION");
//======================================================
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
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
