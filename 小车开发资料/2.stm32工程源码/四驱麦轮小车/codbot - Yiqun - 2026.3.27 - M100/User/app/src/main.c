/*
*********************************************************************************************************
*
*	模块名称 : 主程序
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/
#include "includes.h"
/*
**********************************************************************************************************
											宏定义
**********************************************************************************************************
*/

/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate(void);
static void AppTaskControl(void *argument);
static void AppTaskMsgBack(void *argument);
static void AppTaskStart(void *argument);
static void AppTimerHanle(void *argument);

uint8_t ble_control(void);
void ps2_control();
void serial_control();
void ccd_control();
void vMotorTest();
void vServoTest(void);
void Key_Mode(void);
/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/

/*
通过变量MODE来选择模式，默认蓝牙遥控模式
0-蓝牙遥控模式 1-PS2遥控模式 2-5路灰度巡线模式 3-4路红外巡线模式 4-超声波跟随模式 5-CCD巡线模式 6-串口控制模式
*/

uint8_t MODE= 0;


extern int16_t g_WheelSpeed[4];//转速
uint16_t _usTestTime = 0;
uint16_t _usStopTime = 0;
uint16_t _servo_time_p = 600;
uint8 _servo_time_f = 0;
uint8 _Ctrl_Flag = 0;
u8g2_t u8g2;
int16_t _sCCD_piancha = 0;
int16_t _sTurn_pidout = 0;
float _fDistance = 999;

static  uint8_t     s_Ps2_Get_Cnt = 0;
static	uint8_t 		_PS2blePivotSteerDir = 0;
static	uint8_t			_PS2blePivotSteerSpeed = 0;
static	uint8_t			_PS2bleMoveDir = 0;
static	uint16_t 	  _PS2bleMoveSpeed = 0;
static	uint8_t			_PS2bleMoveDir_Y=0;
static	uint16_t 	  _PS2bleMoveSpeed_Y=0;


int16_t Key_MODE = 0;
int16_t Flag = 0;

uint8_t _disLinkCnt = 0;
uint8_t ucKeyCode ;
LinkStatus_t _linkStatus;

Ps2Key_t _Ps2key = {0};
Ps2Pad_t _left  = {0};
Ps2Pad_t _right = {0};

/*定时任务ID*/
osTimerId_t g_TimerHand_Id;

/* 任务的属性设置 */
const osThreadAttr_t ThreadStart_Attr =
{

    .name = "osRtxStartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal3,
    .stack_size = 4096,
};


const osThreadAttr_t ThreadControl_Attr =
{
    .name = "osRtxThreadControl",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal2,
    .stack_size = 4096,
};

const osThreadAttr_t ThreadMsgBack_Attr =
{
    .name = "osRtxThreadMsgBack",
    .attr_bits = osThreadDetached,
    .priority = osPriorityNormal1,
    .stack_size = 1024,
};




/* 任务句柄 */
osThreadId_t ThreadIdTaskControl = NULL;
osThreadId_t ThreadIdTaskMsgBack = NULL;
osThreadId_t ThreadIdStart       = NULL;


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 程序主入口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

int main(void)
{

    System_Init();

    osKernelInitialize();

    ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);

    osKernelStart();

    while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 消息队列创建
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void AppObjCreate(void)
{


}


