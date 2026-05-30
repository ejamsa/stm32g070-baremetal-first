#ifndef I2C_H
#define I2C_H

#include <stdint.h>

struct i2c { volatile uint32_t CR1, CR2, OAR1, OAR2, TIMINGR, TIMEOUTR, ISR, ICR, PECR; volatile unsigned char X1, X2, X3, RXDR, X4, X5, X6, TXDR; };
#define I2C1 ((struct i2c *) (0x40005400))

#define I2C_CR1_PE       0
#define I2C_CR1_TXIE     1
#define I2C_CR1_RXIE     2
#define I2C_CR1_NACKIE   4
#define I2C_CR1_STOPIE   5
#define I2C_CR1_TCIE     6
#define I2C_CR1_ERRIE    7
#define I2C_CR1_DNF_0    8
#define I2C_CR1_DNF_1    9
#define I2C_CR1_DNF_2   10
#define I2C_CR1_DNF_3   11 
#define I2C_CR1_ANFOFF  12

#define I2C_CR2_RD_WRN  10
#define I2C_CR2_START   13
#define I2C_CR2_NBYTES  16
#define I2C_CR2_AUTOEND 25

#define I2C_ANALOG_NOISE_DISABLED 1

#define I2C_TIMINGR_SCLL     0
#define I2C_TIMINGR_SCLH     8
#define I2C_TIMINGR_SDADEL  16
#define I2C_TIMINGR_SCLDEL  20
#define I2C_TIMINGR_PRESC   28

#define I2C_ISR_TXE   0 //Transmist data register (TXDR) empty
#define I2C_ISR_TXIS  1 //TXDR is empty and must be filled
#define I2C_ISR_RXNE  2 //Receive data is ready (RXDR)
#define I2C_ISR_NACKF 4 //NACK is received
#define I2C_ISR_STOPF 5 //STOP detected
#define I2C_ISR_TC    6 //Transfer Complete

#define PIN_PB8 PIN('B', 8)
#define PIN_PB9 PIN('B', 9)

enum TransferDirection { WRITE, READ };

void i2c_pin_init(void);
void i2c_init(void);
void i2c_write_byte(unsigned char byteToWrite);
void i2c_read_at_beginning(void);
void I2C1_Global_Interrupt(void);

unsigned char i2c_get_last_read_byte(void);

#endif
