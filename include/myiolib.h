/**********************************************
myiolib.h library header file version 1.0
This library was created by Creed Truman.
It was created for Sudeepa Pathak's EGR-150-AF01 Class in Spring 2026.
This library is licensed under the MIT License.
This version was last updated on 4/22/2026.

Credit to Google Gemini for helping me debug and optimize the code, for explaining AVR architecture, and helping me get started with VSCode.


This library is designed to provide basic IO functions, pin definitions, and timer setup for AVR microcontrollers.
It is one of my first attemps at writing a C library.
It is not fully functional by any means.

TODO:
Add better serial IO functions
Consider pivoting to using OOP for better code organization and readability.
For example, using LED_1.on(), .off(), .toggle(), .mode(), .read(), etc.
This would also allow me to also have seperate objects for different pin types which may reduce compiler errors
*/


//This is necessary for IO port definitions, interrupt handling, and uint8_t
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

//Necessary for setting up pins
struct PWMStruct{
  uint8_t modeBit;
  volatile uint8_t* timer;
  volatile uint8_t* PWMPtr;
};
struct PinStruct{
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  uint8_t bit;
  PWMStruct PWMData;
};


//Placeholder
constexpr PWMStruct NO_PWM = {0, nullptr, nullptr};

//This is where I will define the pin constants. I am using the PinStruct to store all necessary information about each pin in one place, which should make it easier to write generic functions for pin manipulation and PWM control.
//I do not have PWM set up for all pins just yet, so I am using the NO_PWM for those pins (3, 9, 10, 11).
constexpr PinStruct PIN_0 = {&PORTD, &DDRD, &PIND, 0, NO_PWM};
constexpr PinStruct PIN_1 = {&PORTD, &DDRD, &PIND, 1, NO_PWM};
constexpr PinStruct PIN_2 = {&PORTD, &DDRD, &PIND, 2, NO_PWM};
constexpr PinStruct PIN_3 = {&PORTD, &DDRD, &PIND, 3, NO_PWM};
constexpr PinStruct PIN_4 = {&PORTD, &DDRD, &PIND, 4, NO_PWM};
constexpr PinStruct PIN_5 = {&PORTD, &DDRD, &PIND, 5, {COM0B1, &TCCR0A, &OCR0B}};
constexpr PinStruct PIN_6 = {&PORTD, &DDRD, &PIND, 6, {COM0A1, &TCCR0A, &OCR0A}};
constexpr PinStruct PIN_7 = {&PORTD, &DDRD, &PIND, 7, NO_PWM};
constexpr PinStruct PIN_8 = {&PORTB, &DDRB, &PINB, 0, NO_PWM};
constexpr PinStruct PIN_9 = {&PORTB, &DDRB, &PINB, 1, NO_PWM};
constexpr PinStruct PIN_10 = {&PORTB, &DDRB, &PINB, 2, NO_PWM};
constexpr PinStruct PIN_11 = {&PORTB, &DDRB, &PINB, 3, NO_PWM};
constexpr PinStruct PIN_12 = {&PORTB, &DDRB, &PINB, 4, NO_PWM}; 
constexpr PinStruct PIN_13 = {&PORTB, &DDRB, &PINB, 5, NO_PWM};

//These are just constants for readability and convenience.
constexpr uint8_t ON = 1;
constexpr uint8_t OFF = 0;
constexpr uint8_t IN = 0;
constexpr uint8_t OUT = 1;

void initSerial9600(void){
	//This formula is from the AVR datasheet. It calculates the value to put in the UBRR0H and UBRR0L registers to set the baud rate to 9600. The formula is (F_CPU / (16 * baud)) - 1, where F_CPU is the clock frequency of the microcontroller (16MHz in this case) and baud is the desired baud rate (9600 in this case). To adjust the baud rate speed, simply change 9600. the UL is necessary to tell the compiler that these are unsigned long constants, which is important for the calculation to work correctly.
	uint16_t ubrr = 16000000UL / (16UL * 9600UL) - 1;
	//This sets the low and high bytes of the UBRR0 register. It is necessary to set them in this manner because sometimes ubrr is greater than 255, which is the maximum value that can be stored in an 8-bit register. By shifting ubrr to the right by 8 bits, we get the high byte, and by casting ubrr to uint8_t, we get the low byte.
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	//This enables the receiver and transmitter by setting the RXEN0 and TXEN0 bits in the UCSR0B register. This switches a multiplexer that links the serial functions to the pins, allowing us to use the TX and RX pins for serial communication instead of digital IO.
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //This sets the frame format to 8 data bits, no parity, and 1 stop bit by setting the USBS0, UCSZ01, and UCSZ00 bits in the UCSR0C register. This is a common frame format for serial communication.

}

void serialPrintChar(char c){
	while (!(UCSR0A & (1 << UDRE0))); //Wait until the transmit buffer is empty by checking the UDRE0 bit in the UCSR0A register. This ensures that we don't overwrite any data that is currently being transmitted.
	UDR0 = c; //Put the character to be transmitted into the UDR0 register, which is the transmit buffer. This starts the transmission of the character.
}

