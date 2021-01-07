/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: file content
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-06
 */
#include "adc.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"

ADC_HandleTypeDef hadc1;

// ADC1 init function
uint32_t BspAdc1Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    // Common config
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        return RET_ERR;
    }

    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return RET_ERR;
    }

    HAL_ADCEx_Calibration_Start(&hadc1);    // AD校准
    return RET_OK;
}

uint32_t BspAdc1GetValueOnce(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);   // 等待转换完成，50为最大等待时间，单位为ms，理论上1.5us完成一次
    
    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (adcHandle->Instance == ADC1) {
        /* ADC1 clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PB0     ------> ADC1_IN8
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
    if (adcHandle->Instance == ADC1) {
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration
        PB0     ------> ADC1_IN8
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
    }
}
