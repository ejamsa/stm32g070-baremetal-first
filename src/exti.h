#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include <stdbool.h>

struct exti {
  volatile uint32_t RTSR1, FTSR1, SWIER1, RPR1, FPR1, RESERVED[19],
      EXTICR1, EXTICR2, EXTICR3, EXTICR4, RESERVED2[4], 
      IMR1, EMR1;
};

#define EXTI ((struct exti *) 0x40021800)


void configureExtiForButtons(void);

bool isRedOn(void);
bool isYellowOn(void);
bool isGreenOn(void);

void EXTI4_15_Handler(void);


#endif
