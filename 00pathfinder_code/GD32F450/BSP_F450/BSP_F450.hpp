#pragma once

#include "IO_F450.hpp"
#include "UART_F450.hpp"
#include "SPI_F450.hpp"
#include "IIC_F450.hpp"
#include "ADC_F450.hpp"
#include "TIM_F450.hpp"

class BSP_F450
{
	public:
		bool flag_debug;
		BSP_F450();
		IO *KEY_MODE;
		IO *OLED_SCL;
		IO *OLED_SDA;
		IO *MPU_SCL;
		IO *MPU_SDA;
		IO *LED_R;
		IO *LED_G;
		IO *LED_B;
		UART *UART0_DEBUG;
		UART *UART2_BT;
		UART *UART1_FPGA;
		ADC *ADC0_VOL;
		TIM *TIM0_MOTORL;
		TIM *TIM0_MOTORR;
		TIM *TIM2_LENCODER;
		TIM *TIM3_RENCODER;
		TIM *TIM8_BEEP;
		void Init(void);
};
