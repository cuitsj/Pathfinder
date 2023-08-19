#include "TASK_MOTOR.hpp"

void TASK_MOTOR::MOTOR_FUN(void* parameter)
{
	bool dec_flag = 0;
	bool inc_flag = 0;
	uint8_t rxdata = 0;
	uint16_t len = 0;
	int16_t set_speed = 30;
	int16_t Lset_speed = 0;	//Ŀ���ٶ�
	int16_t Rset_speed = 0;	//Ŀ���ٶ�
	int16_t Lnow_speed = 0;		//��ǰ�ٶ�
	int16_t Rnow_speed = 0;
	int16_t Lnow_err = 0;			//��ǰ���
	int16_t Rnow_err = 0;			//��ǰ���
	int16_t Llast1_err = 0;		//�ϴ����
	int16_t Rlast1_err = 0;		//�ϴ����
	int16_t Llast2_err = 0;		//���ϴ����
	int16_t Rlast2_err = 0;		//���ϴ����
	int16_t Left_duty = 0;		//���ռ�ձ�
	int16_t Right_duty = 0;
	
	for(;;)
	{
		/*****************************************�ֻ�ң��*****************************************/
		if (gBSP->UART1_FPGA->GetRxLenth() >=  1)
		{
			gBSP->UART1_FPGA->Read(&rxdata);
		}
		if (gBSP->UART2_BT->GetRxLenth() >=  1)
		{
			gBSP->UART2_BT->Read(&rxdata);
		}
		switch (rxdata)
		{
			//ǰ��
			case 0x11:
				dec_flag=0;
				inc_flag=0;
				Lset_speed = set_speed;
				Rset_speed = set_speed;
				break;
			//����
			case 0x22:
				dec_flag=0;
				inc_flag=0;
				Lset_speed = -set_speed;
				Rset_speed = -set_speed;
				break;
			//��ת
			case 0x33:
				dec_flag=0;
				inc_flag=0;
				Lset_speed = -set_speed;
				Rset_speed = set_speed;
				break;
			//��ת
			case 0x44:
				dec_flag=0;
				inc_flag=0;
				Lset_speed = set_speed;
				Rset_speed = -set_speed;
				break;
			//����
			case 0x55:
				if(inc_flag == 0)
				{
					inc_flag=1;
					set_speed=set_speed+10;
					if(set_speed>80)set_speed=80;
				}
				break;
			//����
			case 0x66:
				if (dec_flag == 0)
				{
					dec_flag=1;
					set_speed=set_speed-10;
					if(set_speed<0)set_speed=0;
				}
				break;
			//ֹͣ
			case 0x00:
				dec_flag=0;
				inc_flag=0;
				Lset_speed = 0;
				Rset_speed = 0;
				break;
			//ֹͣ
			default:
				Lset_speed = 0;
				Rset_speed = 0;
				break;
		}

		/*****************************************�ֻ�ң��*****************************************/
		/*****************************************����ʽPID�ٶȿ���*****************************************/
		//1��ȡ��ǰ�ٶ�
		Lnow_speed = gBSP->TIM2_LENCODER->ReadCnt();
		Rnow_speed = gBSP->TIM3_RENCODER->ReadCnt();

		//2���㵱ǰ���
		Lnow_err = (Lset_speed - Lnow_speed);
		Rnow_err = (Rset_speed - Rnow_speed);
		
		//3���������Ŷ�
		if(Lnow_err<6&&Lnow_err>-6)
		{
			Lnow_err=0;
		}
		if(Rnow_err<6&&Rnow_err>-6)
		{
			Rnow_err=0;
		}
		
		//4����ʽPID
		Left_duty +=  0.1*(Lnow_err-Llast1_err) + 0.8*Lnow_err + 0*(Lnow_err - 2*Llast1_err + Llast2_err);
		Right_duty +=  0.1*(Rnow_err-Rlast1_err) + 0.8*Rnow_err + 0*(Rnow_err - 2*Rlast1_err + Rlast2_err);
		
		//5�޷�
		if (Left_duty > 900)
		{
			Left_duty = 900;
		}
		if (Left_duty < -900) 
		{
			Left_duty = -900; 
		}
		if (Right_duty > 900)
		{
			Right_duty = 900;
		}
		if (Right_duty < -900) 
		{
			Right_duty = -900; 
		}
		
		//4��¼��һ�β�
		Llast2_err = Llast1_err;
		Llast1_err = Lnow_err;
		
    //5���
		gBSP->TIM0_MOTORL->SetDUTY(Left_duty);
		gBSP->TIM0_MOTORR->SetDUTY(Right_duty);
		/*****************************************����ʽPID�ٶȿ���*****************************************/
		
		/*****************************************DataScope*****************************************/

	    		    				 
//		DataScope_Get_Channel_Data( Lset_speed , 1 );  //������ 1.0  д��ͨ�� 1
//		DataScope_Get_Channel_Data( Lnow_speed , 2 );  //������ 2.0  д��ͨ�� 2
//		DataScope_Get_Channel_Data( Lnow_err , 3 );  //������ 3.0  д��ͨ�� 3
//		DataScope_Get_Channel_Data( Llast2_err , 4 );  //������ 4.0  д��ͨ�� 4
//		DataScope_Get_Channel_Data( Left_duty , 5 );  //������ 5.0  д��ͨ�� 5
//    
//		gBSP->UART2_BT->Write(DataScope_OutPut_Buffer,DataScope_Data_Generate(5));
		
		/*****************************************DataScope*****************************************/
		gOS->delay_ms(10);
	}
}

TASK_MOTOR::TASK_MOTOR()
{
	func = (mainFun_t)MOTOR_FUN;
	stackSize = 1024;
	name = "TASK_MOTOR";
	thread_prioroty = 1;
}