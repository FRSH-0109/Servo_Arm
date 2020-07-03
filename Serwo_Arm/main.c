
#include "main.h"

float Acc_x,Acc_y,Acc_z,Temperature,Gyro_x,Gyro_y,Gyro_z;
int i=0;
float readAverageX,readAverageY,readAverageZ;
float readBufferX[50];
float readBufferY[50];
int main()
{
	char buffer[20], float_[10];
	float Xa,Ya,Za,t;
	float Xg=0,Yg=0,Zg=0;
	
	DDRB |= (1<<PORTB1) | (1<<PORTB2);
	
	I2C_Init();
	UART_Init(BAUD_RATE_9600_BPS);
	TIMER1_Init();
	MPU6050_Init();
	sei();
	while(1)
	{
		Read_RawValue();

		Xa = (Acc_x/16384.0)*2;								/* Divide raw value by sensitivity scale factor to get real values */
		Ya = (Acc_y/16384.0)*2;
		Za = Acc_z/16384.0;

		Xg = Gyro_x/16.4;
		Yg = Gyro_y/16.4;
		Zg = Gyro_z/16.4;

		t = (Temperature/340.00)+36.53;					/* Convert temperature in °/c using formula */
		//AverageOf10XYZ(Xa,Ya,Za);
		AverageOf100(Xa,Ya);
		//dtostrf(readaverageX, 4, 3, float_ );					/* Take values in buffer to send all parameters over USART */
		//sprintf(buffer,float_);
 		//UART_SendString(buffer);
 		OCR1A=readAverageX*254+762;
 		OCR1B=readAverageY*254+768;
		//NEW_LINE();
	}
}
void AverageOf100(float X,float Y)
{
	if(X>-1 && X<1){}
	else{if(X>1){X=1;}else if(X<-1){X=-1;}}
	if(Y>-1 && Y<1){}
	else{if(Y>1){Y=1;}else if(Y<-1){Y=-1;}}
	readBufferX[i]=X;
	readBufferY[i]=Y;
	readAverageX=0;
	readAverageY=0;
	for(int n=0;n<50;n++)
	{
		readAverageX+=readBufferX[n];
		readAverageY+=readBufferY[n];
	}
	readAverageX/=50;
	readAverageY/=50;
	i++;
	if(i>49){i=0;}
}
void AverageOf10XYZ(float X,float Y, float Z)
{
	float readX[10],readY[10],readZ[10];
		
	readX[i]=X;
	readY[i]=Y;
	readZ[i]=Z;
	readAverageX=(readX[0]+readX[1]+readX[2]+readX[3]+readX[4]+readX[5]+readX[6]+readX[7]+readX[8]+readX[9])/10;
	readAverageY=(readY[0]+readY[1]+readY[2]+readY[3]+readY[4]+readY[5]+readY[6]+readY[7]+readY[8]+readY[9])/10;
	readAverageZ=(readZ[0]+readZ[1]+readZ[2]+readZ[3]+readZ[4]+readZ[5]+readZ[6]+readZ[7]+readZ[8]+readZ[9])/10;
	i++;
	if(i>9){i=0;}
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

void UART_putU8(uint8_t val1)
{
	uint8_t dig1 = '0', dig2 = '0';

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

void UART_putU16(uint16_t val)
{
	uint8_t dig1 = '0', dig2 = '0', dig3 = '0', dig4 = '0';

	// count value in 10000s place
	while(val >= 10000)
	{
		val -= 10000;
		dig1++;
	}

	// count value in 1000s place
	while(val >= 1000)
	{
		val -= 1000;
		dig2++;
	}

	// count value in 100s place
	while(val >= 100)
	{
		val -= 100;
		dig3++;
	}

	// count value in 10s place
	while(val >= 10)
	{
		val -= 10;
		dig4++;
	}

	// was previous value printed?
	uint8_t prevPrinted = 0;

	// print first digit (or ignore leading zeros)
	if(dig1 != '0') {UART_putc(dig1); prevPrinted = 1;}

	// print second digit (or ignore leading zeros)
	if(prevPrinted || (dig2 != '0')) {UART_putc(dig2); prevPrinted = 1;}

	// print third digit (or ignore leading zeros)
	if(prevPrinted || (dig3 != '0')) {UART_putc(dig3); prevPrinted = 1;}

	// print third digit (or ignore leading zeros)
	if(prevPrinted || (dig4 != '0')) {UART_putc(dig4); prevPrinted = 1;}

	// print final digit
	UART_putc(val + '0');
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\r';					/* Put data into buffer, sends the data */
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\n';
	
}

void TIMER1_Init()
{
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1);
	TCCR1B |= (1<<CS11) | (1<<WGM13);																//Phase and freq correct mode,presalcer /8
	ICR1 = 10160;
	OCR1A = 1016;    //2ms
	OCR1B = 508;	//1ms
}


void UART_SendString(char *str)					/* Send string of USART data function */
{
	int i=0;
	while (str[i]!=0)
	{
		UART_putc(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
}


void I2C_Init()												/* I2C initialize function */
{
	TWBR = 2;							/* Get bit rate register value by formula */
}

uint8_t I2C_Start(char slave_write_address)						/* I2C start function */
{
	uint8_t status;											/* Declare variable */
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					/* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status != 0x08)										/* Check weather start condition transmitted successfully or not? */
	return 0;												/* If not then return 0 to indicate start condition fail */
	TWDR = slave_write_address;								/* If yes then write SLA+W in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x18)										/* Check weather SLA+W transmitted & ack received or not? */
	return 1;												/* If yes then return 1 to indicate ack received i.e. ready to accept data byte */
	if (status == 0x20)										/* Check weather SLA+W transmitted & nack received or not? */
	return 2;												/* If yes then return 2 to indicate nack received i.e. device is busy */
	else
	return 3;												/* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Repeated_Start(char slave_read_address)			/* I2C repeated start function */
{
	uint8_t status;											/* Declare variable */
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					/* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status != 0x10)										/* Check weather repeated start condition transmitted successfully or not? */
	return 0;												/* If no then return 0 to indicate repeated start condition fail */
	TWDR = slave_read_address;								/* If yes then write SLA+R in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x40)										/* Check weather SLA+R transmitted & ack received or not? */
	return 1;												/* If yes then return 1 to indicate ack received */
	if (status == 0x20)										/* Check weather SLA+R transmitted & nack received or not? */
	return 2;												/* If yes then return 2 to indicate nack received i.e. device is busy */
	else
	return 3;												/* Else return 3 to indicate SLA+W failed */
}

void I2C_Stop()												/* I2C stop function */
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);					/* Enable TWI, generate stop condition and clear interrupt flag */
	while(TWCR & (1<<TWSTO));								/* Wait until stop condition execution */
}

void I2C_Start_Wait(char slave_write_address)				/* I2C start wait function */
{
	uint8_t status;											/* Declare variable */
	while (1)
	{
		TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);				/* Enable TWI, generate start condition and clear interrupt flag */
		while (!(TWCR & (1<<TWINT)));						/* Wait until TWI finish its current job (start condition) */
		status = TWSR & 0xF8;								/* Read TWI status register with masking lower three bits */
		if (status != 0x08)									/* Check weather start condition transmitted successfully or not? */
		continue;											/* If no then continue with start loop again */
		TWDR = slave_write_address;							/* If yes then write SLA+W in TWI data register */
		TWCR = (1<<TWEN)|(1<<TWINT);						/* Enable TWI and clear interrupt flag */
		while (!(TWCR & (1<<TWINT)));						/* Wait until TWI finish its current job (Write operation) */
		status = TWSR & 0xF8;								/* Read TWI status register with masking lower three bits */
		if (status != 0x18 )								/* Check weather SLA+W transmitted & ack received or not? */
		{
			I2C_Stop();										/* If not then generate stop condition */
			continue;										/* continue with start loop again */
		}
		break;												/* If yes then break loop */
	}
}

uint8_t I2C_Write(char data)								/* I2C write function */
{
	uint8_t status;											/* Declare variable */
	TWDR = data;											/* Copy data in TWI data register */
	TWCR = (1<<TWEN)|(1<<TWINT);							/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8;									/* Read TWI status register with masking lower three bits */
	if (status == 0x28)										/* Check weather data transmitted & ack received or not? */
	return 0;												/* If yes then return 0 to indicate ack received */
	if (status == 0x30)										/* Check weather data transmitted & nack received or not? */
	return 1;												/* If yes then return 1 to indicate nack received */
	else
	return 2;												/* Else return 2 to indicate data transmission failed */
}

char I2C_Read_Ack()										/* I2C read ack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);					/* Enable TWI, generation of ack and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (read operation) */
	return TWDR;											/* Return received data */
}

