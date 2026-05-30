#include "i2c.h"
#include "hal.h"

#define EEPROM_ADR 0b1010000U

void i2c_clock_init() {
  RCC->APBENR1 |= BIT(21U);
}

void i2c_pin_init() {
  gpio_set_mode(PIN_PB8, GPIO_MODE_AF); //SCL
  gpio_set_af(PIN_PB8, 6U);

  gpio_set_mode(PIN_PB9, GPIO_MODE_AF); //SDA
  gpio_set_af(PIN_PB9, 6U);
}

void i2c_init() {
  i2c_clock_init();
  i2c_pin_init();

  I2C1->CR1 &= ~((uint32_t)1 << I2C_CR1_PE);

  I2C1->CR1 |= (I2C_ANALOG_NOISE_DISABLED << I2C_CR1_ANFOFF);

  // I2C master clock must be configured by setting the SCLH and SCLL bits in the I2C_TIMINGR register.
  // Standard mode (10kHz), I2CCLK = 16MHz - RM0454 Table 110
  I2C1->TIMINGR |= (3U << I2C_TIMINGR_PRESC) | (4U << I2C_TIMINGR_SCLDEL) | (2U << I2C_TIMINGR_SDADEL) | (0xC3 << I2C_TIMINGR_SCLH) | (0xC7 << I2C_TIMINGR_SCLL);

  I2C1->CR1 |= BIT(I2C_CR1_TXIE) | BIT(I2C_CR1_RXIE) | BIT(I2C_CR1_NACKIE) | BIT(I2C_CR1_STOPIE) | BIT(I2C_CR1_TCIE) | BIT(I2C_CR1_ERRIE) | BIT(I2C_CR1_PE);

  i2c_pin_init();
}

void i2c_init_communication(enum TransferDirection direction, char nrOfBytes) {
  I2C1->CR2 |= BIT(I2C_CR2_AUTOEND) | (nrOfBytes << I2C_CR2_NBYTES) | BIT(I2C_CR2_START) | (direction << I2C_CR2_RD_WRN) | (EEPROM_ADR << 0);

}

static unsigned char bytesToWrite[] = { 0, 0, 0 };
static volatile unsigned char writeIndex = 0;
void i2c_write_byte(unsigned char byteToWrite) {
  writeIndex = 0;
  bytesToWrite[2] = byteToWrite;
  i2c_init_communication(WRITE, 3U); // 2 address bytes + 1 data byte

  //write first byte
  /*I2C1->TXDR = bytesToWrite[writeIndex];*/
  /*writeIndex++;*/
}

static unsigned char lastReadByte = 0;
unsigned char i2c_get_last_read_byte(void) {
  return lastReadByte;
}

void I2C1_Global_Interrupt(void) {

  if (I2C1->ISR & BIT(I2C_ISR_TXIS)) {
    I2C1->TXDR = bytesToWrite[writeIndex];
    writeIndex++;
  } else if (I2C1->ISR & BIT(I2C_ISR_TC) && (I2C1->ISR & BIT(I2C_ISR_TXIS))) {
    writeIndex = 0;
  } else if (I2C1->ISR & BIT(I2C_ISR_RXNE) && (I2C1->ISR & BIT(I2C_ISR_TC))) {
    lastReadByte = I2C1->RXDR; 
  }
}

//READ
void i2c_current_read() {
  i2c_init_communication(READ, 1U);
}

void i2c_read_at_beginning() {
  i2c_write_byte(0U); //Set word adress to 0

  //Wait till write is done
  while(!(I2C1->ISR & BIT(I2C_ISR_TC)) && !(I2C1->ISR & BIT(I2C_ISR_NACKF))) asm("nop");

  //adress is set to 0, read current word
  i2c_current_read();
}


