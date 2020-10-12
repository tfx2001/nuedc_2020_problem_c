#include "oled.h"
#include "codetab.h"

#include <FreeRTOS.h>
#include <task.h>

#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

void OLED_IIC_Init()
{
    // Configure an iic output pin
    GPIO_setConfig(CONFIG_GPIO_SCL, GPIO_CFG_OUT_STD);
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
}

/**********************************************/
//                      IIC Start
//��ʼ�źţ���SCLΪ�ߵ�ƽʱ��SDA�ɸߵ�ƽ��͵�ƽ���䣬��ʼ�������ݡ�
/**********************************************/
void IIC_Start(void)
{
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
    GPIO_write(CONFIG_GPIO_SDA,1);
    GPIO_write(CONFIG_GPIO_SCL,1);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SDA,0);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SCL,0);
}

/**********************************************/
//                      IIC Stop
//�����źţ���SCLΪ�ߵ�ƽʱ��SDA�ɵ͵�ƽ��ߵ�ƽ���䣬�����������ݡ�
/**********************************************/
void IIC_Stop(void)
{
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
    GPIO_write(CONFIG_GPIO_SCL,1);
    GPIO_write(CONFIG_GPIO_SDA,0);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SDA,1);
    //delay_us(2);
}

/**********************************************/
//                      IIC��������ACKӦ���ź�
/**********************************************/
void IIC_ACK(void)
{
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
    GPIO_write(CONFIG_GPIO_SCL,0);
    GPIO_write(CONFIG_GPIO_SDA,0);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SCL,1);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SCL,0);
}

/**********************************************/
//                      IIC����������ACKӦ���ź�
/**********************************************/
void IIC_NACK(void)

{
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
    GPIO_write(CONFIG_GPIO_SCL,0);
    GPIO_write(CONFIG_GPIO_SDA,1);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SCL,1);
    //delay_us(2);
    GPIO_write(CONFIG_GPIO_SCL,0);
}

/**********************************************/
//                      IIC Wait Ack
//�ȴ�Ӧ���źŵ���
//����ֵ   1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
/**********************************************/
unsigned char IIC_WaitAck(void) //�������źŵĵ�ƽ
{
    uint8_t ucErrTime=0;
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU);      //SDA����Ϊ����
    GPIO_write(CONFIG_GPIO_SDA,1);
    GPIO_write(CONFIG_GPIO_SCL,1);
    //delay_us(2);
    while(GPIO_read(CONFIG_GPIO_SDA))
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    GPIO_write(CONFIG_GPIO_SCL,0);//ʱ�����0
    return 0;
}

/**********************************************
//                      IIC Write byte
//IICдһ���ֽ�
**********************************************/
void IIC_Write_Byte(uint8_t dat)
{
  uint8_t i;

  GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
  GPIO_write(CONFIG_GPIO_SCL,0);//ʱ�����0
  for(i=0;i<8;i++)
  {
      GPIO_write(CONFIG_GPIO_SDA, (dat&0x80)>>7);
      dat=dat<<1;
      //delay_us(2);   //��TEA5767оƬ��������ʱ���Ǳ����
      GPIO_write(CONFIG_GPIO_SCL,1);
      //  delay_us(2);
      GPIO_write(CONFIG_GPIO_SCL,0);
      //  delay_us(2);
  }
}

/**********************************************
// IIC Write Data
**********************************************/
void IIC_Write_Data(unsigned char Device_addr,unsigned char Reg_addr,unsigned char data)
{
    IIC_Start();
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
    IIC_Write_Byte(Device_addr | 0x00);  //������ַ(7λ) + ��дλ(��1д0)
    IIC_WaitAck();
    IIC_Write_Byte(Reg_addr);                        //Ҫд�����ݵļĴ�����ַ
    IIC_WaitAck();
    IIC_Write_Byte(data);
    IIC_WaitAck();
    IIC_Stop();
}

/**********************************************
//                      IIC Read byte
//IIC��һ���ֽ�
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
**********************************************/
unsigned char IIC_Read_Byte(unsigned char Ack_EN)
{
    unsigned char i,receive=0;
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU); //SDA����Ϊ����
    for(i=0;i<8;i++ )
    {
        GPIO_write(CONFIG_GPIO_SCL,0);
        //delay_us(2);
        GPIO_write(CONFIG_GPIO_SCL,1);
        receive<<=1;
    if(GPIO_read(CONFIG_GPIO_SDA))receive++;
        //delay_us(1);
    }
    if (!Ack_EN)
        IIC_NACK();//����nACK
    else
        IIC_ACK(); //����ACK
    return receive;
}

