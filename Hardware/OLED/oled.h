#ifndef __OLED_H
#define __OLED_H
/**********************************************ʹ�÷���***********************************************/
//ʹ��ʱ�����³�ʼ����
// 1. OLED_init();
// 2. OLED_ON();
// 3. OLED_Fill(0x00); 0x00Ϊ���ԣ�0xffΪ����
//�����ʾ��
//ʹ��OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)����������ʾ
//�������ֵȷ��ַ�����ʾ����Ҫʹ��stdio���sprinf��������ת��Ϊ�ַ�������ʾ������
// sprintf(str_buf,"%5.1f %5.1f",Kp,Kd);
// OLED_ShowStr(0,4,(u8*)str_buf,1);  str_bufΪ�Զ�����ַ��������Ա�������
/*****************************************************************************************************/

#define OLED_ADDRESS 0x78  //ͨ������0R����,������0x78��0x7A������ַ -- Ĭ��0x78

#define OLED_CMD  0  //д����
#define OLED_DATA 1  //д����

#define SIZE       16
#define XLevelL    0x00
#define XLevelH    0x10
#define Max_Column 128
#define Max_Row    64
#define Brightness 0xFF
#define X_WIDTH    128
#define Y_WIDTH    64

/////////////////////////////////////////////////////////////
// OLED_IICЭ�麯��///////////////////////////////////////////
void          OLED_IIC_Init(void);
void          IIC_Start(void);
void          IIC_Stop(void);
void          IIC_ACK(void);
void          IIC_NACK(void);
unsigned char IIC_Wait_Ack(void);
void          IIC_Write_Byte(unsigned char data);
void          IIC_Write_Data(unsigned char Device_addr, unsigned char Reg_addr, unsigned char data);
unsigned char IIC_Read_Byte(unsigned char Ack_EN);
unsigned char IIC_Read_Data(unsigned char Device_addr, unsigned char Reg_addr, unsigned char ACK_EN);
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// OLED�����ú���///////////////////////////////////////////
void OLED_WriteCmd(unsigned char I2C_Command);       // -- д����
void OLED_WriteDat(unsigned char I2C_Data);          //-- д����
void OLED_init(void);                                //-- OLED����ʼ��
void OLED_SetPos(unsigned char x, unsigned char y);  //-- ������ʼ������
void OLED_Fill(unsigned char fill_Data);             //-- ȫ�����  0xff
void OLED_CLS(void);                                 //-- ����
void OLED_ON(void);                                  //-- ����
void OLED_OFF(void);                                 //-- ˯��
void OLED_Refresh_Gram(void);                        //-- ˢ��
/*-- ��ʾ�ַ���(�����С��6*8��8*16����)****/
//ֻ��ʾ�ַ�������ʾ����
/*-- ����OLED_ShowStr(0,0,"abc",2);*/
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);

/*-- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��*/
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);

/*-- ��ʾ-- BMPͼƬ*/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);

#endif
