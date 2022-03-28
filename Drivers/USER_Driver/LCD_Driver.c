/**
 *  @file ILI9488_Driver.c
 *
 *  @date 2021-12-16
 *
 *  @author zgl
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief ILI9488驱动
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <assert.h>  
#include <math.h>
/* Private includes ----------------------------------------------------------*/
#include "gpio.h"
#include "spi.h"
#include "LCD_Driver.h"
/** Private typedef ----------------------------------------------------------*/
#define SET_SPI_DATASIZE_8BIT     MODIFY_REG(hspi1.Instance->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT)
#define SET_SPI_DATASIZE_16BIT    MODIFY_REG(hspi1.Instance->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT)
/** Private macros -----------------------------------------------------------*/
#define  Delay                    HAL_Delay
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
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

void WriteData_DMA(unsigned short* data,unsigned short len)
{
	SET_SPI_DATASIZE_16BIT;
	LCD_CS_CLR;
	LCD_DC_SET;
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)data,  len);
}

void WriteData_16bit(unsigned short color)
{
	WriteData(color >> 8);
	WriteData(color);
}

void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
//	//JD5858
	
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

void LCD_Init(void)
{
  /* 打开背光 */
  LCD_BL_HIGH; 
	
	LCD_CS_CLR;
	
	LCD_RESRT_SET; 
	Delay(20);
	
	LCD_RESRT_CLR;
	Delay(20);

	LCD_RESRT_SET;
	Delay(20);
 //---------------------------------------------------------------------------------------------------// 
	
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
	WriteData(0x01);  // 0x01
	WriteData(0x10); //VCOM  0x10

	//Gamma_Set
	WriteComm(0xB7); 
	WriteData(0x10); //VGMP = +5.3V 0x14A
	WriteData(0x4A); 
	WriteData(0x00); //VGSP = +0.0V
	WriteData(0x10); //VGMN = -5.3V 0x14A
	WriteData(0x4A); 
	WriteData(0x00); //VGSN = -0.0V
	 
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
	WriteData(0x04);  // page4

	//SETSTBA
	WriteComm(0xB2);  	
	WriteData(0x14); //GAP = 1 ;SAP= 4
	WriteData(0x14);

	WriteComm(0xB5);  	
	WriteData(0x00); //SS_Panel=0,GS_Panel=0,REV_Panel=0,CFHR=0


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

	///-----------------------------------------------------------------------------------------
	//---------------- PAGE0 --------------
	WriteComm(0xDE);  	
	WriteData(0x00); 
	// RAM_CTRL
	WriteComm(0xD7);  	
	WriteData(0x20);  //GM=1;RP=0;RM=0;DM=00  0x20
	WriteData(0x00);  
	WriteData(0x00); //0x00 : 1DL ; 0x40 : 2DL without SPI_A0_EN; 0x41 : 2DL with SPI_A0_EN

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
	WriteData(0x05); //RGB565

	//TE ON
	WriteComm(0x35);  	
	WriteData(0x00); 

	//MADCTL
	WriteComm(0x36);  	
	WriteData(0x03);//00 01 02 03   0x00

	//SLP OUT
	WriteComm(0x11);  	// SLPOUT
	Delay(40);

	//The host could send 1st image at this time

	//DISP ON
	WriteComm(0x29);  	// DSPON
	Delay(40);	
    
}

