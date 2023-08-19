#pragma once

#include "main.h"

class TASK_HCI: public TASK
{
public:
	TASK_HCI();
	static void HCI_FUN(void* parameter);
};

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  					   
#define OLED_SCLK_Clr() gBSP->OLED_SCL->Reset()//CLK
#define OLED_SCLK_Set() gBSP->OLED_SCL->Set()
#define OLED_SDIN_Clr() gBSP->OLED_SDA->Reset()//DIN
#define OLED_SDIN_Set() gBSP->OLED_SDA->Set()     
#define OLED_CMD  0	//ะดรüม๎
#define OLED_DATA 1	//ะดสýพÝ
#define IIC_SLAVE_ADDR 0x78  //IIC slave device address