/**********************************************
// IIC Read Data
**********************************************/
unsigned char IIC_Read_Data(unsigned char Device_addr,unsigned char Reg_addr,unsigned char ACK_EN)
{
    unsigned char receive;
    IIC_Start();
    IIC_Write_Byte(Device_addr | 0x00);  //������ַ(7λ) + ��дλ(д0)
    IIC_Wait_Ack();
    IIC_Write_Byte(Reg_addr);                        //Ҫ�����ݵļĴ�����ַ
    IIC_Wait_Ack();
    IIC_Start();
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU);
    IIC_Write_Byte(Device_addr | 0x01);  //������ַ(7λ) + ��дλ(��1)
    IIC_Wait_Ack();
    receive=IIC_Read_Byte(ACK_EN);
    IIC_Wait_Ack();
    IIC_Stop();
    return receive;
}
/**********************************************************************************/
/**********************************************************************************/
/*****************************����ΪOLED��������**********************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
//д����
void OLED_WriteCmd(unsigned char IIC_Command)
{
    IIC_Write_Data(OLED_ADDRESS,0x00,IIC_Command);
}

//д����
void OLED_WriteDat(unsigned char I2C_Data)
{
    IIC_Write_Data(OLED_ADDRESS,0x40,I2C_Data);
}

void OLED_init()
{
    vTaskDelay(100);
    OLED_IIC_Init();
    OLED_WriteCmd(0xAE); //��ʾ�ر�
    OLED_WriteCmd(0x20);    //��ʾ�����ڴ�Ѱַģʽ
    OLED_WriteCmd(0x10);    //00��ˮƽѰַģʽ��01����ֱѰַģʽ��10��ҳѰַģʽ�����ã���11����Ч    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xb0);    //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc0);    //����COM���ɨ�跽��
    OLED_WriteCmd(0x00); //---���õ��е�ַ
    OLED_WriteCmd(0x10); //---���ø��е�ַ
    OLED_WriteCmd(0x40); //---������ʼ�е�ַ
    OLED_WriteCmd(0x81); //---���öԱȶȿ��ƼĴ���
    OLED_WriteCmd(0xff); //---���ȵ��� 0x00~0xff
    OLED_WriteCmd(0xa0); //---���ö�����ӳ�䣬0~127
    OLED_WriteCmd(0xa6); //---����������ʾ
    OLED_WriteCmd(0xa8); //---���ö�·���ñ��ʣ�1��64��
    OLED_WriteCmd(0x3F); //
    OLED_WriteCmd(0xa4); //---0xA4���������RAM���ݣ�0xA5���������RAM����
    OLED_WriteCmd(0xd3); //---������ʾƫ��
    OLED_WriteCmd(0x00); //---��ƫ��
    OLED_WriteCmd(0xd5); //---������ʾʱ�ӷ�Ƶ��/����Ƶ��
    OLED_WriteCmd(0xf0); //---���÷ָ����
    OLED_WriteCmd(0xd9); //---������װ��ʱ��
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //---����COM��Ӳ������
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--����VCOMH
    OLED_WriteCmd(0x20); //0x20,0.77xVcc
    OLED_WriteCmd(0x8d); //--set DC-DC enable
    OLED_WriteCmd(0x14); //
    OLED_WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y)
{
    OLED_WriteCmd(0xb0+y);
    OLED_WriteCmd(((x&0xf0)>>4)|0x10);
    OLED_WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        OLED_WriteCmd(0xb0+m);      //page0-page1
        OLED_WriteCmd(0x00);        //low column start address
        OLED_WriteCmd(0x10);        //high column start address
        for(n=0;n<128;n++)
        {
            OLED_WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)
{
    OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
    OLED_WriteCmd(0X8D);  //���õ�ɱ�
    OLED_WriteCmd(0X14);  //������ɱ�
    OLED_WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    OLED_WriteCmd(0X8D);  //���õ�ɱ�
    OLED_WriteCmd(0X10);  //�رյ�ɱ�
    OLED_WriteCmd(0XAE);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0,i = 0,j = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 126)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<6;i++)
                    OLED_WriteDat(F6x8[c][i]);
                x += 6;
                j++;
            }
        }break;
        case 2:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 120)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<8;i++)
                    OLED_WriteDat(F8X16[c*16+i]);
                OLED_SetPos(x,y+1);
                for(i=0;i<8;i++)
                    OLED_WriteDat(F8X16[c*16+i+8]);
                x += 8;
                j++;
            }
        }break;
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned char wm=0;
    unsigned int  adder=32*N;
    OLED_SetPos(x , y);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WriteDat(F16x16[adder]);
        adder += 1;
    }
    OLED_SetPos(x,y + 1);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WriteDat(F16x16[adder]);
        adder += 1;
    }
}


//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

  if(y1%8==0)
        y = y1/8;
  else
        y = y1/8 + 1;
    for(y=y0;y<y1;y++)
    {
        OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
        {
            OLED_WriteDat(BMP[j++]);
        }
    }
}


uint8_t OLED_GRAM[128][8];
void OLED_Refresh_Gram(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WriteCmd (0xb0+i);    //����ҳ��ַ��0~7��
        OLED_WriteCmd (0x00);      //������ʾλ�á��е͵�ַ
        OLED_WriteCmd (0x10);      //������ʾλ�á��иߵ�ַ
        for(n=0;n<128;n++)OLED_WriteCmd(OLED_GRAM[n][i]);
    }

}

