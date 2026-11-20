#ifndef COMM_H
#define COMM_H

/*  
使能CCD，CCD clk引脚与第六路舵机冲突，使用第六路舵机时不使能CCD，使用CCD置为1，使用第六路舵机时置为0
*/
#define CCD_ENABLE 0

/*自定义数据类型，保证能都对齐数据长度*/
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef char int8;
typedef short int16;
typedef int int32;

/*串口发送函数定义，替换成自己的函数*/
#define KDCOM_SendData(pdata, length)     comSendBuf(COM1, (uint8_t*)pdata, length)

typedef struct kdrobot
{
    uint8 head;
    uint8 buf[8];
    uint8 check;
}KDRobot;

/*校验函数，内部调用*/
uint8 KDCOM_Check(KDRobot* buf);

/*从缓冲区中解析出指令，指令详见协议说明书*/
uint8 KDCOM_GetCmd(KDRobot* buf);

/*从缓冲区中解析出指令对应的参数，详见协议说明书*/
uint8 KDCOM_GetCmdPara(KDRobot* buf, uint8 paraNum);

/*从缓冲区中解析出指令附带的数据，详见协议说明数书*/
float KDCOM_GetCmdData(KDRobot* buf);

/*从缓冲区中解析出数据帧的两个数据，dataNum为第几个数据（1,2），详见协议说明数书*/
float KDCOM_GetData(KDRobot* buf, uint8 dataNum);

/*通过参数组装指令帧，返回缓冲区指针，参数详见协议说明书*/
uint8* KDCOM_SetCmd(KDRobot* buf, uint8 cmd, uint8 para1, uint8 para2, uint8 para3, float data);

/*通过参数组装数据帧，返回缓冲区指针，参数详见协议说明书*/
uint8* KDCOM_SetData(KDRobot* buf, uint8 channel, float data1, float data2);

/*指令和数据帧解析函数，通过此函数解析接收到的数据。
buf为接收缓冲区
channel为通道(0-3)
p为参数P的指针
i为参数I的指针
d为参数D的指针
traget为参数目标值的指针
actual为参数实际值的指针
start为开始信号的指针，接收到开始信号时会被置1
stop为停止信号的指针，接收到停止信号时会被置1
data1和data2为上位机传来数据时的两个数据，暂时没用可置为0
注：除了最后两个参数之外的参数都不能置0，否则会发生预料之外的错误*/
uint8 KDCOM_Process(uint8* buf, uint8 *channel, float *p[4], float *i[4], float *d[4], int16_t *traget[4], int16_t *actual[4], 
                    uint8 *start, uint8 *stop, float *data1, float *data2);


#endif // COMM_H

