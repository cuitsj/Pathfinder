#include "ADC_F450.hpp"
#include "main.h"
 
void ADC0_F450::Init()
{
	/* system clocks configuration */
 /* enable GPIOC clock */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);
	/* enable ADC clock */
	rcu_periph_clock_enable(RCU_ADC0);
	/* enable DMA clock */
	rcu_periph_clock_enable(RCU_DMA1);
	/* config ADC clock */
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
	/* GPIO configuration */
	/* config the GPIO as analog mode */
	gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	/* DMA configuration */
	/* ADC_DMA_channel configuration */
	dma_single_data_parameter_struct dma_single_data_parameter;
	
	/* ADC DMA_channel configuration */
	dma_deinit(DMA1, DMA_CH0);
	
	/* initialize DMA single data mode */
	dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
	dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_single_data_parameter.memory0_addr = (uint32_t)adcValue;
	dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
	dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
	dma_single_data_parameter.number = 8;
	dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
	dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
	dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

	/* enable DMA circulation mode */
	dma_circulation_enable(DMA1, DMA_CH0);

	/* enable DMA channel */
	dma_channel_enable(DMA1, DMA_CH0);
		
	/* ADC configuration */
	/* ADC mode config */
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
	adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	
	/* ADC channel length config */
	adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 8);
	/* ADC regular channel config */
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_4, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_5, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_6, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_7, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_10, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_11, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_12, ADC_SAMPLETIME_15);
	adc_regular_channel_config(ADC0, 7, ADC_CHANNEL_13, ADC_SAMPLETIME_15);
	/* ADC trigger config */
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T0_CH0); 
	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
	
	/* ADC DMA function enable */
	adc_dma_request_after_last_enable(ADC0);
	adc_dma_mode_enable(ADC0);
	
	/* enable ADC interface */
	adc_enable(ADC0);
	/* wait for ADC stability */
	rt_thread_mdelay(1);
	/* ADC calibration and reset calibration */
	adc_calibration_enable(ADC0);
	
	/* enable ADC software trigger */
	adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

//获取实际值
void ADC0_F450::PeriodRun(void)
{
	VOLIN4_1V2 = adcValue[0]/4096.0*36.3;
	VOLIN5_2V5 = adcValue[1]/4096.0*36.3;
	VOLIN6_3V3 = adcValue[2]/4096.0*36.3;
	VOLIN7_GD3V3 = adcValue[3]/4096.0*36.3;
	VOLIN10_BAT2 = adcValue[4]/4096.0*36.3;
	VOLIN11_BAT = adcValue[5]/4096.0*36.3;
	VOLIN12_BAT1 = adcValue[6]/4096.0*36.3;
	VOLIN13_5V = adcValue[7]/4096.0*36.3;
}
