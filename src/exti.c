#include "exti.h"

#include "hal.h"

void configureExtiForButtons(void) {

  uint16_t red_button = PIN('C', 10);
  gpio_set_mode(red_button, GPIO_MODE_INPUT);
  gpio_set_pupd(red_button, GPIO_PUPD_PU);

  uint16_t yellow_button = PIN('C', 11);
  gpio_set_mode(yellow_button, GPIO_MODE_INPUT);
  gpio_set_pupd(yellow_button, GPIO_PUPD_PU);

  uint16_t green_button = PIN('C', 12);
  gpio_set_mode(green_button, GPIO_MODE_INPUT);
  gpio_set_pupd(green_button, GPIO_PUPD_PD);
  
  EXTI->IMR1 |= (BIT(10U) | BIT(11U) | BIT(12U));
 
  //PC10 for EXTI10
  //PC11 for EXTI11
  //PC12 for EXTI12
  EXTI->EXTICR3 |= (0x2U << 24U) | (0x2U << 16); 
  EXTI->EXTICR4 |= (0x2U); // (0x2U << 24U) | (0x2U << 16) | (0x2U << 8);
  
  //Rising edge trigger
  EXTI->RTSR1 |= (BIT(10U) | BIT(11U) | BIT(12U));

  *NVIC_ISER |= BIT(7U);
}

static bool redIsOn = false;
static uint32_t lastRedTick = 0;

static bool yellowIsOn = false;
static uint32_t lastYellowTick = 0;

static bool greenIsOn = false;
static uint32_t lastGreenTick = 0;

static unsigned char redPin = 10U;
static unsigned char yellowPin = 11U;
static unsigned char greenPin = 12U;
void EXTI4_15_Handler(void) {
  
  if (EXTI->RPR1 & BIT(redPin)) {
    EXTI->RPR1 |= BIT(redPin); //reset the interrupt

    if (s_ticks - lastRedTick < 200) return;

    redIsOn = !redIsOn;
    lastRedTick = s_ticks;

  } else if (EXTI->RPR1 & BIT(yellowPin)) {
    EXTI->RPR1 |= BIT(yellowPin);

    if (s_ticks - lastYellowTick < 200) return;

    yellowIsOn = !yellowIsOn;
    lastYellowTick = s_ticks;

  } else if (EXTI->RPR1 & BIT(greenPin)) {
    EXTI->RPR1 |= BIT(greenPin);

    if (s_ticks - lastGreenTick < 200) return;

    greenIsOn = !greenIsOn;
    lastGreenTick = s_ticks;
  }

}


bool isRedOn(void) {
  return redIsOn;
}

bool isYellowOn(void) {
  return yellowIsOn;
}

bool isGreenOn(void) {
  return greenIsOn;
}

