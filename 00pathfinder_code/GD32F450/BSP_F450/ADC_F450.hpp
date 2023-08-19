#pragma once
 
#include <stdint.h>

class ADC
{
    public:
		ADC(){}
		
    uint32_t adcValue[8];
		float VOLIN4_1V2;
		float VOLIN5_2V5;
		float VOLIN6_3V3;
		float VOLIN7_GD3V3;
		float VOLIN10_BAT2;
		float VOLIN11_BAT;
		float VOLIN12_BAT1;
		float VOLIN13_5V;
    virtual void Init(void){}
    virtual void PeriodRun(){}
};

class ADC0_F450:public ADC
{
    public:
    ADC0_F450(){}
	 void Init(void);
	 void PeriodRun();
};