/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgBack
*	功能说明: 数据反馈
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskMsgBack(void *argument)
{
    char strpiancha[32];
    while(1)
    {

        u8g2_FirstPage(&u8g2);
        do
        {
						Key_Mode();
					
					
					char *name;
					switch (MODE) {
							case 0: name = "BT Remote"; break;
							case 1: name = "PS2 Remote";   break;
							case 2: name = "Gray Track";break;
							case 3: name = "IR Track";  break;
							case 4: name = "Ultra Follow";  break;
							case 5: name = "CCD Track"; break;
							case 6: name = "Serial Ctrl";break;
							default: name = "Unknown";   break;
							
					}


            u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tf);
            //sprintf(strpiancha, "Mode: %d", MODE);
					  sprintf(strpiancha, "Mode:%d %s", MODE, name);
            u8g2_DrawStr(&u8g2, 0, 30, strpiancha);
            sprintf(strpiancha, "_fDistance: %f", _fDistance);
            u8g2_DrawStr(&u8g2, 0, 40, strpiancha);
//            sprintf(strpiancha, "_sCCD_piancha: %d", _sCCD_piancha);
//            u8g2_DrawStr(&u8g2, 0, 50, strpiancha);
//            sprintf(strpiancha, "pidout: %d", _sTurn_pidout);
//            u8g2_DrawStr(&u8g2, 0, 60, strpiancha);
        }
        while (u8g2_NextPage(&u8g2));
				
				// 串口控制模式下，反馈信息
				if(MODE==6){
					MotorSpeedInfoUpdate();
					CarSpeedInfoUpdate();
					RobotModelInfoUpdate();
				}

        osDelay(5);
    }

}

/*
*********************************************************************************************************
*	函 数 名: AppTaskControl
*	功能说明: 运动控制
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/

static void AppTaskControl(void *argument)
{


    while(1)
    {

        _fDistance = getSR04_fDistance();//获取距离
        HCSR_04_Start(); //启动超声波
				if(CCD_ENABLE){
					Read_CCD_Val(); //读CCd
					Find_CCD_MidVal();
				}
        switch (MODE)
        {
				case 0: //蓝牙控制模式
            _Ctrl_Flag = 3;
            ble_control();
            break;					
        case 1: //ps2控制模式
           _Ctrl_Flag = 4;
            ps2_control();
            break;
        case 2: //灰度巡线跟随
            _Ctrl_Flag = 6;
            break;		
        case 3: //红外巡线
						_Ctrl_Flag = 1;
            break;
				case 4: //超声波跟随模式
            _Ctrl_Flag = 0;
            Ultrasonic_Follow(_fDistance, 25);
            break;
        case 5: //ccd巡线模式
            _Ctrl_Flag = 0;
            ccd_control();
            break;
        
        case 6: //串口控制模式
            _Ctrl_Flag = 5;
            serial_control();
            break;
        default:
            break;
        }

        //vMotorTest();
        //vServoTest();
        osDelay(50);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，这里用作BSP驱动包处理。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4
*********************************************************************************************************
*/


