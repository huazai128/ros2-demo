#ifndef SOLGUI_CONFIG_H
#define SOLGUI_CONFIG_H

//##############################��Ӳ���㡿##############################
//--------------����Ļ������������
#define SCREEN_X_WIDTH 			128				//��Ļ��X��������
#define SCREEN_Y_WIDTH 			64				//��Ļ��Y��������

//--------------����Ļ����������		
#define SCREEN_X_PAGE 	(SCREEN_X_WIDTH/6)		//֧�ֶ��ٸ�6x8�ֿ���һ�㲻��Ҫ�޸ģ�Ĭ�ϼ��ɣ�
#define SCREEN_Y_PAGE 	(SCREEN_Y_WIDTH/8)		//֧�ֶ��ٸ�6x8�ָߣ�һ�㲻��Ҫ�޸ģ�Ĭ�ϼ��ɣ�

//##############################���м�㡿##############################
//--------------���ֿ⿪�ء�
/*----------------Ĭ��ʹ��6x8����----------------*/
/*---------�ɸ���ʵ��ʹ��������أ���Լ�ռ�---------*/
#define FONT4X6_EN 				0	 		//����ʹ��4x6����
#define FONT8X8_EN 				0	 		//����ʹ��8x8����
#define FONT8X10_EN				1	 		//����ʹ��8x10����

//--------------������ͼ�ο⿪�ء�
#define GBASIC_LINE_EN				1			//����ʹ��ֱ�߻���
#define GBASIC_RECTANGLE_EN		1			//����ʹ�þ��λ���
#define GBASIC_CIRCLE_EN			1			//����ʹ��Բ�λ���



//##############################��Ӧ�ò㡿##############################
//--------------��SOLGUI�˵����ǰ̨ʹ�ÿ��ء�
/*---------�رպ��ܵ���Ӧ�ò��к��������޷�ʹ�ò˵����---------*/
#define MENU_FRAME_EN 			0	 			//����SOLGUI�˵������Ϊǰ̨

//--------------��Widget���ء�
/*---------�ɸ���ʵ��ʹ��������أ���Լ�ռ�---------*/
#define WIDGET_GOTOPAGE_EN 		1				//����ʹ�ÿؼ���GotoPageҳ����ת
#define WIDGET_SPIN_EN 			1				//����ʹ�ÿؼ���Spin������ť
#define WIDGET_OPTIONTEXT_EN 	1				//����ʹ�ÿؼ���OptionTextѡ���ı�
#define WIDGET_BUTTON_EN 		1				//����ʹ�ÿؼ���Button����
#define WIDGET_SWITCH_EN		1				//����ʹ�ÿؼ���Switch��ѡ����
#define WIDGET_EDIT_EN 			1				//����ʹ�ÿؼ���Edit�ı��༭
										 	
#define WIDGET_TEXT_EN 			1				//����ʹ�ÿؼ���Text����
#define WIDGET_BAR_EN 			1				//����ʹ�ÿؼ���Bar��
#define WIDGET_SPECTRUM_EN 		1				//����ʹ�ÿؼ���Spectrum��
#define WIDGET_OSCILLOGRAM_EN 	1				//����ʹ�ÿؼ���Oscillogram����
#define WIDGET_PICTURE_EN		1				//����ʹ�ÿؼ���Pictureͼ

//--------------��ѡ�������Ŀ��
#define OPTIONS_MAX				8				//ÿҳ���������ɵ�ѡ��������������̫��

//--------------��FIFO��ֵ�����С��
#define FIFOBUF_SIZE			5				//���Ի���ļ�ֵ����Ĭ�ϼ��ɣ�

//--------------����ֵ��Ӧ��Ϊ���á�
/*---------�û������ϵͳ�������صļ�ֵ�������趨---------*/
#define	SOLGUI_KEY_UP				0x50			//��
#define SOLGUI_KEY_DOWN			0x20			//��
#define	SOLGUI_KEY_LEFT			0x30			//��
#define SOLGUI_KEY_RIGHT		0x10			//��
#define SOLGUI_KEY_OK				0x40 			//ȷ��
#define	SOLGUI_KEY_BACK			0x60			//����

//--------------��ϵͳͼ�����á�
/*---------����û��������Զ���ͼ�꣬�˴�Ĭ�ϼ��ɣ������޸�---------*/
#define	ICON_UP								0x80			//�ϣ�������Font6x8_ASCII.c�����ã�
#define	ICON_DOWN							0x81			//�£�������Font6x8_ASCII.c�����ã�
#define	ICON_LEFT							0x82			//�󣨿�����Font6x8_ASCII.c�����ã�
#define	ICON_RIGHT						0x83			//�ң�������Font6x8_ASCII.c�����ã�
#define	ICON_OK								0x84			//OK��������Font6x8_ASCII.c�����ã�
#define	ICON_BACK							0x85			//���أ�������Font6x8_ASCII.c�����ã�
#define	ICON_CURSOR 					0x86			//��꣨������Font6x8_ASCII.c�����ã�
#define	ICON_WIDGET_GOTOPAGE 	0x87			//ҳ����ת�ؼ���������Font6x8_ASCII.c�����ã�
#define	ICON_WIDGET_EDIT			0x88			//�ı��༭�ؼ���������Font6x8_ASCII.c�����ã�
#define	ICON_OTHER_HIDE				0x89			//ʡ�Ժţ�������Font6x8_ASCII.c�����ã�

//--------------��SPIN�ؼ���������λ�����á�
#define SPIN_DIGIT_MAX 			5				//����SPIN�п����õ�����λ��������5λ�������
#define SPIN_DIGIT_MIN 			(-3)			//����SPIN�п����õ�����λ��������10^-3��

//--------------��EDIT�ؼ��������С���á�
#define EDIT_BUF_SIZE			(40+1)			//��EDIT���ַ��������󳤶ȣ�128*64�����94��
#define EDIT_THUMBNAIL_SIZE		7				//��EDITѡ����������Ϣ�������󳤶�										 	


#endif
