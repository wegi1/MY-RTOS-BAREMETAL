/*
 * leds.h
 *
 *  Created on: Nov 13, 2025
 *      Author: BOLO
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <inttypes.h>

//---
#define LED_GREEN			12
#define LED_ORANGE 			13
#define LED_RED 			14
#define LED_BLUE 			15
//---

//---
extern void led_init(void);
extern void led_toggle(uint32_t LED);
extern void led_on(uint32_t LED);
extern void led_off(uint32_t LED);
//---


#endif /* LEDS_H_ */