void LCD_Init_1(void)
{
  /* 打开背光 */
  LCD_BL_HIGH;
  
  LCD_CS_CLR;
	
	LCD_RESRT_SET; 
	Delay(20);
	
	LCD_RESRT_CLR;
	Delay(20);

	LCD_RESRT_SET;
	Delay(20);
  
                  
  WriteComm(0xFE);			 
	WriteComm(0xEF); 

	WriteComm(0xEB);	
	WriteData(0x14); 

	WriteComm(0x84);			
	WriteData(0x40); 

	WriteComm(0x88);			
	WriteData(0x0A);

	WriteComm(0x89);			
	WriteData(0x21); 

	WriteComm(0x8A);			
	WriteData(0x00); 

	WriteComm(0x8B);			
	WriteData(0x80); 

	WriteComm(0x8C);			
	WriteData(0x01); 

	WriteComm(0x8D);			
	WriteData(0x01); 

	WriteComm(0xB6);			
	WriteData(0x20); 

	WriteComm(0x36);			
	WriteData(0x88);   // 48

	WriteComm(0x3A);			
	WriteData(0x05); 


	WriteComm(0x90);			
	WriteData(0x08);
	WriteData(0x08);
	WriteData(0x08);
	WriteData(0x08); 

	WriteComm(0xBD);			
	WriteData(0x06);
	
	WriteComm(0xBC);			
	WriteData(0x00);	

	WriteComm(0xFF);			
	WriteData(0x60);
	WriteData(0x01);
	WriteData(0x04);

	WriteComm(0xC3);			
	WriteData(0x13);
	WriteComm(0xC4);			
	WriteData(0x13);

	WriteComm(0xC9);			
	WriteData(0x22);

	WriteComm(0xBE);			
	WriteData(0x11); 

	WriteComm(0xE1);			
	WriteData(0x10);
	WriteData(0x0E);

	WriteComm(0xDF);			
	WriteData(0x21);
	WriteData(0x0c);
	WriteData(0x02);

	WriteComm(0xF0);   
  WriteData(0x45);
  WriteData(0x09);
  WriteData(0x08);
  WriteData(0x08);
  WriteData(0x26);
 	WriteData(0x2A);

 	WriteComm(0xF1);    
 	WriteData(0x43);
 	WriteData(0x70);
 	WriteData(0x72);
 	WriteData(0x36);
 	WriteData(0x37);  
 	WriteData(0x6F);


 	WriteComm(0xF2);   
 	WriteData(0x45);
 	WriteData(0x09);
 	WriteData(0x08);
 	WriteData(0x08);
 	WriteData(0x26);
 	WriteData(0x2A);

 	WriteComm(0xF3);   
 	WriteData(0x43);
 	WriteData(0x70);
 	WriteData(0x72);
 	WriteData(0x36);
 	WriteData(0x37); 
 	WriteData(0x6F);

	WriteComm(0xED);	
	WriteData(0x1B); 
	WriteData(0x0B); 

	WriteComm(0xAE);			
	WriteData(0x74);
	
	WriteComm(0xCD);			
	WriteData(0x63);		


	WriteComm(0x70);			
	WriteData(0x07);
	WriteData(0x09);
	WriteData(0x04);
	WriteData(0x0E); 
	WriteData(0x0F); 
	WriteData(0x09);
	WriteData(0x07);
	WriteData(0x08);
	WriteData(0x03);

	WriteComm(0xE8);			
	WriteData(0x34);

	WriteComm(0x62);			
	WriteData(0x18);
	WriteData(0x0D);
	WriteData(0x71);
	WriteData(0xED);
	WriteData(0x70); 
	WriteData(0x70);
	WriteData(0x18);
	WriteData(0x0F);
	WriteData(0x71);
	WriteData(0xEF);
	WriteData(0x70); 
	WriteData(0x70);

	WriteComm(0x63);			
	WriteData(0x18);
	WriteData(0x11);
	WriteData(0x71);
	WriteData(0xF1);
	WriteData(0x70); 
	WriteData(0x70);
	WriteData(0x18);
	WriteData(0x13);
	WriteData(0x71);
	WriteData(0xF3);
	WriteData(0x70); 
	WriteData(0x70);

	WriteComm(0x64);			
	WriteData(0x28);
	WriteData(0x29);
	WriteData(0xF1);
	WriteData(0x01);
	WriteData(0xF1);
	WriteData(0x00);
	WriteData(0x07);

	WriteComm(0x66);			
	WriteData(0x3C);
	WriteData(0x00);
	WriteData(0xCD);
	WriteData(0x67);
	WriteData(0x45);
	WriteData(0x45);
	WriteData(0x10);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);

	WriteComm(0x67);			
	WriteData(0x00);
	WriteData(0x3C);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
	WriteData(0x54);
	WriteData(0x10);
	WriteData(0x32);
	WriteData(0x98);

	WriteComm(0x74);			
	WriteData(0x10);	
	WriteData(0x85);	
	WriteData(0x80);
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x4E);
	WriteData(0x00);					
	
  WriteComm(0x98);			
	WriteData(0x3e);
	WriteData(0x07);



	WriteComm(0x35);	
	WriteComm(0x21);
	Delay(120);
	//--------end gamma setting--------------//

WriteComm(0x11);
Delay(320);
WriteComm(0x29);
Delay(120);
WriteComm(0x2C);

//--------end gamma setting--------------//
WriteComm(0x2A);
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0xEF);

WriteComm(0x2B);
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0xEF);

WriteComm(0x11);
Delay(120);
WriteComm(0x29);

WriteComm(0x2C);
}

