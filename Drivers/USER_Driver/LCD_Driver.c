/**
 *  @file LCD_Driver.c
 *
 *  @date 2022-4-1
 *
 *  @author PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief  LCD驱动（泰斯强厂商）
 *
 *  @details 1、
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "LCD_Driver.h"
#include "main.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
#define SET_SPI_DATASIZE_8BIT   MODIFY_REG(hspi1.Instance->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT)
#define SET_SPI_DATASIZE_16BIT  MODIFY_REG(hspi1.Instance->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT)


/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/


/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
static void  SendDataSPI(unsigned char dat)
{
  SET_SPI_DATASIZE_8BIT;
  unsigned char data = dat;
  HAL_SPI_Transmit(&hspi1, &data, 1, 0xff);
}

static void WriteComm(unsigned char com)
{
  LCD_CS_CLR;
  LCD_DC_CLR;
  SendDataSPI(com);
  LCD_CS_SET;
}

static void WriteData(unsigned char dat)
{
  LCD_CS_CLR;
  LCD_DC_SET;
  SendDataSPI(dat);
  LCD_CS_SET;
}
/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
 * @brief LCD 背光使能控制
 *
 * @param Cs 为0时，为低，为1时，为高
 */
void LCD_Driver_BL_Ctl(uint8_t Cs)
{
  if(Cs)
  {
    LCD_BL_HIGH;
  }
  else
  {
    LCD_BL_LOW;
  }
}

/**
 * @brief LCD cs使能控制
 *
 * @param Cs 为0时，CS脚为低，为1时，CS脚为高
 */
void LCD_Driver_CS_Ctl(uint8_t Cs)
{
  if(Cs)
  {
    LCD_CS_SET;
  }
  else
  {
    LCD_CS_CLR;
  }
}

void WriteData_DMA(uint16_t * data, uint16_t len)
{
  SET_SPI_DATASIZE_16BIT;
  LCD_CS_CLR;
  LCD_DC_SET;
  while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
  HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)data,  len);
}

void WriteData_16bit(uint16_t color)
{
  WriteData(color >> 8);
  WriteData(color);
}

void BlockWrite(uint32_t Xstart, uint32_t Xend, uint32_t Ystart, uint32_t Yend)
{
  //JD5858
  WriteComm(0x2A);
  WriteData(Xstart>>8);
  WriteData(Xstart);
  WriteData(Xend>>8);
  WriteData(Xend);

  WriteComm(0x2B);
  WriteData(Ystart>>8);
  WriteData(Ystart);
  WriteData(Yend>>8);
  WriteData(Yend);

  //WriteComm(0x2C);

  LCD_CS_CLR;
  LCD_DC_CLR;

  SendDataSPI(0x2c);
}

/**
 * @brief LCD初始化
 *
 */