char I2C_Read_Nack()										/* I2C read nack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT);								/* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1<<TWINT)));							/* Wait until TWI finish its current job (read operation) */
	return TWDR;											/* Return received data */
}


void MPU6050_Init()										/* Gyro initialization function */
{
	_delay_ms(150);										/* Power up time >100ms */
	I2C_Start_Wait(I2C_WRITE_ADDRESS);								/* Start with device write address */
	I2C_Write(SMPLRT_DIV);								/* Write to sample rate register */
	I2C_Write(0x07);									/* 1KHz sample rate */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);								/* Write to power management register */
	I2C_Write(0x01);									/* X axis gyroscope reference frequency */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);									/* Write to Configuration register */
	I2C_Write(0x00);									/* Fs = 8KHz */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);								/* Write to Gyro configuration register */
	I2C_Write(0x18);									/* Full scale range +/- 2000 degree/C */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);								/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(I2C_WRITE_ADDRESS);								/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);							/* Write start location address from where to read */
	I2C_Repeated_Start(I2C_READ_ADDRESS);							/* I2C start with device read address */
}

void Read_RawValue()
{
	MPU_Start_Loc();									/* Read Gyro values */
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Temperature = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
}

void NEW_LINE()
{
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\r';					/* Put data into buffer, sends the data */
	
	while (!( UCSR0A & (1<<UDRE0)));	 /* Wait for empty transmit buffer       */

	UDR0 = '\n';
}
