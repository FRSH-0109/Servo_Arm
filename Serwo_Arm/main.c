
#define F_CPU 8000000UL
#define BAUD_RATE_9600_BPS  52 // 9600bps
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void UART_Init(unsigned int ubrr);
void UART_putc(unsigned char data);
void UART_putU8(uint8_t val,uint8_t val1);
void ADC_Init();	
void TIMER0_Init();		
void TIMER1_Init();			

volatile uint8_t adc4read;
volatile uint8_t adc5read;
volatile uint8_t i=0;
volatile uint16_t j=0;
volatile uint8_t serwo1;
volatile uint8_t serwo2;

int main()
{
	DDRB |= (1<<PB1) | (1<<PB2);
	UART_Init(BAUD_RATE_9600_BPS);
	TIMER0_Init();
	ADC_Init();
	TIMER1_Init();
	sei();
	while(1) 
	{
		UART_putU8(adc4read,adc5read);
		serwo1=adc4read*0.2+50;
		serwo2=adc5read*0.2+50;
	}
}

void UART_Init(unsigned int ubrr)
{
	UBRR0H = (ubrr>>8); // Shift the 16bit value ubrr 8 times to the right and transfer the upper 8 bits to UBBR0H register.
	UBRR0L = (ubrr);    // Copy the 16 bit value ubrr to the 8 bit UBBR0L register,Upper 8 bits are truncated while lower 8 bits are copied
	
	UCSR0C = 0x06;       /* Set frame format: 8data, 1stop bit  */
	UCSR0B = (1<<TXEN0); /* Enable  transmitter                 */
	
}

void UART_putc(unsigned char data)
{
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = data;					/* Put data into buffer, sends the data */
					/* Put data into buffer, sends the data */
}

void UART_putU8(uint8_t val,uint8_t val1)
{
	uint8_t dig1 = '0', dig2 = '0';

	// count value in 100s place
	while(val >= 100)
	{
		val -= 100;
		dig1++;
	}

	// count value in 10s place
	while(val >= 10)
	{
		val -= 10;
		dig2++;
	}

	// print first digit (or ignore leading zeros)
	if(dig1 != '0') UART_putc(dig1);

	// print second digit (or ignore leading zeros)
	if((dig1 != '0') || (dig2 != '0')) UART_putc(dig2);

	// print final digit
	UART_putc(val + '0');
	
	 dig1 = '0', dig2 = '0';

	// count value in 100s place
	while(val1 >= 100)
	{
		val1 -= 100;
		dig1++;
	}

	// count value in 10s place
	while(val1 >= 10)
	{
		val1 -= 10;
		dig2++;
	}

	// print first digit (or ignore leading zeros)
	if(dig1 != '0') UART_putc(dig1);

	// print second digit (or ignore leading zeros)
	if((dig1 != '0') || (dig2 != '0')) UART_putc(dig2);

	// print final digit
	UART_putc(val1 + '0');
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\r';					/* Put data into buffer, sends the data */
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\n';
	
}

void ADC_Init()
{
	ADCSRA |= (1<<ADEN) | (1<<ADATE) | (1<< ADIE) | (1<< ADPS0) ;	//ADC enebale, Auto Trigger Enable, Interupt enable,
	ADCSRB |= (1<<ADTS2);											//trigger source na TIM0 overflow
	ADMUX |= (1<<MUX2) | (1<<ADLAR) | (1<<REFS0);					//let shift dla wyniku ADC
}

void TIMER0_Init()
{
	TCCR0B |= (1<<CS02) | (1<<CS00);																//prescaler timer0 na 1024
	TIMSK0 |= (1<<TOIE0);																			//overflow interrupt enable
}

void TIMER1_Init()
{
	 TCCR1B |= (1<<WGM12) | (1<<CS11);																//CTC,presalcer /8
	 TIMSK1 |=  (1<<TIMSK1) | (1<<OCIE1A);
	 OCR1AL = 20;
}

ISR(ADC_vect)
{
	if(!(ADMUX & (1<<MUX0)))
	{
		adc4read=ADCH;
		ADMUX |= (1<<MUX0);	
	}
	else
	{
		adc5read=ADCH;
		ADMUX &= ~(1<<MUX0);
		i++;
		if(i>9){i=0;}
	}
	}
	

ISR(TIMER0_OVF_vect)
{
	
}

ISR(TIMER1_COMPA_vect)
{
	if(j==0){PORTB |= (1<<PORTB1) | (1<<PORTB2);}
	if(j==serwo1){PORTB &= ~(1<<PORTB1);}
	if(j==serwo2){PORTB &= ~(1<<PORTB2);}
	j++;
	if(j==1001){j=0;}
}




