void LCD_Driver_Init(void)
{
  /* 开启背光电源 */
  LCD_BL_HIGH;

  LCD_CS_CLR;

  LCD_RESRT_SET;
  Delay(20);

  LCD_RESRT_CLR;
  Delay(20);

  LCD_RESRT_SET;
  Delay(20);

  //PASSWORD
  WriteComm(0xDF); //Password
  WriteData(0x58);
  WriteData(0x58);
  WriteData(0xb0);

  //---------------- PAGE0 --------------
  WriteComm(0xDE);
  WriteData(0x00);

  //VCOM_SET
  WriteComm(0xB2);
  WriteData(0x01);
  WriteData(0x10);//VCOM

  //Gamma_Set
  WriteComm(0xB7);
  WriteData(0x10);//VGMP = +5.3V 0x14A
  WriteData(0x4A);
  WriteData(0x00);//VGSP = +0.0V
  WriteData(0x10);//VGMN = -5.3V 0x14A
  WriteData(0x4A);
  WriteData(0x00);//VGSN = -0.0V

  //DCDC_SEL
  WriteComm(0xBB);
  WriteData(0x01);
  WriteData(0x1D);
  WriteData(0x43);
  WriteData(0x43);
  WriteData(0x21);
  WriteData(0x21);

  //GATE_POWER
  WriteComm(0xCF);
  WriteData(0x20); //VGHO = +8V
  WriteData(0x50); //VGLO = -8V


  //SET_R_GAMMA
  WriteComm(0xC8);
  WriteData(0x7F);
  WriteData(0x52);
  WriteData(0x3B);
  WriteData(0x2A);
  WriteData(0x22);
  WriteData(0x12);
  WriteData(0x17);
  WriteData(0x04);
  WriteData(0x21);
  WriteData(0x26);
  WriteData(0x29);
  WriteData(0x4B);
  WriteData(0x3A);
  WriteData(0x45);
  WriteData(0x3A);
  WriteData(0x35);
  WriteData(0x2C);
  WriteData(0x1E);
  WriteData(0x01);
  WriteData(0x7F);
  WriteData(0x52);
  WriteData(0x3B);
  WriteData(0x2A);
  WriteData(0x22);
  WriteData(0x12);
  WriteData(0x17);
  WriteData(0x04);
  WriteData(0x21);
  WriteData(0x26);
  WriteData(0x29);
  WriteData(0x4B);
  WriteData(0x3A);
  WriteData(0x45);
  WriteData(0x3A);
  WriteData(0x35);
  WriteData(0x2C);
  WriteData(0x1E);
  WriteData(0x01);

  //-----------------------------
  // SET page4 TCON & GIP
  //------------------------------
  WriteComm(0xDE);
  WriteData(0x04);// page4

  //SETSTBA
  WriteComm(0xB2);
  WriteData(0x14);//GAP = 1 ;SAP= 4
  WriteData(0x14);

  WriteComm(0xB5);
  WriteData(0x00);//SS_Panel=0,GS_Panel=0,REV_Panel=0,CFHR=0

  //SETRGBCYC1
  WriteComm(0xB8);
  WriteData(0x74);
  WriteData(0x44);
  WriteData(0x00);
  WriteData(0x01);
  WriteData(0x01);
  WriteData(0x00);
  WriteData(0x01);
  WriteData(0x01);
  WriteData(0x00);
  WriteData(0x09);
  WriteData(0x82);
  WriteData(0x10);
  WriteData(0x8A);
  WriteData(0x03);
  WriteData(0x11);
  WriteData(0x0B);
  WriteData(0x84);
  WriteData(0x21);
  WriteData(0x8C);
  WriteData(0x05);
  WriteData(0x22);
  WriteData(0x0D);
  WriteData(0x86);
  WriteData(0x32);
  WriteData(0x8E);
  WriteData(0x07);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //SETRGBCYC2
  WriteComm(0xB9);
  WriteData(0x40);
  WriteData(0x22);
  WriteData(0x08);
  WriteData(0x3A);
  WriteData(0x22);
  WriteData(0x4B);
  WriteData(0x7D);
  WriteData(0x22);
  WriteData(0x8D);
  WriteData(0xBF);
  WriteData(0x32);
  WriteData(0xD0);
  WriteData(0x02);
  WriteData(0x33);
  WriteData(0x12);
  WriteData(0x44);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x0A);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x0A);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x0A);

  //SETRGBCYC3
  WriteComm(0xBA);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x07);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x07);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x07);
  WriteData(0x00);
  WriteData(0x01);
  WriteData(0x01);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x01);
  WriteData(0x00);
  WriteData(0x01);
  WriteData(0x30);
  WriteData(0x0A);
  WriteData(0x40);
  WriteData(0x30);
  WriteData(0x01);
  WriteData(0x3E);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //SETRGBCYC3
  WriteComm(0xBB);
  WriteData(0x11);
  WriteData(0x66);
  WriteData(0x66);
  WriteData(0xA0);
  WriteData(0x80);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x60);
  WriteData(0x00);
  WriteData(0xC0);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x40);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //SET_TCON
  WriteComm(0xBC);
  WriteData(0x1A);
  WriteData(0x00);
  WriteData(0xB4);
  WriteData(0x03);
  WriteData(0x00);
  WriteData(0xD0);
  WriteData(0x08);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x2C);
  WriteData(0x00);
  WriteData(0xD0);
  WriteData(0x08);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x2C);
  WriteData(0x82);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x00);
  WriteData(0xD0);
  WriteData(0x08);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x2C);

  //SET_GIP_EQ
  WriteComm(0xC4);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x02);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x02);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x02);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //SET_GIP_L
  WriteComm(0xC5);
  WriteData(0x00);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1E);
  WriteData(0xDF);
  WriteData(0x1F);
  WriteData(0xC7);
  WriteData(0xC5);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);

  //SET_GIP_R
  WriteComm(0xC6);
  WriteData(0x00);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x00);
  WriteData(0xC4);
  WriteData(0xC6);
  WriteData(0xE0);
  WriteData(0xE1);
  WriteData(0xE2);
  WriteData(0xE3);
  WriteData(0xE4);
  WriteData(0xE5);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);

  //SET_GIP_L_GS
  WriteComm(0xC7);
  WriteData(0x00);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0xDE);
  WriteData(0x1F);
  WriteData(0x00);
  WriteData(0xC4);
  WriteData(0xC6);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);

  //SET_GIP_R_GS
  WriteComm(0xC8);
  WriteData(0x00);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0xC7);
  WriteData(0xC5);
  WriteData(0x20);
  WriteData(0x21);
  WriteData(0x22);
  WriteData(0x23);
  WriteData(0x24);
  WriteData(0x25);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);
  WriteData(0x1F);

  //SETGIP1
  WriteComm(0xC9);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x10);
  WriteData(0x00);
  WriteData(0x10);
  WriteData(0x10);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x20);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //SETGIP2
  WriteComm(0xCB);
  WriteData(0x01);
  WriteData(0x10);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x07);
  WriteData(0x01);
  WriteData(0x00);
  WriteData(0x0A);
  WriteData(0x00);
  WriteData(0x02);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x21);
  WriteData(0x23);
  WriteData(0x30);
  WriteData(0x00);
  WriteData(0x08);
  WriteData(0x04);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x05);
  WriteData(0x10);
  WriteData(0x01);
  WriteData(0x04);
  WriteData(0x06);
  WriteData(0x10);
  WriteData(0x10);


  //SET_GIP_ONOFF
  WriteComm(0xD1);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x60);
  WriteData(0x30);
  WriteData(0x03);
  WriteData(0x18);
  WriteData(0x30);
  WriteData(0x07);
  WriteData(0x3A);
  WriteData(0x30);
  WriteData(0x03);
  WriteData(0x18);
  WriteData(0x30);
  WriteData(0x03);
  WriteData(0x18);
  WriteData(0x30);
  WriteData(0x03);
  WriteData(0x18);

  WriteComm(0xD2);
  WriteData(0x00);
  WriteData(0x30);
  WriteData(0x07);
  WriteData(0x3A);
  WriteData(0x32);
  WriteData(0xBC);
  WriteData(0x20);
  WriteData(0x32);
  WriteData(0xBC);
  WriteData(0x20);
  WriteData(0x32);
  WriteData(0xBC);
  WriteData(0x20);
  WriteData(0x32);
  WriteData(0xBC);
  WriteData(0x20);
  WriteData(0x30);
  WriteData(0x10);
  WriteData(0x20);
  WriteData(0x30);
  WriteData(0x10);
  WriteData(0x20);
  WriteData(0x30);
  WriteData(0x10);
  WriteData(0x20);
  WriteData(0x30);
  WriteData(0x10);
  WriteData(0x20);

  WriteComm(0xD4);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x14);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x20);
  WriteData(0x00);
  WriteData(0x09);
  WriteData(0x82);
  WriteData(0x10);
  WriteData(0x8A);
  WriteData(0x03);
  WriteData(0x11);
  WriteData(0x0B);
  WriteData(0x84);
  WriteData(0x21);
  WriteData(0x8C);
  WriteData(0x05);
  WriteData(0x22);
  WriteData(0x0D);
  WriteData(0x86);
  WriteData(0x32);
  WriteData(0x8E);
  WriteData(0x07);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);

  //---------------- PAGE0 --------------
  WriteComm(0xDE);
  WriteData(0x00);
  // RAM_CTRL
  WriteComm(0xD7);
  WriteData(0x20);//GM=1;RP=0;RM=0;DM=00  0x20
  WriteData(0x00);
  WriteData(0x00);//0x00 : 1DL ; 0x40 : 2DL without SPI_A0_EN; 0x41 : 2DL with SPI_A0_EN

  //---------------- PAGE1 --------------
  WriteComm(0xDE);
  WriteData(0x01);

  ////MCMD_CTRL
  WriteComm(0xCA);
  WriteData(0x00);//00

  //---------------- PAGE2 --------------
  WriteComm(0xDE);
  WriteData(0x02);

  //OSC DIV
  WriteComm(0xC5);
  WriteData(0x03); //FPS 60HZ (0x03) to 30HZ (0x0B) ,47HZ(0x0F),42HZ(0x0E)  0x03

  //---------------- PAGE4 --------------
  //WriteComm(0xDE);
  //WriteData(0x04);

  //WriteComm(0xb7);
  //WriteData(0x5C); //Bist mode
  //WriteData(0x82);


  //---------------- PAGE0 --------------
  WriteComm(0xDE);
  WriteData(0x00);

  //Color Pixel Format
  WriteComm(0x3A);
  WriteData(0x05);//RGB565

  //TE ON
  WriteComm(0x35);
  WriteData(0x00);

  //MADCTL
  WriteComm(0x36);
  WriteData(0x03);//00 01 02 03   0x00

  //SLP OUT
  WriteComm(0x11);// SLPOUT
  Delay(40);

  //The host could send 1st image at this time

  //DISP ON
  WriteComm(0x29);// DSPON
  Delay(40);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
