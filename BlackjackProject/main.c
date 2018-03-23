#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPC17xx.H"
#include "GLCD.h"
#include "ADC.h"

#define PINSEL_EINT0    20
#define PINSEL_EINT1    22
#define PINSEL_EINT2    24

#define LED1            0   
#define LED2            1
#define LED3            2

#define SBIT_EINT0      0
#define SBIT_EINT1      1
#define SBIT_EINT2      2

#define SBIT_EXTMODE0   0
#define SBIT_EXTMODE1   1

#define SBIT_EXTPOLAR0  0
#define SBIT_EXTPOLAR1  1

int eint1flag = 0, eint2flag = 0;
char text[10];

extern uint16_t AD_last;  // Last converted value    
extern uint8_t  AD_done;  // AD conversion done flag  
extern uint8_t  clock_1s; // Flag activated each second

void EINT1_IRQHandler(void)
{
    LPC_SC->EXTINT = (1<<SBIT_EINT1);  /* Clear Interrupt Flag */
		eint1flag = 1;
	  
}

void EINT2_IRQHandler(void)
{
    LPC_SC->EXTINT = (1<<SBIT_EINT2);  /* Clear Interrupt Flag */
    eint2flag = 1;
}


int main(void) {
	
	uint32_t ad_avg = 0;
  uint16_t ad_val = 0, ad_val_previous = 0xFFFF;
	int betAmount = 0;
	char str[50];
	int coinTotal = 100;
	char buffer[10];
	char buffer2[10];
	int i = 0;
	
	//Initializations
	SystemInit();
	
	SysTick_Config(SystemCoreClock/100);
	
	//Initialize ADC
	ADC_Init();
  
	//LCD display initialization
	LCD_Initialization(); // Initialize graphical LCD 
	LCD_Clear(Black); // Clear graphical LCD display

	LCD_PutText(100, 160, "Blackjack", White, Black);
	LCD_PutText(30, 180, "Press Key1 to Start", White, Black);
	
	//LPC_SC->EXTINT      = (1<<SBIT_EINT0);
  //LPC_PINCON->PINSEL4 |= (1<<PINSEL_EINT0);
  //LPC_SC->EXTMODE     |= (1<<SBIT_EXTMODE0);
  //LPC_SC->EXTPOLAR    |= (1<<SBIT_EXTPOLAR0);

	LPC_SC->EXTINT      = (1<<SBIT_EINT1);
  LPC_PINCON->PINSEL4 |= (1<<PINSEL_EINT1);   
  LPC_SC->EXTMODE     |= (1<<SBIT_EXTMODE1);  
  LPC_SC->EXTPOLAR    |= (1<<SBIT_EXTPOLAR0);
	
	LPC_SC->EXTINT      = (1<<SBIT_EINT2);
  LPC_PINCON->PINSEL4 |= (1<<PINSEL_EINT2);   
  LPC_SC->EXTMODE     |= (1<<SBIT_EXTMODE1);  
  LPC_SC->EXTPOLAR    |= (1<<SBIT_EXTPOLAR0);
	
  //NVIC_EnableIRQ(EINT0_IRQn);    /* Enable the EINT0,EINT1 interrupts */
  NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);

  while(eint1flag == 0)
  {
      // Do nothing
	}
	for(i = 0; i < 7500000; i++);
	
	eint1flag = 0;
	
	LCD_Clear(Black); // Clear graphical LCD display
	
	strcpy(str, "COINS: ");
	sprintf(buffer, "%d", coinTotal);
  strcat(str, buffer);
	LCD_PutText(5, 5, str, White, Black);
	
	//LCD_PutText(5, 110, "How many coins would you like to bet?", White, Black);
	//LCD_PutText(5, 160, "Use the analog input to change bet", White, Black);
	//LCD_PutText(5, 210, "Press Key1 to confirm bet", White, Black);
	
	while(eint1flag == 0)
  {
		// (a) get AD convertion result and compute average of the last 16 values; 
    if (AD_done) { // If conversion has finished 
      AD_done = 0; // reset to prepare for a new conversion

      ad_avg += AD_last << 8; // Add AD value to averaging 
      ad_avg ++;
      if ((ad_avg & 0xFF) == 0x10) { // average over 16 values  
        ad_val = (ad_avg >> 8) >> 4; // average devided by 16
        ad_avg = 0;
      }
    }
		
    // (b) display on LCD;
    if (ad_val ^ ad_val_previous) { // AD value changed? 
      ad_val_previous = ad_val;
			
			if(ad_val <= 0xCD) {
				betAmount = 5;
			}
			else if(ad_val > 0xCD && ad_val <= 0x19A) {
				betAmount = 10;
			}
			else if(ad_val > 0x19A && ad_val <= 0x267) {
				betAmount = 15;
			}
			else if(ad_val > 0x267 && ad_val <= 0x334) {
				betAmount = 20;
			}
			else {
				betAmount = 50;
			}

			sprintf(buffer2, "%d", betAmount);
			LCD_PutText(5,300, buffer2, White, Black);
			
      sprintf(text, "0x%04X ", ad_val); // format text for print out
      LCD_PutText(32,192, (unsigned char *)text, White, Black);          
      LCD_DrawBargraph (32,208, 176,20, (ad_val >> 2), Red, Black); // max bargraph is 10 bit; 
    }
	}
	for(i = 0; i < 7500000; i++);
	eint1flag = 0;
	sprintf(buffer2, "%d", betAmount);
	//LCD_PutText(5,300, buffer2, White, Black); 
}