inline bool myDigitalRead(const PinStruct target){
	return *target.pin & (1 << target.bit);
}
inline void myPinMode(const PinStruct target, bool mode){
	if(mode)	*target.ddr |= (1 << target.bit);
	else	*target.ddr &= ~(1 << target.bit);
}
inline void myDigitalWrite(const PinStruct target, bool level){
 	if(level)	*target.port |= (1 << target.bit);
  else  *target.port &= ~(1 << target.bit);
}
inline void myAnalogWrite(const PinStruct target, uint8_t level){
	if(target.PWMData.timer != nullptr){
		*target.PWMData.PWMPtr = level;
	}
}


inline void enablePWM(const PinStruct target){
	if(target.PWMData.timer != nullptr){
        	*target.PWMData.timer |= (1 << target.PWMData.modeBit);
	}
}
inline void disablePWM(const PinStruct target){
	if(target.PWMData.timer != nullptr){
		*target.PWMData.timer &= ~(1 << target.PWMData.modeBit);
    	}
}


//defines the functions necessary for setting up, running, and updating myMillis().
volatile unsigned long systemMillis = 0;
ISR(TIMER2_COMPA_vect){
	systemMillis ++;
}
inline unsigned long myMillis(void){
	unsigned long time;
	cli();
	time = systemMillis;
	sei();
	return time;
}
inline unsigned long myMicros(void){
	cli();
	uint32_t time = systemMillis * 1000UL + TCNT2 * 4;
	sei();
	return time;
}
inline void initTimer2Millis(void){
	//Inititate Timer2 for myMillis()
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	OCR2A = 249;
	TIMSK2 = (1 << OCIE2A);

	sei();
}

//this initiates Timer0 PWM for pins 5 and 6.
inline void initTimer0PWM(void){
	//Sets Pins 5 and 6 Waveform Generation Mode to Mode 3: Fast PWM
	//This means that Timer0 starts at 0, counts up to 255, and then resets to 0.
	TCCR0A = (1 << WGM00) | (1 << WGM01);
	//Sets the clock select bits to use a prescaler of 64, which gives us a PWM frequency of about 976Hz.
	TCCR0B = (1 << CS01) | (1 << CS00);
}
//This initiates Timer1 for 50Hz servo control.
inline void initTimer1Servo50Hz(void){

	myPinMode(PIN_9, OUT);
	myPinMode(PIN_10, OUT);

	//Setting the Waveform Generation Module to Mode 14: Fast PWM with ICR1 as TOP.
	//This allows us to set a custom TOP value for a specific PWM frequency, which is necessary for accurate servo control.
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12);

	//Sets the TOP value for 50Hz
	ICR1 = 39999;

	//Sets the prescaler to 8 so that it only increments the timer every 8 clock cycles
	//This, combined with the TOP value, gives us a PWM frequency of 50Hz, which is standard for servo control.
	TCCR1B |= (1 << CS11);

	//We need to set the Compare Output Mode (COM) bits to enable the multiplexer to select PWM instead of Digital.
	//This is equivalent to enablePWM
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

	//Set the motors to be stopped initially (1.5ms pulse)
	OCR1A = 3000;
	OCR1B = 3000; 

}

//Blocking tone function
inline void myTone(uint16_t freq, uint32_t duration){
	uint32_t startTime = myMillis() * 1000UL;
	uint32_t currentTime = startTime;
	uint32_t period = 1000000UL / freq;
	duration *= 1000UL;

	while(currentTime - startTime < duration){
		if(currentTime % period < period / 2){
			myDigitalWrite(PIN_2, ON);
		}
		else{
			myDigitalWrite(PIN_2, OFF);
		}
		currentTime = myMicros();
	}
	myDigitalWrite(PIN_2, OFF);
}

//Primary robot drive function
inline void drive(int8_t left, int8_t right){
	//Drives the robot. It starts by taking 3000 (1.5ms). Then it takes the left and right variables (which range from -100 to 100) and multiplies them by 4.This gives us a range from -400 to 400. Adding them to the 3000 gives us a range from 2600 to 3400, or 1.3ms to 1.7ms to control the servos. The right motor is reversed because of the way it is oriented on the robot.
	OCR1A = 3000 + (uint16_t)left * 4;
	OCR1B = 3000 - (uint16_t)right * 4;
}
inline void millisDrive(int16_t left, int16_t right){
	//This is here if I ever need direct millisecond-level control over the motors.
	OCR1A = left * 2;
	OCR1B = right * 2;
}

//drive(0,0) is used so often that I made a stop function for it.
inline void stop(void){
	drive(0,0);
}

//This is a simple blocking delay function. It functions identically to Arduio's delay() function.
inline void myDelay(unsigned long ms){
	unsigned long startTime = myMillis();
	while(myMillis() - startTime < ms);
}