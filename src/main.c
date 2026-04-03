#include "hal.h"

int main(void) {
  uint16_t led = PIN('A', 5);

  gpio_set_mode(PIN('A', 2), GPIO_MODE_AF);
  gpio_set_af(PIN('A', 2), 1U);

  RCC->APBENR1 |= BIT(17); //USART2EN
  uart_init(USART2, 115200);
  systick_init(16000000 / 1000);

  gpio_set_mode(led, GPIO_MODE_OUTPUT);

  uint32_t period = 1000, timer = 0;
  bool on = true;
  
  while (1) {
    if (isTimerTick(&timer, period)) {
      gpio_write(led, on);

      if (on) uart_write_string(USART2, "It's ON!\n\r", 10);
      else uart_write_string(USART2, "It's OFF!\n\r", 11);
      on = !on;
    }

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
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler
};
