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
//开始信号：在SCL为高电平时，SDA由高电平向低电平跳变，开始传输数据。
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
//结束信号：在SCL为高电平时，SDA由低电平向高电平跳变，结束传输数据。
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
//                      IIC主机产生ACK应答信号
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
//                      IIC主机不产生ACK应答信号
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
//等待应答信号到来
//返回值   1，接收应答失败
//        0，接收应答成功
/**********************************************/
unsigned char IIC_WaitAck(void) //测数据信号的电平
{
    uint8_t ucErrTime=0;
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU);      //SDA设置为输入
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
    GPIO_write(CONFIG_GPIO_SCL,0);//时钟输出0
    return 0;
}

/**********************************************
//                      IIC Write byte
//IIC写一个字节
**********************************************/
void IIC_Write_Byte(uint8_t dat)
{
  uint8_t i;

  GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_OUT_STD);
  GPIO_write(CONFIG_GPIO_SCL,0);//时钟输出0
  for(i=0;i<8;i++)
  {
      GPIO_write(CONFIG_GPIO_SDA, (dat&0x80)>>7);
      dat=dat<<1;
      //delay_us(2);   //对TEA5767芯片这三个延时都是必须的
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
    IIC_Write_Byte(Device_addr | 0x00);  //器件地址(7位) + 读写位(读1写0)
    IIC_WaitAck();
    IIC_Write_Byte(Reg_addr);                        //要写入数据的寄存器地址
    IIC_WaitAck();
    IIC_Write_Byte(data);
    IIC_WaitAck();
    IIC_Stop();
}

/**********************************************
//                      IIC Read byte
//IIC读一个字节
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
**********************************************/
unsigned char IIC_Read_Byte(unsigned char Ack_EN)
{
    unsigned char i,receive=0;
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU); //SDA设置为输入
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
        IIC_NACK();//发送nACK
    else
        IIC_ACK(); //发送ACK
    return receive;
}

/**********************************************
// IIC Read Data
**********************************************/
unsigned char IIC_Read_Data(unsigned char Device_addr,unsigned char Reg_addr,unsigned char ACK_EN)
{
    unsigned char receive;
    IIC_Start();
    IIC_Write_Byte(Device_addr | 0x00);  //器件地址(7位) + 读写位(写0)
    IIC_Wait_Ack();
    IIC_Write_Byte(Reg_addr);                        //要读数据的寄存器地址
    IIC_Wait_Ack();
    IIC_Start();
    GPIO_setConfig(CONFIG_GPIO_SDA, GPIO_CFG_IN_PU);
    IIC_Write_Byte(Device_addr | 0x01);  //器件地址(7位) + 读写位(读1)
    IIC_Wait_Ack();
    receive=IIC_Read_Byte(ACK_EN);
    IIC_Wait_Ack();
    IIC_Stop();
    return receive;
}
/**********************************************************************************/
/**********************************************************************************/
/*****************************以下为OLED操作函数**********************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
//写命令
void OLED_WriteCmd(unsigned char IIC_Command)
{
    IIC_Write_Data(OLED_ADDRESS,0x00,IIC_Command);
}

//写数据
void OLED_WriteDat(unsigned char I2C_Data)
{
    IIC_Write_Data(OLED_ADDRESS,0x40,I2C_Data);
}

void OLED_init()
{
    vTaskDelay(100);
    OLED_IIC_Init();
    OLED_WriteCmd(0xAE); //显示关闭
    OLED_WriteCmd(0x20);    //显示设置内存寻址模式
    OLED_WriteCmd(0x10);    //00，水平寻址模式；01，垂直寻址模式；10，页寻址模式（重置）；11，无效    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xb0);    //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc0);    //设置COM输出扫描方向
    OLED_WriteCmd(0x00); //---设置低列地址
    OLED_WriteCmd(0x10); //---设置高列地址
    OLED_WriteCmd(0x40); //---设置起始行地址
    OLED_WriteCmd(0x81); //---设置对比度控制寄存器
    OLED_WriteCmd(0xff); //---亮度调节 0x00~0xff
    OLED_WriteCmd(0xa0); //---设置段重新映射，0~127
    OLED_WriteCmd(0xa6); //---设置正常显示
    OLED_WriteCmd(0xa8); //---设置多路复用比率（1到64）
    OLED_WriteCmd(0x3F); //
    OLED_WriteCmd(0xa4); //---0xA4，输出跟随RAM内容；0xA5，输出忽略RAM内容
    OLED_WriteCmd(0xd3); //---设置显示偏移
    OLED_WriteCmd(0x00); //---不偏移
    OLED_WriteCmd(0xd5); //---设置显示时钟分频比/振荡器频率
    OLED_WriteCmd(0xf0); //---设置分割比率
    OLED_WriteCmd(0xd9); //---设置重装载时间
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //---设置COM针硬件配置
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--设置VCOMH
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
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
    OLED_WriteCmd(0X8D);  //设置电荷泵
    OLED_WriteCmd(0X14);  //开启电荷泵
    OLED_WriteCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    OLED_WriteCmd(0X8D);  //设置电荷泵
    OLED_WriteCmd(0X10);  //关闭电荷泵
    OLED_WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
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
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
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
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
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
        OLED_WriteCmd (0xb0+i);    //设置页地址（0~7）
        OLED_WriteCmd (0x00);      //设置显示位置—列低地址
        OLED_WriteCmd (0x10);      //设置显示位置—列高地址
        for(n=0;n<128;n++)OLED_WriteCmd(OLED_GRAM[n][i]);
    }

}

