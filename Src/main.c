
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

#include "gpio.h"
#include "rtc.h"
#include "clock.h"

void     SystemClock_Config(void);
#define LED_PORT GPIOA
#define LED_PIN LL_GPIO_PIN_5
#define PWM_PORT GPIOC
#define PWM_PIN LL_GPIO_PIN_10

int tick = 0;
int expe = 1;
int blue_mode = 0;
int stateButton = 0;
int previousState = 0;

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
	if( BLUE_BUTTON() ) {
		//On regarde si le bouton est pressé
		previousState = stateButton;
		stateButton = 1;
 	}
	//On détecte un nouvel appui sur le bouton
	if(stateButton == 1 && previousState == 0){
		blue_mode = 1;

		if(expe == 1 || expe == 3) {
			LL_LPM_EnableSleep();
			__WFI();
		} else if (expe == 2 || expe == 4) {
			MSICalibration_Process(); //pas testé
		}
		else if(expe == 5){
			LL_PWR_SetPowerMode(LL_PWR_MODE_STOP0);
			LL_LPM_EnableDeepSleep();
			 __WFI();
		}
		else if(expe == 6){
			LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
			LL_LPM_EnableDeepSleep();
			__WFI();
		}
		else if(expe == 7){
			LL_PWR_SetPowerMode(LL_PWR_MODE_STOP2);
			LL_LPM_EnableDeepSleep();
			__WFI();
		}
		else if(expe == 8){
			LL_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);
			LL_LPM_EnableDeepSleep();
		}
	}
}




int main(void) {

// Config GPIO
GPIO_init();

//Initialisation module RTC
Configure_RTC();

// On récupère la variable expe
// On l'incrémente si nécessaire
/*
expe = LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0);
if (expe < 1) expe = 1;
if(BLUE_BUTTON()){
	expe++;
	if (expe > 8) expe = 1;
	LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, expe);
} */
expe = 5;

/* Configure the system clock */
if(expe == 1) {
	SystemClock_Config();
}
else if(expe == 2) {
	SystemClock_Config2();
} else {
	SystemClock_Config3();
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
}

if (expe >= 5) {
	LL_LPM_EnableSleep();
	__WFI();
}

if(expe >= 5 && expe <= 7) {
	RTC_wakeup_init_from_stop(5);
} else if (expe == 8) {
	RTC_wakeup_init_from_standby_or_shutdown(5);
}

while (1) {
}

}


