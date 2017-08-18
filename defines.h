/*
 * defines.h
 *
 *  Created on: 12.08.2017
 *      Author: rauser
 */

#ifndef DEFINES_H_
#define DEFINES_H_

typedef enum gpio_value {
	OFF = 0,
	ON  = 1
} GPIOValue;

typedef enum gpio_direction {
	OUTPUT = 0,
	INPUT  = 1
} GPIODirection;

typedef enum game_direction {
	FORWARD = 0,
	BACKWARD
} GameDirection;

typedef enum led_numeration {
	LED1, // red
	LED2, // blue
	LED3, // green
	LED4  // yellow
} LEDNumeration;

typedef enum mode {

	M_NORMAL_FORWARD,
	M_FAST_FORWARD,
	M_RANDOM_FORWARD,
	M_RANDOM_RANDOM,
	MODE_MAX

} Mode;

#endif /* DEFINES_H_ */
