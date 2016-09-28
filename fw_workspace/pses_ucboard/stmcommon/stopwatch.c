
#include "stm32f3xx.h"
//#include "stm32f3xx_hal_conf.h"

#include "stopwatch.h"
#include "stm32f3xx_hal_tim.h"
//#include <stdint.h>

extern void Error_Handler(void);

static TIM_HandleTypeDef htim15;
static TIM_HandleTypeDef htim20;

void stopwatch_init( void )
{
	__HAL_RCC_TIM15_CLK_ENABLE();
	__HAL_RCC_TIM20_CLK_ENABLE();

	//TIM_MasterConfigTypeDef sMasterConfig;
	//TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	//TIM_OC_InitTypeDef sConfigOC;

	// Timer läuft mit 1MHz (72 MHz Prozessor, 72er Prescaler)
	htim15.Instance = TIM15;
	htim15.Init.Prescaler = 71;
	htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim15.Init.Period = 0xFFFF;
	htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim15.Init.RepetitionCounter = 0;
	if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
	{
		Error_Handler();
	}

	htim20.Instance = TIM20;
	htim20.Init.Prescaler = 0;
	htim20.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim20.Init.Period = 0xFFFF;
	htim20.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim20.Init.RepetitionCounter = 0;
	if (HAL_TIM_Base_Init(&htim20) != HAL_OK)
	{
		Error_Handler();
	}

	//TIM15->DIER = 0;
	__HAL_TIM_CLEAR_FLAG( &htim15, TIM_FLAG_UPDATE );
	__HAL_TIM_CLEAR_FLAG( &htim20, TIM_FLAG_UPDATE );



	/*

	TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );
	*/

	// Timer 20 ist Slave von Timer 15
	// CR2[6:4] = 010 -> Update-Mode (Timer 15 ist Prescaler für Timer 20)
	// Bit 4-6 löschen
	TIM15->CR2 &= ~(uint16_t)(0x7 << 4);
	// Folge 010 in Bits 4-6 schreiben
	TIM15->CR2 |= (uint16_t)(0x2 << 4);


	// Timer 20 ist Slave
	// SMCR[6:4] = 011 -> Master ist TIM20
	// SMCR[2:0] = 111 -> External Clock Mode

	// Bit 4-6 löschen
	TIM20->SMCR &= ~(uint16_t)(0x7 << 4);
	// Folge 010 in Bits 4-6 schreiben
	TIM20->SMCR |= (uint16_t)(0x3 << 4);

	// Folge 111 in Bits 0-2 schreiben
	TIM20->SMCR |= (uint16_t)0x7;

	TIM15->CNT = 0;
	TIM20->CNT = 0;

	__HAL_TIM_ENABLE_IT(&htim15, TIM_IT_UPDATE);

	HAL_TIM_Base_Start(&htim20);
	HAL_TIM_Base_Start(&htim15);

	return;
}


void TIM15_IRQHandler( void )
{
	__HAL_TIM_CLEAR_FLAG( &htim15, TIM_FLAG_UPDATE );

	return;
}

