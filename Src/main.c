
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l476xx.h"
#include "core_cm4.h"
#include "stm32l4xx_ll_cortex.h"

// #if defined(USE_FULL_ASSERT)
// #include "stm32_assert.h"
// #endif /* USE_FULL_ASSERT */

#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF)

#include "gpio.h"

void     SystemClock_Config(void);
#define LED_PORT GPIOA
#define LED_PIN LL_GPIO_PIN_5
#define PWM_PORT GPIOC
#define PWM_PIN LL_GPIO_PIN_10


int tick = 0;
int expe = 1;
int blue_mode = 0;

void SystemClock_Config2(void){
	/* MSI configuration and activation */
	LL_RCC_MSI_EnableRangeSelection();
	LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_9);
	LL_RCC_MSI_Enable();
	while	(LL_RCC_MSI_IsReady() != 1)
		{ };

	/* Main PLL configuration and activation */
	/*LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_Enable();
	LL_RCC_PLL_EnableDomain_SYS();
	while(LL_RCC_PLL_IsReady() != 1)
		{ };*/

	/* Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_MSI)
		{ };

	/* Set APB1 & APB2 prescaler*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

	/* Update the global variable called SystemCoreClock */
	SystemCoreClockUpdate();
}

void SysTick_Handler(void){
	tick++;

	if(tick%2 == 1){
		LL_GPIO_SetOutputPin(PWM_PORT, PWM_PIN );
	}
	else{
		LL_GPIO_ResetOutputPin(PWM_PORT, PWM_PIN );
	}
	if(tick == expe*5) {
		LED_GREEN(0);
	}
	else if(tick > 100) {
		LED_GREEN(1);
		tick = 0;
	}
}

void MSICalibration_Process(void)
{
  /* LSE configuration and activation */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();

  if (LL_RCC_LSE_IsReady() == 0)
  {
      LL_RCC_ForceBackupDomainReset();
      LL_RCC_ReleaseBackupDomainReset();
      LL_RCC_LSE_Enable();
  }

  /* Enable MSI clock PLL */
  LL_RCC_MSI_EnablePLLMode();

  /* Calibration success*/
  //LED_On();
}

void Configure_RTC(void){
	  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	  LL_PWR_EnableBkUpAccess();

	  if (LL_RCC_LSE_IsReady() == 0)
	  {
	      LL_RCC_ForceBackupDomainReset();
	      LL_RCC_ReleaseBackupDomainReset();
	      LL_RCC_LSE_Enable();
	  }
	  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

	  LL_RCC_EnableRTC();

	  LL_RTC_DisableWriteProtection(RTC);
	  LL_RTC_EnableInitMode(RTC);
	  /*Truc à faire*/
	  LL_RTC_SetHourFormat(RTC, LL_RTC_HOURFORMAT_AMPM);
	  /* Set Asynch Prediv (value according to source clock) */
	  LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
	  /* Set Synch Prediv (value according to source clock) */
	  LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);
	  LL_RTC_DisableInitMode(RTC);
	  LL_RTC_EnableWriteProtection(RTC);
}



int main(void) {

int stateButton = 0;
int previousState = 0;

// Config GPIO
GPIO_init();

//Initialisation module RTC
Configure_RTC();


// On récupère la variable expe
// On l'incrémente si nécessaire
expe = LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0);
if (expe < 1) expe = 1;
if(BLUE_BUTTON()){
	expe++;
	if (expe > 8) expe = 1;
	LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, expe);
}


/* Configure the system clock */

switch(expe){
case 1:
	SystemClock_Config();
	break;
case 2:
	SystemClock_Config2();
	MSICalibration_Process();
	break;
case 3:
	break;
case 4:
	break;
case 5:
	break;
case 6:
	break;
case 7:
	break;
case 8:
	break;
}
SystemCoreClockUpdate();

// Configuration du SysTick
// A faire une fois que les clocks sont ocnfigurées
SysTick_Config(SystemCoreClock/100);

while (1) {
	if( BLUE_BUTTON() ) {
		//On regarde si le bouton est pressé
		previousState = stateButton;
		stateButton = 1;
 	}
	//On détecte un nouvel appui sur le bouton
	if(stateButton == 1 && previousState == 0){
		blue_mode = 1;
		/*
		if(expe == 2) {
				//Activation de la calibration du MSI
			LL_RCC_MSI_EnablePLLMode();
		}
		LL_LPM_EnableSleep();
		__WFI();*/
	}
 }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
/* MSI configuration and activation */
LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
LL_RCC_MSI_Enable();
while	(LL_RCC_MSI_IsReady() != 1)
	{ };
  
/* Main PLL configuration and activation */
LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
LL_RCC_PLL_Enable();
LL_RCC_PLL_EnableDomain_SYS();
while(LL_RCC_PLL_IsReady() != 1)
	{ };
  
/* Sysclk activation on the main PLL */
LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{ };
  
/* Set APB1 & APB2 prescaler*/
LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

/* Update the global variable called SystemCoreClock */
SystemCoreClockUpdate();
}
