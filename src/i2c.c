#include "i2c.h"
#include "hal.h"

void i2c_clock_init() {
  RCC->APBENR1 |= BIT(21U);
}

void i2c_pin_init() {
  gpio_set_mode(PIN_PB6, GPIO_MODE_AF); //SCL
  gpio_set_af(PIN_PB6, 6U);

  gpio_set_mode(PIN_PB7, GPIO_MODE_AF); //SDA
  gpio_set_af(PIN_PB7, 6U);
}

void i2c_init() {
  I2C1->CR1 &= ~((uint32_t)1 << I2C_CR1_PE);

  I2C1->CR1 |= (I2C_ANALOG_NOISE_DISABLED << I2C_CR1_ANFOFF);

  // I2C master clock must be configured by setting the SCLH and SCLL bits in the I2C_TIMINGR register.
  // Standard mode (10kHz), I2CCLK = 16MHz - RM0454 Table 110
  I2C1->TIMINGR |= (3U << I2C_TIMINGR_PRESC) | (4U << I2C_TIMINGR_SCLDEL) | (2U << I2C_TIMINGR_SDADEL) | (0xC3 << I2C_TIMINGR_SCLH) | (0xC7 << I2C_TIMINGR_SCLL);

  I2C1->CR1 |= BIT(I2C_CR1_TXIE) | BIT(I2C_CR1_RXIE) | BIT(I2C_CR1_NACKIE) | BIT(I2C_CR1_STOPIE) | BIT(I2C_CR1_TCIE) | BIT(I2C_CR1_ERRIE) | BIT(I2C_CR1_PE);

  i2c_pin_init();
}

void I2C1_Global_Interrupt(void) {
  
}
