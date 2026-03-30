#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

struct systick {
  volatile uint32_t CSR, RVR, CVR, CALIB;
};
#define SYSTICK ((struct systick *) 0xE000E010)

struct rcc {
  volatile uint32_t CR, ICSCR, CFGR, PLL, RESERVED1, RESERVED2, CIER, CIFR, CICR, 
        IOPRSTR, AHBRSTR, APBRSTR1, APBRSTR2, IOPENR, AHBENR, APBENR1, APBENR2,
        IOPSMENR, AHBSMENR, APBSMENR1, APBSMENR2, CCIPR, CCIPR2, BDCR, CSR;
};
#define RCC ((struct rcc *) 0x40021000)

struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
#define GPIO(bank) ((struct gpio *) (0x50000000 + (0x400 * (bank))))
#define GPIOA ((struct gpio *) 0x50000000)

struct usart {
  volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR, PRESC;
};
#define USART1 ((struct usart *) 0x40013800)
#define USART2 ((struct usart *) 0x40004400)

// Enum values are per datasheet: 0, 1, 2, 3
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  int n = PINNO(pin);
  RCC->IOPENR |= BIT(PINBANK(pin));

  gpio->MODER &= ~(3U << (n * 2));         // Clear existing setting
  gpio->MODER |= (mode & 3U) << (n * 2);   // Set new mode
}

static inline void gpio_set_af(uint16_t pin, uint8_t af_num) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  int n = PINNO(pin);
  gpio->AFR[n >> 3] &= ~(15UL << ((n & 7) * 4));
  gpio->AFR[n >> 3] |= ((uint32_t) af_num) << ((n & 7) * 4);
}

static inline void gpio_write(uint16_t pin, bool val) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0U : 16U);
}

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}

static inline void systick_init(uint32_t ticks) {
  if ((ticks - 1) > 0xffffff) return;  // Systick timer is 24 bit
  
  SYSTICK->RVR = ticks - 1;
  SYSTICK->CVR = 0;
  SYSTICK->CSR = BIT(0) | BIT(1) | BIT(2);

  RCC-> APBENR2 = 0x1U;
}

static volatile uint32_t s_ticks; 
void SysTick_Handler(void) {
  s_ticks++;
}

bool isTimerTick(uint32_t *timer, uint32_t period) {
  if (s_ticks + period < *timer) *timer = 0;
  if (*timer == 0) *timer = s_ticks + period;
  if (*timer > s_ticks) return false;

  *timer = (s_ticks - *timer) > period ? s_ticks + period : *timer + period;
  return true;
}

static inline void uart_init(struct usart *uart, unsigned long baud_rate) {
  // USART default clock is PCLK.
  // PCLK is APB clock with prescaler set to 1.
  // HCLK is AHB clock with prescaler set to 1.
  // SYSCLK default i HSISYS.
  // HSISYS divider default is 1, thus SYSCLK is 16MHz.
  // 2. Select the desired baud rate using the USART_BRR register.
  uart->CR1 = 0;  // Disable uart, to write BRR
  uart->BRR = 16000000 / baud_rate;
  // 3. Program the number of stop bits in USART_CR2.
  // 4. Enable the USART by writing the UE bit in USART_CR1 register to 1.
  uart->CR1 |= BIT(3) | BIT(0);  // enable TE and UE

  // 5. Set the TE bit in USART_CR1 to send an idle frame as first transmission.

}

static inline void uart_write_byte(struct usart *uart, uint8_t byte) {
  // 1. Write the data to send in the USART_TDR register. Repeat this for each data to be transmitted in case of single buffer.
  uart->TDR = byte;
  // 2. When the last data is written to the USART_TDR register, wait until TC = 1.
  while(!(uart->ISR & BIT(6))) { //BIT 6 is TC (Transmission complete)
    asm("nop");
  }
}

static inline void uart_write_string(struct usart *uart, char *text, int len) {
  while(len-- > 0) uart_write_byte(uart, *((uint8_t *)text++));
}


