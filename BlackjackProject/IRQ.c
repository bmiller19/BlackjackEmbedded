/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: MCB1700 IRQ Handler
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.H" 
//#include "LED.h"
#include "ADC.h"


extern uint16_t AD_last;  // Last converted value  
extern uint8_t  AD_done;  // AD conversion done flag   
unsigned char   clock_1s; // Flag activated each second


/*----------------------------------------------------------------------------
  SysTick IRQ: Executed periodically
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) // SysTick Interrupt Handler (10ms);
{           
  static unsigned long ticks;
  static unsigned long timetick;
  //static unsigned char leds = 0x01;

  // (1) Set clock_1s to 1 every 1 second;
  if (ticks++ >= 99) { 
    ticks    = 0;
    clock_1s = 1;
  }

  // (2) Blink the LEDs depending on ADC_ConvertedValue;
  if (timetick++ >= (AD_last >> 8)) {
    timetick   = 0;
  }

  // (3) is the AD conversion taking less than 10 ms or not?
  ADC_StartCnv();
}


/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is done
 *----------------------------------------------------------------------------*/
void ADC_IRQHandler(void) 
{
  // (1)
  volatile uint32_t adstat;
  adstat = LPC_ADC->ADSTAT;  // Reading ADC clears interrupt;

  // (2)
  AD_last = (LPC_ADC->ADGDR >> 4) & ADC_VALUE_MAX; // Store result of A/D conversion;
  
  // (3) record conversion finished; this will be seen inside main();
  AD_done = 1;
}
