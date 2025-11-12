#include "MKL46Z4.h"                    // NXP::Device:Startup

void Initsw1(void)
	{
		SIM->SCGC5 |= (1 << 11); 
		PORTC->PCR[3] = (1u << 8)|(1u<<1)|(1u << 0); 
		PTC->PDDR &= ~((uint32_t)(1u << 3)); 
  }
	
void Initsw3(void)
	{
		SIM->SCGC5 |= (1 << 11); 
		PORTC->PCR[12] = (1u << 8)|(1u<<1)|(1u << 0); 
		PTC->PDDR &= ~((uint32_t)(1u << 12)); 
  }

void InitLED(void)
  {
		SIM->SCGC5 |= (1 << 12);
		PORTD->PCR[5] = (1 << 8);
		PTD->PDDR |= (1 << 5);
		PTD->PSOR = (1<<5);
  }

void InitLED2(void)
  {
		SIM->SCGC5 |= (1 << 13);
		PORTE->PCR[29] = (1 << 8);
		PTE->PDDR |= (1 << 29);
		PTE->PSOR = (1<<29);
  }
	

	