void AppTaskStart(void *argument)
{
    const uint16_t usFrequency = 1000; /* 延迟周期100 */

    uint32_t tick;
    bsp_Init();
    u8g2Init(&u8g2);


    /*jlink打印*/
    /* 配置通道0，上行配置*/
    SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    /* 配置通道0，下行配置*/
    SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    /* 创建任务 */
    AppTaskCreate();
    AppObjCreate();



    /* 获取当前时间 */
    tick = osKernelGetTickCount();

    while(1)
    {


        /* 相对延迟 */
        tick += usFrequency;
        osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTimerHanle
*	功能说明: 软件定时器的回调函数，周期性执行喂狗及发送状态。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4
*********************************************************************************************************
*/

void AppTimerHanle(void *argument)
{

    _usTestTime++;
    if(_level.l1 && _level.l2 && _level.r1 && _level.r2)
    {
        _usStopTime++;
    }


    if(_servo_time_p == 2400)
    {
        _servo_time_f = 1;
    }

    if(_servo_time_p == 600)
    {
        _servo_time_f = 0;
    }

    if(_servo_time_f == 0)
    {
        _servo_time_p += 50;
    }
    else _servo_time_p -= 50;


}



/*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 任务创建。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4
*********************************************************************************************************
*/
static void AppTaskCreate(void)
{


    ThreadIdTaskMsgBack = osThreadNew(AppTaskMsgBack, NULL, &ThreadMsgBack_Attr);
    ThreadIdTaskControl = osThreadNew(AppTaskControl, NULL, &ThreadControl_Attr);
    g_TimerHand_Id      = osTimerNew(AppTimerHanle, osTimerPeriodic, NULL, NULL);

    osTimerStart(g_TimerHand_Id, 100);

}



/***********************************************************
*@fuction	:ble_control
*@brief		:蓝牙控制函数
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-23
***********************************************************/

uint8_t ble_control(void)
{
    static uint8_t  _byte = 0;
    static uint16_t _bleMoveSpeed = 0;
    static uint8_t  _bleMoveDir = 0;
    static uint8_t _bleMoveDir_Y=0;
    static uint8_t  _blePivotSteerDir = 0;
    static uint16_t  _blePivotSteerSpeed = 0;
    static uint16_t  _blePivotMoveSpeed = 350;
	    static uint16_t  _bleMoveSpeed_Y = 0;

    if(comGetChar(COM5, &_byte) != 0)
    {


        switch(_byte)
        {
        case'0':/*停车*/
        case'5':/*停车*/
            _bleMoveSpeed = 0;
            _bleMoveDir = 0;
            _blePivotSteerDir = 0;
            _blePivotSteerSpeed = 0;
            _bleMoveSpeed_Y=0;
						_bleMoveDir_Y=0;
            break;
        case'3':/*左原地转弯*/
            _blePivotSteerDir = 0;
            _blePivotSteerSpeed = 200;


            break;
        case'4':/*右原地转弯*/
            _blePivotSteerDir = 1;
            _blePivotSteerSpeed = 200;

            break;
        case'6':/*前进*/
            _bleMoveSpeed = _blePivotMoveSpeed;
            _bleMoveDir = 0;


            break;
        case'7':/*后退*/
            _bleMoveSpeed = _blePivotMoveSpeed;
            _bleMoveDir = 1;

            break;

        case'a':/*速度增加*/
            _blePivotMoveSpeed = _blePivotMoveSpeed + 50;
            if(_blePivotMoveSpeed >= 800)
            {
                _blePivotMoveSpeed = 800;
            }
            break;
        case'b':/*速度减少*/
           if(_blePivotMoveSpeed > 0)
           {
              _blePivotMoveSpeed = _blePivotMoveSpeed - 50;
           }

            if(_blePivotMoveSpeed <= 0)
            {
                _blePivotMoveSpeed = 0;
            }
            break;
        case'8':/*左平移*/
            _bleMoveDir_Y = 0;
            _bleMoveSpeed_Y = 200;
            break;
        case'9':/*右平移*/

            _bleMoveDir_Y = 1;
            _bleMoveSpeed_Y = 200;
            break;
        case'1':
            case'2':
                    break;
        default:
            auto_ControlCarStop();
        }
//        printf("_bleMoveSpeed  = %d \r\n",_bleMoveSpeed);
        MotorControl(_blePivotSteerDir, _blePivotSteerSpeed, _bleMoveDir, _bleMoveSpeed, _bleMoveDir_Y, _bleMoveSpeed_Y);
        return 1;
    }
    else
    {
        return 0;
    }

}

/***********************************************************
*@fuction	:ps2_control
*@brief		:ps2控制函数
*@param		:void
*@return	:void
*@author	:zwb
*@date		:2026-03-26
***********************************************************/
void ps2_control()
{
	 __set_PRIMASK(1);
		ps2ClearData();
		ps2GetData();
		__set_PRIMASK(0);
		if( (ps2CheckMode() == 1) && (ps2DisLinkStatus() == 0) ) /*手柄模式*/
		{
				/*获取手柄摇杆值*/
				ps2GetAnalogValue(&_left, &_right, &_Ps2key);

				if(_left.Ps2PadXValue<=128)	
				{
				_PS2blePivotSteerDir=0;
							_PS2blePivotSteerSpeed=120*(128-_left.Ps2PadXValue)/128;
				}	
				else{
						_PS2blePivotSteerDir=1;
									_PS2blePivotSteerSpeed=120*(_left.Ps2PadXValue-128)/128;
				}		

				if(_right.Ps2PadYValue<=127)
				{
				_PS2bleMoveDir=0;
							_PS2bleMoveSpeed=180*(127-_right.Ps2PadYValue)/127;
				}
				else
				{
						_PS2bleMoveDir=1;
					
							_PS2bleMoveSpeed=180*(_right.Ps2PadYValue-127)/127;
				}
				
				
				if(_right.Ps2PadXValue<=128)	
				{
				_PS2bleMoveDir_Y=0;
				_PS2bleMoveSpeed_Y=180*(128-_right.Ps2PadXValue)/128;
				}	
				else{
						_PS2bleMoveDir_Y=1;
						_PS2bleMoveSpeed_Y=180*(_right.Ps2PadXValue-128)/128;
				}	
				MotorControl(_PS2blePivotSteerDir, _PS2blePivotSteerSpeed, _PS2bleMoveDir, _PS2bleMoveSpeed, _PS2bleMoveDir_Y, _PS2bleMoveSpeed_Y);
		}
		PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  _sMotorA_Speed_M), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  _sMotorB_Speed_M),
							 wheelSpeedPidCalc(2, g_WheelSpeed[2],  _sMotorC_Speed_M), wheelSpeedPidCalc(3, g_WheelSpeed[3],  _sMotorD_Speed_M));
}


