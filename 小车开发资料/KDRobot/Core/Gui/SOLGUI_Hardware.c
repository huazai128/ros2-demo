#include "SOLGUI_Type.h"
#include "SOLGUI_Config.h"
#include "SOLGUI_Hardware.h"

//*******************【屏幕驱动头文件】*******************

extern void OLED_Init(void);
extern void OLED_Refresh(u8 OLED_GRAM[SCREEN_X_WIDTH][SCREEN_Y_PAGE]);
extern void OLED_Clear(void);
//--------------[OLED初始化函数]----------
#define pixelC_HW_OLED_Init()  			  OLED_Init()
//--------------[更新显示函数]------------
#define pixelC_HW_OLED_FillScreen(GRAM)   OLED_Refresh(SOLGUI_GRAM)
//--------------[硬件清屏函数]
#define pixelC_HW_OLED_CLS()   			  OLED_Clear()

//////

extern u8 SOLGUI_GRAM[SCREEN_X_WIDTH][SCREEN_Y_PAGE];

void SOLGUI_Hardware_Init(void)
{
//=========【用户硬件初始化移植处】=======
	pixelC_HW_OLED_Init();

//========================================
}

void SOLGUI_Hardware_FillScreen(u8 GRAM[SCREEN_X_WIDTH][SCREEN_Y_PAGE])
{
//=========【用户硬件屏幕填充函数移植处】=======
	pixelC_HW_OLED_FillScreen(GRAM);
//========================================
}

void SOLGUI_Hardware_ClearScreen(void)
{
//=========【用户硬件清屏函数移植处】=======
	pixelC_HW_OLED_CLS();		
//========================================	
}



