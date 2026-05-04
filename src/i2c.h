#ifndef I2C_H
#define I2C_H

#include <stdint.h>

struct i2c { volatile uint32_t CR1, CR2, OAR1, OAR2, TIMINGR, TIMEOUTR, ISR, ICR, PECR, RXDR, TXDR; };
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

#define I2C_ANALOG_NOISE_DISABLED 1

#define I2C_TIMINGR_SCLL     0
#define I2C_TIMINGR_SCLH     8
#define I2C_TIMINGR_SDADEL  16
#define I2C_TIMINGR_SCLDEL  20
#define I2C_TIMINGR_PRESC   28

#define PIN_PB6 PIN('B', 6)
#define PIN_PB7 PIN('B', 7)

void i2c_pin_init(void);
void i2c_init(void);
void I2C1_Global_Interrupt(void);

#endif
