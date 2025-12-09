/*
 * uart.c
 *
 *  Created on: Dec 4, 2025
 *      Author: BOLO
 */
//==================================================================================================================================

//---
#include "stdint.h"
#include "uart.h"
#include "MY_RTOS.h"
//--

//---
//==================================================================================================================================

void uart_UART2_config(void){


#define GPIOA_EN (1 << 0)
#define UART2_EN (1 << 17)
#define BAUD_RATE  115200
#define BRR_HI (F_CPU / BAUD_RATE)

    uint32_t * pRCC_AHB1_enr   = (uint32_t *)0x40023830;
    uint32_t * pRCC_APB1_enr   = (uint32_t *)0x40023840;

    uint32_t * pGPIOA_Base     = (uint32_t *)0x40020000;
    uint32_t * pUART2_Base     = (uint32_t *)0x40004400;

    pRCC_APB1_enr[0] |= UART2_EN; // enable UART 2
    pRCC_AHB1_enr[0] |= GPIOA_EN; // enable GPIO A

    __asm volatile("DSB");

    pGPIOA_Base[0] |= (2 << 4); // GPIOA->MODER Bits (5:4) = 1:0 --> PA2  as alternate function
    pGPIOA_Base[0] |= (2 << 6); // GPIOA->MODER Bits (7:6) = 1:0 --> Alternate function for PA3
    pGPIOA_Base[2] |= (3 << 4) | (3 << 6); //  GPIOA->OSPEEDR Bits [5:4] = 1:1 and Bits (7:6) = 1:1 --> High Speed for PIN PA2 and PA3
    pGPIOA_Base[8] |= (7 << 8);  // Bytes (11:10:9:8)   = 0:1:1:1 --> AF7 Alternate Function for USART2 at Pin PA2
    pGPIOA_Base[8] |= (7 << 12); // Bytes (15:14:13:12) = 0:1:1:1 --> AF7 Alternate Function for USART2 at Pin PA3

    pUART2_Base[3] = 0; // reset all bits in USART2->CR1 (1 bits STOP Bit 14 = 0)
    pUART2_Base[3] |= (1 << 13); // UE Bit = 1 ... Enable UART 2

    pUART2_Base[2]  = BRR_HI;    // Baud rate of 115200, PCLK = 16MHz UART2->BRR
    pUART2_Base[3] |= (1 << 2) | (1 << 3); // Enable Receiver and Transmitter
}

//==================================================================================================================================
void prt_char(uint8_t *  znak) {
//---
	uint32_t * pUART2_Base     = (uint32_t *)0x40004400;
//---

//===
	while((pUART2_Base[0] & (1 << 6)) == 0) {
		__asm volatile("NOP"); // need beware idiot compiler in "Os" optimization remove all while loop
	}
	pUART2_Base[1] =   znak[0];
//===

}
//==================================================================================================================================

