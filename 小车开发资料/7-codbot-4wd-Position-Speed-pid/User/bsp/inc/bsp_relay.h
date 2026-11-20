#ifndef __BSP_RELAY_H__

#define __BSP_RELAY_H__



void bsp_InitRelay(void);
void RelayTest(void);
void RelayControl(uint8_t _num, uint8_t _activeLevel);
#endif /*__BSP_RELAY_H__*/