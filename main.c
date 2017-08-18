//#include "PeripheralNames.h"


#include "defines.h"
#include "macros.h"
#include <stdlib.h>

#define PWM_OFF   0 // 0V (OFF)
#define PWM_3V   85 // 3V (9V / 255 * 155 = 3V)
#define PWM_9V  255 // 9V

static GameDirection direction   = FORWARD;
static LEDNumeration active_led  = LED1;
static uint8_t       motor_power = PWM_3V;
static Mode          mode        = M_NORMAL_FORWARD;

void setup ( void );
void init  ( void );

void show_mode( void );
void show_direction ( void );
void trigger_direction( void );

uint8_t calc_speed( void );
GameDirection calc_direction( void );

/* ****************************************************************************
 *
 * interrupt service routine
 *
 * ***************************************************************************/
ISR(INT0_vect)
{
	ADD_ONE_BETWEEN( mode, M_NORMAL_FORWARD, M_RANDOM_RANDOM );
	show_mode();
}

void setup( void )
{
	// leds
	GPIO_init( DDRD, PD0, OUTPUT ); // 1
	GPIO_init( DDRD, PD1, OUTPUT ); // 2
	GPIO_init( DDRA, PA1, OUTPUT ); // 3
	GPIO_init( DDRA, PA0, OUTPUT ); // 4

	GPIO_init( DDRD, PD4, OUTPUT ); // red
	GPIO_init( DDRD, PD5, OUTPUT ); // blue
	GPIO_init( DDRB, PB0, OUTPUT ); // green
	GPIO_init( DDRB, PB1, OUTPUT ); // yellow

	// motor control
	GPIO_init( DDRB, PB3, OUTPUT ); // forward
	GPIO_init( DDRB, PB4, OUTPUT ); // backward
	GPIO_init( DDRB, PB2, OUTPUT ); // PWM
	PWM_enable( (1 << COM0A1) | (1 << WGM00), (1 << CS01), 0x00 );

	// button
	GPIO_init( DDRD, PD2, INPUT);
	GPIO_interrupt( PORTD, PD2, INT0, (1 << ISC01) );
}

void init( void )
{
	// set initial values
	direction   = FORWARD;
	active_led  = LED1;
	motor_power = PWM_3V;
	mode        = M_NORMAL_FORWARD;

	// show current mode
	show_mode();

	// initial pwm
	PWM_set( PWM_3V );

	// set seed
	init_random( TCNT1L );
}

void show_mode( void ) {

//	GPIO_on ( PORTD, PD0 );
//				GPIO_off( PORTD, PD1 );
//				GPIO_off( PORTA, PA1 );
//				GPIO_off( PORTA, PA0 );

	switch ( mode )
	{
		case M_NORMAL_FORWARD:
			GPIO_off( PORTD, PD0 );
			GPIO_off( PORTD, PD1 );
			GPIO_off( PORTA, PA1 );
			GPIO_on ( PORTA, PA0 );
			break;
		case M_FAST_FORWARD:
			GPIO_off( PORTD, PD0 );
			GPIO_off( PORTD, PD1 );
			GPIO_on ( PORTA, PA1 );
			GPIO_on ( PORTA, PA0 );
			break;
		case M_RANDOM_FORWARD:
			GPIO_off( PORTD, PD0 );
			GPIO_on ( PORTD, PD1 );
			GPIO_on ( PORTA, PA1 );
			GPIO_on ( PORTA, PA0 );
			break;
		case M_RANDOM_RANDOM:
			GPIO_on ( PORTD, PD0 );
			GPIO_on ( PORTD, PD1 );
			GPIO_on ( PORTA, PA1 );
			GPIO_on ( PORTA, PA0 );
			break;
		default: /* unkown */
			GPIO_off( PORTD, PD0 );
			GPIO_off( PORTD, PD1 );
			GPIO_off( PORTA, PA1 );
			GPIO_off( PORTA, PA0 );
			break;
	}
}

void show_direction ( void )
{
	if( direction == BACKWARD )	DEC_ONE_BETWEEN( active_led, LED1, LED4 );
	else                        ADD_ONE_BETWEEN( active_led, LED1, LED4 );

	switch ( active_led )
	{
		case LED1:
			GPIO_off ( PORTD, PD4 );
			GPIO_off( PORTD, PD5 );
			GPIO_off( PORTB, PB0 );
			GPIO_on ( PORTB, PB1 );
			break;
		case LED2:
			GPIO_off( PORTD, PD4 );
			GPIO_off( PORTD, PD5 );
			GPIO_on ( PORTB, PB0 );
			GPIO_off( PORTB, PB1 );
			break;
		case LED3:
			GPIO_off( PORTD, PD4 );
			GPIO_on ( PORTD, PD5 );
			GPIO_off( PORTB, PB0 );
			GPIO_off( PORTB, PB1 );
			break;
		case LED4:
			GPIO_on ( PORTD, PD4 );
			GPIO_off( PORTD, PD5 );
			GPIO_off( PORTB, PB0 );
			GPIO_off( PORTB, PB1 );
			break;
		default: /* unkown */
			GPIO_off( PORTD, PD4 );
			GPIO_off( PORTD, PD5 );
			GPIO_off( PORTB, PB0 );
			GPIO_off( PORTB, PB1 );
			break;
	}
}

void trigger_direction( void )
{
	if( direction == FORWARD )
	{
		GPIO_off( PORTB, PB3 );
		GPIO_on ( PORTB, PB4 );
	}
	else
	{
		GPIO_on ( PORTB, PB3 );
		GPIO_off( PORTB, PB4 );
	}
}

GameDirection calc_direction( void )
{
	switch( mode )
	{
		case M_NORMAL_FORWARD:
		case M_FAST_FORWARD:
		case M_RANDOM_FORWARD:
		default:
			return FORWARD;
			break;
		case M_RANDOM_RANDOM:
			return ( (get_random_between( 0, 10 ) == 0) ? BACKWARD : FORWARD ); /* 10% backward : 90% forward */
			break;
	}
}

/**
 * Return a random speed.
 * @return pwm value between 0x00 and 0xFF
 */
uint8_t calc_speed( void )
{
	switch( mode )
	{
	case M_NORMAL_FORWARD:
	default:
		return PWM_3V;
		break;
	case M_FAST_FORWARD:
		return PWM_9V;
		break;
	case M_RANDOM_FORWARD:
	case M_RANDOM_RANDOM:
		/*
		 * Calculate speed in 25 steps.
		 * 0 = 0V, ...,  25 = ~9V
		 *
		 * For a better and faster playing pleasure,
		 * a probability of
		 * 10% values between   0V and 3.5V and
		 * 90% values between 3.5V and   9V is selected.
		 */
		if ( get_random_between( 0, 255 ) == 0 )
		{ /* 20% slow ( 0V - ~3,5V ) */
			return ( get_random_between(  0, 10 ) * 10 );
		}
		else
		{ /* 80% fast ( ~3,5V - 9V ) */
			return ( get_random_between( 11, 25 ) * 10 );
		}
		break;
	}
}

void play( void )
{
	static uint8_t cycle = 0;
	if( cycle == 0 )
	{ /* is called every 1 second */
		direction = calc_direction();
		trigger_direction();
		PWM_set( calc_speed() );
	}
	show_direction();
	ADD_ONE_BETWEEN( cycle, 0, 10 );
	_delay_ms( 100 );
}

int main(void) {

	setup();
	init();

	while (1) {
		play();
	}

	return 0;
}
