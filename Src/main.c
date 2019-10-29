
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l476xx.h"
#include "core_cm4.h"
#include "stm32l4xx_ll_cortex.h"

// #if defined(USE_FULL_ASSERT)
// #include "stm32_assert.h"
// #endif /* USE_FULL_ASSERT */

#include "gpio.h"

void     SystemClock_Config(void);
#define LED_PORT GPIOA
#define LED_PIN LL_GPIO_PIN_5
#define PWM_PORT GPIOC
#define PWM_PIN LL_GPIO_PIN_10


int tick = 0;
int expe = 2;

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
	//LL_GPIO_TogglePin(LED_PORT, LED_PIN);
	/*if(!stateLed){
		LED_GREEN(1);
		stateLed = 1;
	}
	else if(stateLed){
		LED_GREEN(0);
		stateLed = 0;
	}*/
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
	/*if(expe == 1){
		LL_LPM_EnableSleep();
	}*/
}

int main(void)
{
int stateButton = 0;
expe = 2;
/* Configure the system clock */
if (expe == 2){
	SystemClock_Config2();
}
else SystemClock_Config();
// config GPIO
GPIO_init();

// init timer pour utiliser la fonction LL_mDelay() de stm32l4xx_ll_utils.c
//LL_Init1msTick( SystemCoreClock );
//SysTick->CTRL |= 1;
//SysTick->CTRL |= 2;
SysTick_Config(SystemCoreClock/100);

//Activation du LSE pour future calibration
if(expe == 2) {
	LL_RCC_LSE_Enable ();
}



while (1) {
	if	( BLUE_BUTTON() ) {
		stateButton = 1;
 	}
	if(stateButton == 1){

		//Activation de la calibration du MSI
		if(expe == 2) {
			LL_RCC_MSI_EnablePLLMode();
		}


		LL_LPM_EnableSleep();
		__WFI();
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
