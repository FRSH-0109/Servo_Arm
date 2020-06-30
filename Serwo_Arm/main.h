

#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL
#define BAUD_RATE_9600_BPS  52 // 9600bps
#define I2C_WRITE_ADDRESS 0b11010000
#define I2C_READ_ADDRESS 0b11010001

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>									/* Include integer type header file */
#include <stdlib.h>										/* Include standard library file */
#include <stdio.h>
#include <math.h>
#include "MPU6050_res_define.h"

void UART_Init(unsigned int ubrr);
void UART_putc(unsigned char data);
void UART_putU8(uint8_t val1);
void UART_putU16(uint16_t val);
void UART_SendString(char *str);

void TIMER1_Init();

void I2C_Init();								/* I2C initialize function */
uint8_t  I2C_Start(char);						/* I2C start function */
uint8_t  I2C_Repeated_Start(char);				/* I2C repeated start function */
void I2C_Stop();								/* I2C stop function */
void I2C_Start_Wait(char);						/* I2C start wait function */
uint8_t  I2C_Write(char);						/* I2C write function */
char I2C_Read_Ack();							/* I2C read ack function */
char I2C_Read_Nack();

void MPU6050_Init();
void MPU_Start_Loc();
void Read_RawValue();

void NEW_LINE();




#endif /* INCFILE1_H_ */