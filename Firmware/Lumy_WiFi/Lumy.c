/*************************************************************************
Title:    Lumy Control - Control of 3x TI LM3404 LED Driver over WiFi with Wifly module (RN-VX)
Author:   EP
File:     $Id: Lumy.c,v 1.0 2013/07/10 11:46:30 $
Software: AVR-GCC 4.1
Hardware: ATmega328P at 8 Mhz, 


*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "uart.h"
#include "wifly1.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* 9600 baud */
#define UART_BAUD_RATE    9600

/* End of line */
#define CR "\r\n"
#define ZERO "\n"

// LED Defines
#define LED_DDR     DDRD        // DDRA, DDRB...
#define LED_PORT    PORTD       // PORTA, PORTB...
#define LED_PORTPIN PD2         // PA0, PA1..., PB0, ...

#define INVERT_PWM 0//(1 << COM1A0)


#define BODY_BUFFER_SIZE 64
#define BUFFER_SIZE 8



//Prototypen
void uart_gets(char *Buffer, int Size);
void uart_Debug(char *Buffer, int Size);

//Globale Variablen
char rxBuffer[BODY_BUFFER_SIZE];
char commBuffer[BUFFER_SIZE];
unsigned char flag_ISDNewData = 0;
unsigned char flag_WaitForData = 0;
unsigned char flag_Steuerung = 0;
char responseBuffer[BODY_BUFFER_SIZE];
unsigned char step = 0;
const char FixedPrompt[] = "CMD";
unsigned int Grafzahl; 
unsigned int ergebnis;
int Size;


// 10-Bit PWM with 64 different settings
 
void pwm_10_64_1 (int8_t LightVal1)
{ 
    // 10 Bit Fast PWM
    TCCR0A = 0x83 | INVERT_PWM;
    // prescaler 64 -> ~122 Hz PWM frequency
    TCCR0B = 0x03;//(1 << WGM02) | 3;
 
 	OCR0A = LightVal1;
    //pwm_up_down (pwmtable_10, 64, delay);
}

//void pwm_10_64_2 (int16_t LightVal2)
//{ 
    // 10 Bit Fast PWM
//    TCCR1A = 0x83 | INVERT_PWM;
    // prescaler 64 -> ~122 Hz PWM frequency
//    TCCR1B = (1 << WGM12) | 3;
 
// 	OCR1A = LightVal2;
    //pwm_up_down (pwmtable_10, 64, delay);
//} 

void pwm_10_64_3 (int16_t LightVal2, int16_t LightVal3 )
{ 
    // 10 Bit Fast PWM
    TCCR1A = 0xA3 | INVERT_PWM;
    // prescaler 64 -> ~122 Hz PWM frequency
    TCCR1B = (1 << WGM12) | 3;
 
 	OCR1A = LightVal2;
 	OCR1B = LightVal3;
    //pwm_up_down (pwmtable_10, 64, delay);
} 

int main(void)
{
  

    //LED_DDR |= (1<<LED_PORTPIN);
	DDRC = 0x02;
	DDRD = 0x68;
	DDRB = 0x06;

    uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU) );
    sei();

	uart_flush();
	_delay_ms(300);
	uart_puts("$$$");
	uart_flush();
	//	uart_puts("\r");	
	//_delay_ms(300);
	//uart_puts("$$$");
	//uart_flush();
	
	while(1)
	{	
		PORTC |= (1<<PC1);	
		
		if( uart_available() > 0 )
		{					
			uart_gets(rxBuffer, BODY_BUFFER_SIZE);
			//uart_Debug(rxBuffer, BODY_BUFFER_SIZE);
			
			if (strcmp(rxBuffer, "CMD") == 0)
			{
				LED_PORT &= ~(1<<LED_PORTPIN);
				flag_WaitForData = 1;
				uart_flush();
			}	
			
			if (strcmp(rxBuffer, "<2.30>") == 0)
			{
				LED_PORT &= ~(1<<LED_PORTPIN);
				flag_ISDNewData = 1;
				uart_flush();
			}

			if (strcmp(rxBuffer, "*OPEN*") == 0)
			{
				flag_Steuerung = 1;
				uart_flush();
			}
        }
	
		if(flag_WaitForData == 1)
		//rxBuffer[count] = '\0';
		//uart_puts("OK4");
		//uart_puts("\n");
		{
			LED_PORT |= (1<<LED_PORTPIN);
      		//flag_ISDNewData = 1;
			uart_flush();
			_delay_ms(1000);
			uart_puts("set wlan ssd htc\r");
		    flag_WaitForData = 0;
			//	uart_puts("\r");	
			//}

		}
  		
		if(flag_ISDNewData == 1)
		//rxBuffer[count] = '\0';
		//uart_puts("OK4");
		//uart_puts("\n");
		{
		    //LED_PORT |= (1<<LED_PORTPIN);
      	    //flag_ISDNewData = 1;
		    uart_flush();
		    _delay_ms(1000);
		    uart_puts("exit\r");
			flag_ISDNewData = 0;
		}

		while(flag_Steuerung == 1)
		//rxBuffer[count] = '\0';
		//uart_puts("OK4");
		//uart_puts("\n");
		{
			//LED_PORT |= (1<<LED_PORTPIN);
			if( uart_available() >0 )
			{					
				uart_gets(commBuffer, BUFFER_SIZE);
				//uart_Debug(commBuffer, BUFFER_SIZE);				

				if (atoi(commBuffer) != 0)
				{
					//LED_PORT &= ~(1<<LED_PORTPIN);
    	  			//flag_ISDNewData = 1;
					//uart_puts("LED_AN!");
					Grafzahl = atoi(commBuffer);
					ergebnis = Grafzahl * 10;
					itoa(ergebnis, responseBuffer, 10);
					//PORTB |= (1<<PB1);//LED_PORTPIN);
					pwm_10_64_1 (ergebnis);
					pwm_10_64_3 (ergebnis, ergebnis);
					uart_puts(responseBuffer);
					//uart_puts("\r");
					uart_flush();
				}
				else
				{
					//PORTB &= ~(1<<PB1);
					//LED_PORT &= ~(1<<LED_PORTPIN);
					pwm_10_64_1 (0);
					pwm_10_64_3 (0,0);
					uart_flush();
				}	
			}

		}	

  	}
}


/* Diese Funktion sammelt Zeichen aus dem uart-Ringbuffer ein bis ein End of String (\r) empfangen wird 
und schreibt sie in ein Array bis das Array gefüllt wird.*/
void uart_gets(char *Buffer, int Size)
{
	unsigned int nextChar = 0;
	int count = 0;	
	
	//if(uart_available > 0)
	//{					
		//_delay_ms(10);

		while(count < Size-1)
		{	
			nextChar = uart_getc();
			_delay_ms(5);
			
			if (nextChar != '\r' && nextChar != '\n')
			{	
			Buffer[count] = nextChar;
			count++;
			}
		}
		count = 0;
		uart_flush();
	//}
}/*uart_gets*/


/* Dies Funktion schreibt Zeichenweise den Inhalt des Arrays in den uart-Ringbuffer, 
wenn der Inhalt ungleich Null ist. */
void uart_Debug(char *Buffer, int Size)
{
	int count = 0;

	if (*Buffer != '\0' )
	{	
		while( count <  Size )
		{
			uart_putc(Buffer[count]);
			count++;
		}
		count = 0;
		uart_putc('\r');
		//uart_puts(rxBuffer);
		//uart_putc('\r');
	}
}/*uart_Debug*/


