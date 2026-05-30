#include "hal.h"
#include "i2c.h"
#include "exti.h"

volatile uint32_t s_ticks;
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

int main(void) {
  uint16_t led = PIN('A', 5);

  gpio_set_mode(PIN('A', 2), GPIO_MODE_AF);
  gpio_set_af(PIN('A', 2), 1U);

  RCC->APBENR1 |= BIT(17); //USART2EN
  uart_init(USART2, 115200);
  systick_init(16000000 / 1000);

  gpio_set_mode(led, GPIO_MODE_OUTPUT);
  i2c_init();
  
  uint32_t period = 1000, timer = 0;
  bool on = true;

  /*i2c_read_at_beginning();*/
  spin(50000);
  /*unsigned char onCounter = i2c_get_last_read_byte();*/
  /*volatile unsigned char test1 = 0;*/

  configureExtiForButtons();
  
  uint16_t red = PIN('A', 6);
  gpio_set_mode(red, GPIO_MODE_OUTPUT);

  uint16_t yellow = PIN('A', 7);
  gpio_set_mode(yellow, GPIO_MODE_OUTPUT);

  uint16_t green = PIN('A', 8);
  gpio_set_mode(green, GPIO_MODE_OUTPUT);
 
  while (1) {
    if (isTimerTick(&timer, period)) {
      gpio_write(led, on);

      if (on) {
        /*onCounter++;*/
        uart_write_string(USART2, "It's ON!\n\r", 10);
        /*i2c_write_byte(onCounter);*/
      } else {
        uart_write_string(USART2, "It's OFF!\n\r", 11);
        /*i2c_read_at_beginning();*/
      }

      gpio_write(red, isRedOn());
      gpio_write(yellow, isYellowOn());
      gpio_write(green, isGreenOn());

      on = !on;
      /*test1 = i2c_get_last_read_byte();*/
    }
  
  /*if (test1){};*/
  }
  return 0;
}

// Startup code
__attribute__((naked, noreturn)) void _reset(void) {          //naked=don't generate prologue/epilogue  //noreturn=it will never exit
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;          //linker symbols declared in the .ld file
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();
  while(1);
}

extern void _estack(void);  // Defined in link.ld

// 16 standard and 32 STM32-specific handlers
// PM0223 2.3.4
__attribute__((section(".vectors"))) void (*const tab[16 + 32])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler,
    0, 0, 0, 0, 0, 0, 0, EXTI4_15_Handler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, I2C1_Global_Interrupt
};
