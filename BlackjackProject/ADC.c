/*----------------------------------------------------------------------------
**                   http://www.PowerAVR.com
**                   http://www.PowerMCU.com
**--------------File Info-----------------------------------------------------
** File name:           adc.c
** Last modified Date:  2010-05-12
** Last Version:        V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------
** Created by:          PowerAVR
** Created date:        2010-05-10
** Version:             V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------*/

#include "lpc17xx.h"   
#include "ADC.h"


uint16_t AD_last;     // Last converted value    
uint8_t  AD_done = 0; // AD conversion done flag  


/*----------------------------------------------------------------------------
  initialize ADC Pins
 *----------------------------------------------------------------------------*/
void ADC_Init (void) 
{
  // enable power to ADC 
  LPC_SC->PCONP |= (1<<12); 
  
  LPC_PINCON->PINSEL3 &= ~(3UL<<30); // P1.31 is GPIO 
  LPC_PINCON->PINSEL3 |=  (3UL<<30); // P1.31 is AD0.5

  LPC_ADC->ADCR        =  (1<< 5) |  // select AD0.5 pin 
                          (4<< 8) |  // ADC clock is 25MHz/5 
                          (1<<21);   // enable ADC 

  // ADINTEN controls so that only individual channels will generate interrupts!
  LPC_ADC->ADINTEN  = (1 << 8); // global enable interrupt
  // enable ADC as source of interrupts in NVIC
  NVIC_EnableIRQ(ADC_IRQn); // enable ADC Interrupt 
}

/*----------------------------------------------------------------------------
  start ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StartCnv (void) 
{
  // (1)
  LPC_ADC->ADCR &= ~(7<<24); // stop conversion 
  // what the above does:   
  // 0000 0000 0000 0000 0000 0000 0000 0000 0111 : 7
  // 0000 0000 0111 0000 0000 0000 0000 0000 0000 : 7 << 24
  // 1111 1111 1000 1111 1111 1111 1111 1111 1111 : ~( 7 << 24)
  // bitwise AND-ed with contents of ADCR will reset to '0' bits 26:24
  // see page 586 of the User Manual that these bits are START;
  
  // (2)
  LPC_ADC->ADCR |=  (1<<24); // start conversion 
  // 1111 1111 1001 1111 1111 1111 1111 1111 1111 : ( 1 << 24)
  // bitwise OR-ed with with contents of ADCR will make bits 26:24 = "001"
  // which according to User Manual has an effect to "Start conversion now"
  // Note: when conversion finishes, DONE - bit 31 of ADGDR register, 
  // will be set to '1'; this will generate the Interrupt!
}


/*----------------------------------------------------------------------------
  stop ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StopCnv (void) 
{
  LPC_ADC->ADCR &= ~(7<<24); // stop conversion 
}

/*----------------------------------------------------------------------------
  get converted ADC value
 *----------------------------------------------------------------------------*/
uint32_t ADC_GetCnv (void) 
{
  while (!(LPC_ADC->ADGDR & (1UL << 31))); // Wait for Conversion process to end   
  
  AD_last = (LPC_ADC->ADGDR >> 4) & ADC_VALUE_MAX; // Store converted value   

  AD_done = 1;

  return (AD_last);
}

/*------------------------------------------------------------------------------
  read a converted value from the Analog/Digital converter
 *------------------------------------------------------------------------------*/
uint16_t ADC_Get (void) 
{
  uint16_t val;

  ADC_StartCnv();      // start A/D conversion
  val = ADC_GetCnv();  // use upper 8 bits of 12 bit AD conversion
  ADC_StopCnv();       // stop A/D conversion
  
  return (val);
}
