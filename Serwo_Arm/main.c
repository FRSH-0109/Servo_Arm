
# define F_CPU 1000000UL
#include <stdint.h>
#include <avr/io.h>

#define BAUD_RATE_9600_BPS  52 // 9600bps
void UART_Init(unsigned int ubrr);
void UART_putc(unsigned char data);
void UART_putU8(uint8_t val);
unsigned char test = 52;
int main()
{
	UART_Init(BAUD_RATE_9600_BPS);
	while(1) 
	{
		UART_putU8(test);
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

void UART_putU8(uint8_t val)
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
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\r';					/* Put data into buffer, sends the data */
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\n';
	
}






