/***********************************************************
*@fuction	:serial_control
*@brief		:串口控制函数
*@param		:void
*@return	:void
*@author	:zwb
*@date		:2026-03-26
***********************************************************/

void serial_control()
{
	if(g_uartState.uartRecvTcState == 1)/*当接受到控制信息，置位自动驾驶状态*/
	{
			g_uartState.uartRecvTcState  = 0;
			_linkStatus.autoStatus = 1;
			_linkStatus.disLinkStatus = 0;
			_disLinkCnt = 0;
	}
	else
	{

			if(_disLinkCnt < 25) /*累计没有接收到数据的次数*/
			{
					_disLinkCnt ++;
			}
			else
			{

					_linkStatus.disLinkStatus = 1;
					_linkStatus.autoStatus    = 0;
			}
	}

	if(_linkStatus.autoStatus == 1)
	{

			/*只对数据部分做校验*/
			uint8_t _xorCheck = XorCheck(g_uartFrame.buf, g_uartFrame.len);
			//				/*检查校验*/
			if(_xorCheck == g_uartFrame.xorcheck)
			{
					/*执行自动驾驶控制命令*/
					//                  printf("auto----\r\n");
					_linkStatus.autoStatus    = 0;
					auto_ControlCarCmdHandle();
			}

	}
	else if(_linkStatus.disLinkStatus == 1)
	{
			/*断连控制*/
			g_uartFrame.buf[8] = 0;
			g_uartFrame.buf[9] = 0;
			g_uartFrame.buf[10] = 0;
			auto_ControlCarStop();
	}
	PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  _sMotorA_Speed_M), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  _sMotorB_Speed_M),
						 wheelSpeedPidCalc(2, g_WheelSpeed[2],  _sMotorC_Speed_M), wheelSpeedPidCalc(3, g_WheelSpeed[3],  _sMotorD_Speed_M));
}
/***********************************************************
*@fuction	:ccd_control
*@brief		:ccd巡线模式下控制函数
*@param		:void
*@return	:void
*@author	:zwb
*@date		:2026-03-26
***********************************************************/

void ccd_control()
{
	if(CCD_ENABLE)
	{
			_sCCD_piancha = CCD_MidVal - 60;
			_sTurn_pidout = wheelTurnPidCalc(_sCCD_piancha, 0);
			PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  350 - _sTurn_pidout), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  350 - _sTurn_pidout),
								 wheelSpeedPidCalc(2, g_WheelSpeed[2],  350 + _sTurn_pidout), wheelSpeedPidCalc(3, g_WheelSpeed[3],  350 + _sTurn_pidout));
	}
}

/***********************************************************
*@fuction	:Key_Mode
*@brief		:按键单击切换MODE
*@param		:void
*@return	:void
*@author	:zwb
*@date		:2026-03-26
***********************************************************/

void Key_Mode(void)
{
		if(Flag == 0 && IsKeyDownFunc(KID_K1))
		{
          Flag = 1;
		}
		if(Flag == 1)
		{
			if(!IsKeyDownFunc(KID_K1))
			{
				MODE++;
				MODE%=7;
				Flag = 0;
			}
		}

}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
