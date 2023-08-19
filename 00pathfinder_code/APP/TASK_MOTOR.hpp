#pragma once

#include "main.h"

class TASK_MOTOR: public TASK
{
public:
	TASK_MOTOR();
	static void MOTOR_FUN(void* parameter);
};
