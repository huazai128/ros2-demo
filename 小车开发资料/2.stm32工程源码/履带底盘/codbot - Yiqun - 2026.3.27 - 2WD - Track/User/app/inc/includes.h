#ifndef __INCLUDE_H__
#define __INCLUDE_H__


#include "bsp.h"

#include "cmsis_os2.h" 
#include "motor.h"
#include "steercontrol.h"
#include "pid.h"
#include "autocontrol.h"
#include "feedback.h"
#include "SEGGER_RTT.h"
#include "tool.h"
#include "u8g2.h"
typedef struct{

	uint8_t autoStatus;
	uint8_t remoteStatus;
	uint8_t disLinkStatus;
}LinkStatus_t;

extern uint16_t _usStopTime;

#endif /*__INCLUDE_H__*/

