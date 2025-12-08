
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
//=====================================================================================================================
#include "leds.h"
#include "uart.h"
#include "MY_RTOS.h"
//=====================================================================================================================
extern void iddle_task(void);
extern uint8_t idx[];
//---
uint8_t idx[20];

//---
//=====================================================================================================================
//---
const  char clr_scr[] = {"\033[2J\033[3J\033[92m\033[0;0f"};
const  char run_INFO[] = {"MY RTOS coded by wegi\r\nRAW VERSION running \r\r\n\nSTARTED !!!\r\r\n\n"};
//---
//=====================================================================================================================

//====================================================================================================================

void NVIC_Enable_IRQ(uint32_t IRQ){

	__asm volatile ("LDR     R2, = 0xE000E100");
	__asm volatile ("LSRS    R1, R0, #5");
	__asm volatile ("MOVS    R3, #1");
	__asm volatile ("AND.W   R0, R0, #31");
	__asm volatile ("LSLS    R3, R0");
	__asm volatile ("STR.W   R3, [R2, R1, LSL #2]");
}


//=====================================================================================================================


//=====================================================================================================================
int main(void)
{
#define EXTI0 6

	led_init();
	uart_UART2_config();

	uint32_t * pRCC_APB1_enr   = (uint32_t *) 0x40023840;
	uint32_t * pRCC_APB2_enr   = (uint32_t *) 0x40023844;
    uint32_t * pRCC_AHB1_enr   = (uint32_t *) 0x40023830;
    uint32_t * pGPIOA          = (uint32_t *) 0x40020000;
    uint32_t * pEXTI           = (uint32_t *) 0x40013C00;

    pRCC_APB2_enr[0] |= (1 << 14); // SYSCFGEN ENABLE
	pRCC_APB1_enr[0] |= (1 << 28); // PWREN ENABLE
	pRCC_AHB1_enr[0] |= (1 << 0);  // GPIOA ENABLE


	__asm volatile("DSB");

	// 1. enable periph clock
	// 2. keep gpio as input mode
	// 3. configure edge detection
	// 4. enable IRQ over that GPIO pin (EXTI->IMR ???)
	// 5 enable IRQ in NVIC

	// 2.
	pGPIOA[0] &= ~(3 << 0); // GPIOA moder = input
	// 3. EXTI->RTSR |= (1 << 0);
	pEXTI[2]  |= (1 << 0);
	// 4. EXTI->IMR |= (1 << 0);
	pEXTI[0]  |= (1 << 0);
	// 5.

	config_IRQ_PRIO((uint8_t) EXTI0, 14);
	NVIC_Enable_IRQ(EXTI0);



//---
	OS_start();   // start RTOS and immediatelly run the iddle task
//---

	while(1){;}
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
void task1_handler(void)
{
	uint8_t idx2[20];
	uint8_t idx3[20];
	bool result ;
	static uint32_t cntr = 0;
	uint32_t * pdata = (uint32_t*) 0x40004408;

	while(1){
		led_toggle(LED_GREEN);
		if((cntr %5) == 0) {
			result = false;
			while( result != true ) {
				result = put_Queue1_task((uint32_t) &clr_scr[0]);
			}
			result = false;
			while( result != true ) {
				result = put_Queue1_task((uint32_t) &run_INFO[0]);
			}
		}
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"\r\nPrint from TASK1\r\n");
		}

		my_utoa(&idx[0], cntr);


		cntr ++;
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t) &idx[0]);
		}

		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)" Seconds from start...\r\n");
		}


		my_htoa32(&idx3[0], (uint32_t) pdata);
		my_htoa32(&idx2[0], (uint32_t) pdata[0]);
		set_LF((uint32_t*) &idx2[10]);

		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"Register at: ");
		}
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t) &idx3[0]);
		}

		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)" contains value ");
		}

		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t) &idx2[0]);

		}
		OS_delay(1000);
	}
}
//=====================================================================================================================
void task2_handler(void)
{
	bool result ;
	while(1){
		led_toggle(LED_ORANGE);
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"Print from TASK2\r\n");
		}
		OS_delay(500);
	}
}
//=====================================================================================================================
void task3_handler(void)
{
	bool result ;
	while(1){
		led_toggle(LED_RED);
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"Print from TASK3\r\n");
		}
		OS_delay(250);
	}
}
//=====================================================================================================================
void task4_handler(void)
{
	bool result ;
	while(1){
		led_toggle(LED_BLUE);
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"Print from TASK4\r\n");
		}
		OS_delay(125);
	}
}
//=====================================================================================================================
void task5_handler(void)
{

	while(1){

		OS_delay(handle_Queue1());
	}
}
//=====================================================================================================================
//=====================================================================================================================
void task6_handler(void)
{
    uint32_t * pGPIOA          = (uint32_t *) 0x40020000;
    uint32_t * pEXTI           = (uint32_t *) 0x40013C00;
	bool result ;
	while(1){
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"\r\n ... BUTTON PRESSED ... !!!\r\n\r\n");
		}
		OS_delay(20); // debounce delay

		while(1) {
			//if((GPIOA->IDR & 1 ) == 0) { break;}
			if((pGPIOA[4] & 1 ) == 0) { break;}
			OS_delay(10); // check every 10ms of button releasse
		}
		result = false;
		while( result != true ) {
			result = put_Queue1_task((uint32_t)"\r\n ....... BUTTON RELEASSED .......\r\n\r\n");
		}
		OS_delay(20);  // debounce delay
		//EXTI->PR = 1; // clear pending bit
		pEXTI[5]  = (1 << 0);
//		EXTI->IMR = 1; // enable IRQ turn arround
		pEXTI[0] = 1;
		OS_delay(0xFFFFFFFF); // job is done - disable task
	}
}
//=====================================================================================================================

void HardFault_Handler(void)
{
	for(;;);
}
//---
void MemManage_Handler(void)
{
	for(;;);
}
//---
void BusFault_Handler(void)
{
	for(;;);
}
//=====================================================================================================================
void EXTI0_IRQHandler(void) {

    uint32_t * pEXTI           = (uint32_t *) 0x40013C00;
	//EXTI->PR = 1; // clear pending bit
	pEXTI[5]  = (1 << 0);
	//EXTI->IMR = 0; // disable IRQ by mask
	pEXTI[0]  = 0;

	Unblock_Task(6); // enable Handling button debonce, releasse and back to enable Interrupts
}
//=====================================================================================================================

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================


