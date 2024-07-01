#include "fuse.h"
#include "math.h"

Body Flag;                  // �ṹ�嶨�����ؼ���־λ
int16 Turn_PWM = 0;  				// ����ת��PWM
float vtest = 0;
//float vtest_t = 0;
float maxPWM = 0;
float minPWM = 0;
int SideRate = 900, CornerRate = 900, XieRate = 100;
/*******************PIT��ʱ�ж�******************
������void Fuse_result(void)
���ܣ��ٶȻ���ת�򻷿���
��������
˵����
����ֵ����
 **********************************************/
unsigned char int_OK = 0;								// ��ʼ���ɹ���־λ
unsigned char Flag_OpenLoop = 0;				// Ĭ�Ͻ��бջ�����
void Fuse_result(void)
{
    if(int_OK)
    {
        if(!Flag.start_go){
            go_motor(0, 0);	// ���籣��
			Flag.T_Inmost = 0;
			Flag.T_Turn = 0;
			Flag.T_Distance = 0;
			Flag.T_IMU = 0;
			Flag.T_Speed = 0;
			Electromagnetism_Control();                  // ��вɼ����� ð�����򻬶��˲�
		}
        else
        {
            // 5ms�������ڻ�:����������ǰ��
            if(1 == Flag.T_Inmost)
            {
                Flag.T_Inmost = 0;
                if(!Flag_Slope)														// ֻҪ�����µ�
				{
                    Speed_PWM = OpenLoop_Speed;						// ����Ҫ����ٶȱջ���ֱ�ӽ�����ֵ����ռ�ձ�
				}
				Speed_PWM = PID_Realize(&SpeedPID, Speed_Pid, real_speed, ClsLoop_Speed - abs(Turn_PWM) * 10);		// �ٶ�λ��ʽPID
				Speed_PWM = range_protect(Speed_PWM, 500, ClsLoop_Speed - abs(Turn_PWM) * 10 + 500);													// ע��������
                // �������ٶȻ�������Ͷ��ռ�ձ�
                All_PWM_left = Speed_PWM - (Turn_PWM) * (vtest);
                All_PWM_right = Speed_PWM + (Turn_PWM) * (vtest);
				//maxPWM = abs(All_PWM_left) > abs(All_PWM_right) ? abs(All_PWM_left) : abs(All_PWM_right);
				//maxPWM = maxPWM > 5000 ? 5000 / maxPWM : 1;
				All_PWM_left = range_protect(All_PWM_left, -3200, 7000);
				All_PWM_right = range_protect(All_PWM_right, -3200, 7000);
				go_motor(All_PWM_left/*  * maxPWM*/, All_PWM_right/* * maxPWM*/);
            }

            // 10ms���ƣ����ת��
            if(1 == Flag.T_Turn && 0 == Flag_OpenLoop)
            {
                Flag.T_Turn = 0;
                Electromagnetism_Control();						// ��Ųɼ�����
                adc_deviation = 0
					+ Cha_BI_He_Sqrt(Left_Adc, Right_Adc, SideRate) 
					+ Cha_BI_He_Sqrt(Left_Corner_Adc, Right_Corner_Adc, CornerRate) 
					+ Cha_BI_He_Sqrt(Left_Xie_Adc, Right_Xie_Adc, XieRate); //   9�� 1
                Turn_PWM = PlacePID_Control(&TurnPID, Turn_Pid[Turn_Suquence], adc_deviation, 0); //ת��̬PID
                Turn_PWM = -Turn_PWM;
				Turn_PWM = range_protect(Turn_PWM, -750, 750);
				Annulus_Analysis();								// Բ��ʶ����
                //Steering_Control_Out(Turn_PWM);				// �����������������ڲ����޷���
            }

            // 30ms���ƣ�TOF����
            if(1 == Flag.T_Distance)
            {
                Flag.T_Distance = 0;

                if(!Flag_Tof_Finish)						// ��δ��ɹ�һ�μ��
                    Tof_Control();								// �ϰ��������
            }

            // 40ms���ƣ��µ����(IMU660)
            if(1 == Flag.T_IMU)
            {
                Flag.T_IMU = 0;
//								if(!Flag_Slope_Finish)
//									IMU_Control();
            }

            // 50ms���ƣ��ٶȱջ�����
            if(1 == Flag.T_Speed)
            {
                Flag.T_Speed = 0;
                //Speed_PWM = PID_Realize(&SpeedPID, Speed_Pid, real_speed, ClsLoop_Speed);		// �ٶ�λ��ʽPID
                //Speed_PWM = range_protect(Speed_PWM, -0, 4000);													// ע��������
            }
        }
    }
}

//���Ĺؼ��ı�־λ�ṹ���ʼ��
void Flag_Init(void)
{
    Flag.start_go = 0;
    Flag.T_Inmost = 0;
    Flag.T_Turn = 0;
    Flag.T_Distance = 0;
    Flag.T_Speed = 0;
    Flag.T_IMU = 0;
    Flag.STOP = 0;								// ͣ������
    Flag.OUT_Garage = 0;				  // �����־λ
    Flag.Game = 0;
}
