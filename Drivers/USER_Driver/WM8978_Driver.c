/**
 *  @file WM8978_Driver.c
 *
 *  @date 2021-01-06
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief WM8978驱动
 *
 *  @details 1、mode 下拉 两线模式
 *           2、mode 上拉 三线模式
 *           3、I2S接口，支持最高48K,24bit音频播放（音频数据的接收与发送）默认16k
 *           4、DAC信噪比98dB；ADC信噪比90dB
 *           5、支持无电容耳机驱动（提供40mW@16Ω的输出能力）
 *           6、支持扬声器输出（提供0.9W@8Ω的驱动能力）
 *           7、支持立体声差分输入/麦克风输入
 *           8、支持左右声道音量独立调节
 *           9、支持3D效果，支持5路EQ调节
 *           10、寄存器只支持写操作不支持读操作
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "main.h"
#if USE_FREERTOS
#include "cmsis_os.h"
#endif
#include "WM8978_Driver.h"
#include "utilities.h"
#include "math.h"
/** Private typedef ----------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
/** Private macros -----------------------------------------------------------*/
#define USE_24BIT_DATA_FORMAT   1        /**< 是否启用24BIT数据格式，不启用则使用16bit*/
#define ENABLE_LR_OUT2          1        /**< 是否启用LOUT2/ROUT2输出，不启用则只使用LOUT1/ROUT1输出*/

#define WM8978_ADDR     0x1A
#define WM8978_ADDR_W   (WM8978_ADDR<<1) /**< 写数据地址0x34*/

#define EQ1_80Hz		    0X00
#define EQ1_105Hz		    0X01
#define EQ1_135Hz		    0X02
#define EQ1_175Hz		    0X03

#define EQ2_230Hz		    0X00
#define EQ2_300Hz		    0X01
#define EQ2_385Hz		    0X02
#define EQ2_500Hz		    0X03

#define EQ3_650Hz		    0X00
#define EQ3_850Hz		    0X01
#define EQ3_1100Hz		  0X02
#define EQ3_14000Hz		  0X03

#define EQ4_1800Hz		  0X00
#define EQ4_2400Hz		  0X01
#define EQ4_3200Hz		  0X02
#define EQ4_4100Hz		  0X03

#define EQ5_5300Hz		  0X00
#define EQ5_6900Hz		  0X01
#define EQ5_9000Hz		  0X02
#define EQ5_11700Hz		  0X03

/*芯片ID*/
#define CHIP_ID_1       1
#define CHIP_ID_2       2

/*通道*/
#define R_CHANNEL	      1
#define L_CHANNEL       2
#if USE_FREERTOS
#define WM8978_DELAY_MS(x)  osDelay(x)    /**< WM8978 ms级延时*/
#define WM8978_DELAY_US(x)  delay_xus(x)  /**< WM8978 us级延时*/
#else
#define WM8978_DELAY_MS(x)  HAL_Delay(x)    /**< WM8978 ms级延时*/
#define WM8978_DELAY_US(x)  delay_xus(x)    /**< WM8978 us级延时*/
#endif

/*16BIT位*/
#define REG_BIT0                (1U<<0)
#define REG_BIT1                (1U<<1)
#define REG_BIT2                (1U<<2)
#define REG_BIT3                (1U<<3)
#define REG_BIT4                (1U<<4)
#define REG_BIT5                (1U<<5)
#define REG_BIT6                (1U<<6)
#define REG_BIT7                (1U<<7)
#define REG_BIT8                (1U<<8)
#define REG_BIT9                (1U<<9)
#define REG_BIT10               (1U<<10)
#define REG_BIT11               (1U<<11)
#define REG_BIT12               (1U<<12)
#define REG_BIT13               (1U<<13)
#define REG_BIT14               (1U<<14)
#define REG_BIT15               (1U<<15)

/*WM8978 I2C句柄*/
#define WM8978HANDLE     &hi2c1
/** Private constants --------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
/*WM8978 0～57寄存器的默认值*/
static uint16_t RegValTab[58]=
{
	0X0000, 0X0000, 0X0000, 0X0000, 0X0050, 0X0000, 0X0140, 0X0000,
	0X0000, 0X0000, 0X0000, 0X00FF, 0X00FF, 0X0000, 0X0100, 0X00FF,
	0X00FF, 0X0000, 0X012C, 0X002C, 0X002C, 0X002C, 0X002C, 0X0000,
	0X0032, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
	0X0038, 0X000B, 0X0032, 0X0000, 0X0008, 0X000C, 0X0093, 0X00E9,
	0X0000, 0X0000, 0X0000, 0X0000, 0X0003, 0X0010, 0X0010, 0X0100,
	0X0100, 0X0002, 0X0001, 0X0001, 0X0039, 0X0039, 0X0039, 0X0039,
	0X0001, 0X0001
};

/*DAC端口AGC参数*/
static WM8978_AGC_PAR_Typedef_t WM8978_AGC_Par =
{
  .atk = LIMATK_US_375,
  .dcy = LIMDCY_MS_6,
  .boost = 0,
  .level_l = 0
};
/** Private function prototypes ----------------------------------------------*/
static uint8_t WM8978_Write_Reg(uint8_t chipid, uint8_t reg, uint16_t val);

static uint8_t WM8978_Init(uint8_t chipid);
static uint8_t WM8978_PwrOn(uint8_t chipid);
static uint8_t WM8978_InitSingleEndInputPath(uint8_t chipid);
static uint8_t WM8978_SetInPGAVolume(uint8_t chipid, uint8_t chid, float ndB);
static uint8_t WM8978_InitADC(uint8_t chipid);
static uint8_t WM8978_SetADCDigVolume(uint8_t chipid, uint8_t chid, float ndB);
static uint8_t WM8978_InitInputALC(uint8_t chipid);
static uint8_t WM8978_InitDACOutputSignalPath(uint8_t chipid);
static uint8_t WM8978_SetDACDigVolume(uint8_t chipid, uint8_t chid, float ndB);
static uint8_t WM8978_SetOutPGAVolume_CH1(uint8_t chipid, uint8_t chid, float ndB);
static uint8_t WM8978_SetOutPGAVolume_CH2(uint8_t chipid, uint8_t chid, float ndB);
static uint8_t WM8978_InitOutputALC(uint8_t chipid);
static uint8_t WM8978_InitAudioInterface(uint8_t chipid);
static uint8_t WM8978_InitEQ(uint8_t chipid);
/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   写寄存器
  * @param   [in]ChipID 芯片ID
  * @param   [in]reg 寄存器地址
  * @param   [in]val 写入数值
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_Write_Reg(uint8_t chipid, uint8_t reg, uint16_t val)
{
	uint8_t temp[2] = {0};
	temp[0]= (uint8_t)((reg << 1)|((val >> 8)& 0x01));//H8  reg+data_B8
	temp[1]= (uint8_t)(val & 0x00FF);//L8 dataB7~B0

	WM8978_DELAY_US(100);
	if(chipid == CHIP_ID_1)
	{
		if(HAL_I2C_Master_Transmit(WM8978HANDLE, WM8978_ADDR_W, temp, 2, 100) != HAL_OK)
    {
      printf("WM8978 I2C 1 Write Error.\r\n");
    }
	}
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 初始化
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_Init(uint8_t chipid)
{
	WM8978_PwrOn(chipid);
//	WM8978_InitSingleEndInputPath(chipid);
//	WM8978_InitADC(chipid);
	WM8978_InitDACOutputSignalPath(chipid);
	WM8978_InitAudioInterface(chipid);
	WM8978_InitEQ(chipid);
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 开启
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_PwrOn(uint8_t chipid)
{
	//1. wait for supply voltage to settle
	WM8978_DELAY_MS(100);
	//2. mute all analogue output
	RegValTab[0] |= 0x0000;
	WM8978_Write_Reg(chipid, 0, RegValTab[0]);	//software reset
	WM8978_DELAY_MS(100);
	//3. Set L/RMIXEN =1 and DACENL/R =1 in R3
	RegValTab[3] |= 0x000F;
	WM8978_Write_Reg(chipid, 3, RegValTab[3]);
	//4. Set BUFIOEN =1 and VMIDSEL[1:0] to required value in register R1, wait for VMID to settle
	RegValTab[1] |= 0x0005;
	WM8978_Write_Reg(chipid, 1, RegValTab[1]);
	WM8978_DELAY_MS(10);
	//5. Set BIASEN=1 in R1;
	RegValTab[1] |= 0x0008;
	WM8978_Write_Reg(chipid, 1, RegValTab[1]);
	//6. set L/ROUT1EN = 1 in R2;
	RegValTab[2] |= 0x0180;
	WM8978_Write_Reg(chipid, 2, RegValTab[2]);
  /* 关闭LOUT2/ROUT2 */
#if ENABLE_LR_OUT2
    //关闭
//  RegValTab[3] &= ~REG_BIT5;
//  RegValTab[3] &= ~REG_BIT6;
  //打开
  RegValTab[3] |= REG_BIT5;
  RegValTab[3] |= REG_BIT6;
  WM8978_Write_Reg(chipid, 3, RegValTab[3]);
#endif
	//7. Enable other Mixers as required;
	//8. Enable other Output as required;
	//9. Set remaining regs
	//RegValTab[49] |= 0x0000;
	//WM8978_Write_Reg(chipid, 49, RegValTab[49]);

	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 初始化单端输入L2、R2的信号路径
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
UNUSEDX static uint8_t WM8978_InitSingleEndInputPath(uint8_t chipid)
{
	//1. enable input PGAs in R2
	RegValTab[2] |= 0x000C;
	WM8978_Write_Reg(chipid, 2, RegValTab[2]);
	//2. set input path of PGAs in R44
	RegValTab[44] |= 0x0033; //L2 R2
	WM8978_Write_Reg(chipid, 44, RegValTab[44]);
	//3. set PGAs volume in R45,R46,[B5:B0]
	WM8978_SetInPGAVolume(chipid, R_CHANNEL, 0);
	WM8978_SetInPGAVolume(chipid, L_CHANNEL, 0);
	//4. enable input boost in R2
	RegValTab[2] |= 0x0030;
	WM8978_Write_Reg(chipid, 2, RegValTab[2]);
	//5. set input Boost for PGA out 0dB in R47,R48
	RegValTab[47] |= 0x0000;
	WM8978_Write_Reg(chipid, 47, RegValTab[47]);
	RegValTab[48] |= 0x0000;
	WM8978_Write_Reg(chipid, 48, RegValTab[48]);
	//6. enable micphone bias in R1
	RegValTab[1] |= (1<<4);
	WM8978_Write_Reg(chipid, 1, RegValTab[1]);
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 设置输入PGA的增益 -12dB ~ 35.25dB,step 0.75dB ,set PGAs volume in R45&R46,[B5:B0]
  * @brief   C1RIN-CH1 C1LIN-CH2 C2RIN-CH3 C2LIN-CH4
  * @param   [in]ChipID 芯片ID
  * @param   [in]chid 声道
  * @param   [in]ndB 分贝
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_SetInPGAVolume(uint8_t chipid, uint8_t chid, float ndB)
{
	uint16_t Volval = 0;
	Volval = (uint16_t)((ndB+12.f)/0.75f);

	if(chid == R_CHANNEL)//R_CHANNEL -> R46
	{
		RegValTab[46] = (1<<7)|(Volval&0x003F)|(1<<8);
		WM8978_Write_Reg(chipid, 46, RegValTab[46]);
	}
	else if(chid == L_CHANNEL) //L_CHANNEL -> R45
	{
		RegValTab[45] = (1<<7)|(Volval&0x003F)|(1<<8);
		WM8978_Write_Reg(chipid, 45, RegValTab[45]);
	}
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 初始化ADC
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
UNUSEDX static uint8_t WM8978_InitADC(uint8_t chipid)
{
	//1. enable ADCs in R2;
	RegValTab[2] |= 0x0003;
	WM8978_Write_Reg(chipid, 2, RegValTab[2]);
	//2. set ADC high pass fliter and polarity, oversamplerate in R14
	RegValTab[14] = 0x0104; //128 oversample
	WM8978_Write_Reg(chipid, 14, RegValTab[14]);
	//3. set notch filter in R27,28,29,30
	RegValTab[27] = 0x0000;
	WM8978_Write_Reg(chipid, 27, RegValTab[27]);
	//4. set ADC digital volume in R15,R16
	WM8978_SetADCDigVolume(chipid, R_CHANNEL, 0);
	WM8978_SetADCDigVolume(chipid, L_CHANNEL, 0);
	//5. disable ALC
	RegValTab[32] = 0x0038;
	WM8978_Write_Reg(chipid, 32, RegValTab[32]);
	//5. LOOPBACK
	//RegValTab[5] = 0x0001;
	//WM8978_Write_Reg(chipid, 5, RegValTab[5]);
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 设置ADC数字增益 mute,-127dB~0dB,0.5dB step
  * @param   [in]ChipID 芯片ID
  * @param   [in]chid 声道
  * @param   [in]ndB 分贝
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_SetADCDigVolume(uint8_t chipid, uint8_t chid, float ndB)
{
	uint16_t Volval = (uint16_t)((127.5f + ndB) / 0.5f);

	if(chid == R_CHANNEL)
	{
		RegValTab[16] = (Volval&0x00FF)|(1<<8);
		WM8978_Write_Reg(chipid, 16, RegValTab[16]);
	}
	else if(chid == L_CHANNEL)
	{
		RegValTab[15] = (Volval&0x00FF)|(1<<8);
		WM8978_Write_Reg(chipid, 15, RegValTab[15]);
	}
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 配置输入ALC ALC limiter mode init sequence in P49 of datasheet
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
UNUSEDX static uint8_t WM8978_InitInputALC(uint8_t chipid)
{
  UNUSED(chipid);
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 初始化DAC信号输出路径
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_InitDACOutputSignalPath(uint8_t chipid)
{
	//1. enable DAC in R3
	RegValTab[3] |= 0x0003;
	WM8978_Write_Reg(chipid, 3, RegValTab[3]);
	//2. set DAC polarity and oversample rate in R10;
  /*开启自动静音 - 禁用易出现哒哒声*/
  //RegValTab[10] |= REG_BIT2;
  /*设置128倍过采样*/
	RegValTab[10] |= REG_BIT3;
	WM8978_Write_Reg(chipid, 10, RegValTab[10]);
	//3. set DAC Digtal volume in R11 R12
	WM8978_SetDACDigVolume(chipid, R_CHANNEL, 0);
	WM8978_SetDACDigVolume(chipid, L_CHANNEL, 0);

	//4. set output mixer in R49,50,51,3  DAC to L1 R1
	RegValTab[50] |= 0x0001;
	WM8978_Write_Reg(chipid, 50, RegValTab[50]);
	RegValTab[51] |= 0x0001;
	WM8978_Write_Reg(chipid, 51, RegValTab[51]);

	//5. set output PGA volume in R52 R53
	WM8978_SetOutPGAVolume_CH1(chipid, R_CHANNEL, 0);
	WM8978_SetOutPGAVolume_CH1(chipid, L_CHANNEL, 0);

	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 设置DAC数字增益 mute,-127dB~0dB,0.5dB step
  * @param   [in]ChipID 芯片ID
  * @param   [in]chid 声道
  * @param   [in]ndB 分贝
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_SetDACDigVolume(uint8_t chipid, uint8_t chid, float ndB)
{
	uint16_t Volval = (uint16_t)((127.5f + ndB) / 0.5f);

	if(chid == R_CHANNEL)//R_CHANNEL -> R12
	{
		RegValTab[12] = (Volval&0x00FF)|(1<<8);
		WM8978_Write_Reg(chipid, 12, RegValTab[12]);
	}
	else if(chid == L_CHANNEL) //L_CHANNEL -> R11
	{
		RegValTab[11] = (Volval&0x00FF)|(1<<8);
		WM8978_Write_Reg(chipid, 11, RegValTab[11]);
	}
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 设置输出PGA增益 -57dB ~ 6dB,step 1dB,set PGAs volume in R52&R53,[B5:B0]
  * @brief   C1RIN-CH1 C1LIN-CH2 C2RIN-CH3 C2LIN-CH4
  * @param   [in]ChipID 芯片ID
  * @param   [in]chid 声道
  * @param   [in]ndB 分贝
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_SetOutPGAVolume_CH1(uint8_t chipid, uint8_t chid, float ndB)
{
	uint16_t Volval = 0;
	Volval = (uint16_t)(ndB + 57.f);

	if(chid == R_CHANNEL)//R_CHANNEL -> R53
	{
		RegValTab[53] = (1<<7)|(Volval&0x003F)|(1<<8);
		WM8978_Write_Reg(chipid, 53, RegValTab[53]);
	}
	else if(chid == L_CHANNEL) //L_CHANNEL -> R52
	{
		RegValTab[52] = (1<<7)|(Volval&0x003F)|(1<<8);
		WM8978_Write_Reg(chipid, 52, RegValTab[52]);
	}
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 设置LOUT2 and ROUT2 volumes输出PGA增益 -57dB ~ 6dB,step 1dB,set PGAs volume in R54&R55,[B5:B0]
  * @brief   C1RIN-CH1 C1LIN-CH2 C2RIN-CH3 C2LIN-CH4
  * @param   [in]ChipID 芯片ID
  * @param   [in]chid 声道
  * @param   [in]ndB 分贝
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
UNUSEDX static uint8_t WM8978_SetOutPGAVolume_CH2(uint8_t chipid, uint8_t chid, float ndB)
{
  uint16_t Volval = (uint16_t)(ndB + 57.f);

  if(chid == R_CHANNEL)//R_CHANNEL -> R53
  {
    RegValTab[55] = (1<<7)|(Volval&0x003F)|(1<<8);
    WM8978_Write_Reg(chipid, 55, RegValTab[53]);
  }
  else if(chid == L_CHANNEL) //L_CHANNEL -> R52
  {
    RegValTab[54] = (1<<7)|(Volval&0x003F)|(1<<8);
    WM8978_Write_Reg(chipid, 54, RegValTab[52]);
  }
  return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 配置输出ALC
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
UNUSEDX static uint8_t WM8978_InitOutputALC(uint8_t chipid)
{
	UNUSED(chipid);
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 配置数字音频接口
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_InitAudioInterface(uint8_t chipid)
{
	//1. interface in R4
  RegValTab[4] = 0;
#if USE_24BIT_DATA_FORMAT
  /*24Bit*/
	RegValTab[4] = RegValTab[4] | (REG_BIT6);
  RegValTab[4] = RegValTab[4] & (~REG_BIT5);
  /*I2S*/
	RegValTab[4] = RegValTab[4] | (REG_BIT4);
  RegValTab[4] = RegValTab[4] & (~REG_BIT3);
#else
  /*16Bit*/
	RegValTab[4] = RegValTab[4] & (~REG_BIT6);
  RegValTab[4] = RegValTab[4] & (~REG_BIT5);
  /*I2S*/
	RegValTab[4] = RegValTab[4] | (REG_BIT4);
  RegValTab[4] = RegValTab[4] & (~REG_BIT3);
#endif
	WM8978_Write_Reg(chipid, 4, RegValTab[4]);

	/* 2. clk in R6 */
	RegValTab[6] = 0x0000;  //salve mode,MCLK=SYSCLK=256Fs, no use PLL
	WM8978_Write_Reg(chipid, 6, RegValTab[6]);
	/* 3. set approximate fs for digtal filters in R7 */
	RegValTab[7] = 0x0006;
	WM8978_Write_Reg(chipid, 7, RegValTab[7]); //~16kHz
	/* 4. pll off */
	RegValTab[1] |= 0x0000;
	WM8978_Write_Reg(chipid, 1, RegValTab[1]); //~16kHz
	return 0;
}

/**
  ******************************************************************
  * @brief   WM8978 初始化音效均衡器
  * @param   [in]ChipID 芯片ID
  * @return  0 正常
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
static uint8_t WM8978_InitEQ(uint8_t chipid)
{
	RegValTab[18] = 0x002C;
	WM8978_Write_Reg(chipid, 18, RegValTab[18]);
	RegValTab[19] = 0x002C;
	WM8978_Write_Reg(chipid, 19, RegValTab[19]);
	RegValTab[20] = 0x002C;
	WM8978_Write_Reg(chipid, 20, RegValTab[20]);
	RegValTab[21] = 0x002C;
	WM8978_Write_Reg(chipid, 21, RegValTab[21]);
	RegValTab[22] = 0x002C;
	WM8978_Write_Reg(chipid, 22, RegValTab[22]);
	return 0 ;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
 * @brief 设置WM8978输出1使能状态
 *
 * @param State true打开
 */
void WM8978_Set_LR_OUT1_On_Off(bool State)
{
  if(State)
  {
  /* 开启LOUT1/ROUT1 */
    RegValTab[2] |= REG_BIT7;
    RegValTab[2] |= REG_BIT8;
  }
  else
  {
    RegValTab[2] &= ~REG_BIT7;
    RegValTab[2] &= ~REG_BIT8;
  }
  WM8978_Write_Reg(CHIP_ID_1, 2, RegValTab[2]);
  WM8978_Write_Reg(CHIP_ID_2, 2, RegValTab[2]);
}

/**
 * @brief 设置WM8978输出2使能状态
 *
 * @param State true打开
 */
void WM8978_Set_LR_OUT2_On_Off(bool State)
{
#if ENABLE_LR_OUT2
  if(State)
  {
    /* 开启LOUT2/ROUT2 */
    RegValTab[3] |= REG_BIT5;
    RegValTab[3] |= REG_BIT6;
  }
  else
  {
    RegValTab[3] &= ~REG_BIT5;
    RegValTab[3] &= ~REG_BIT6;
  }
  WM8978_Write_Reg(CHIP_ID_1, 3, RegValTab[3]);
  WM8978_Write_Reg(CHIP_ID_2, 3, RegValTab[3]);
#endif
}

/**
  ******************************************************************
  * @brief   WM8978 设置DAC AGC参数
  * @param   [in]AGC_Par
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-08-03
  ******************************************************************
  */
void WM8978_Set_AGC_Gain(WM8978_AGC_PAR_Typedef_t *AGC_Par)
{
  memmove(&WM8978_AGC_Par, AGC_Par, sizeof(WM8978_AGC_PAR_Typedef_t));
  /*设置ATK*/
  uint16_t temp = (uint16_t)WM8978_AGC_Par.atk;
  RegValTab[24] = temp;
  /*设置DCY*/
  temp = (uint16_t)WM8978_AGC_Par.dcy;
  RegValTab[24] |= (temp << 7);
  /*使能*/
  RegValTab[24] |= (REG_BIT8);

  /*换算百分比*/
  /*boot 12dB == 100%*/
  uint16_t dB = (uint16_t)((float)WM8978_AGC_Par.boost * 12.f / 100.f);
  RegValTab[25] = dB;
  /*-6dB == 100%*/
  dB = (uint16_t)((float)WM8978_AGC_Par.level_l * 6.f / 100.f);
  RegValTab[25] |= dB << 6;

  WM8978_Write_Reg(CHIP_ID_1, 24, RegValTab[24]);
  WM8978_Write_Reg(CHIP_ID_2, 24, RegValTab[24]);

  WM8978_Write_Reg(CHIP_ID_1, 25, RegValTab[25]);
  WM8978_Write_Reg(CHIP_ID_2, 25, RegValTab[25]);
}

/**
  ******************************************************************
  * @brief   WM8978 设置软件静音
  * @param   [in]Enable true使能静音
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-08-03
  ******************************************************************
  */
void WM8978_Set_Soft_Mute(bool Enable)
{
  if(Enable == true)
  {
    RegValTab[10] |= (REG_BIT6);
  }
  else
  {
    RegValTab[10] &= ~(REG_BIT6);
  }
  WM8978_Write_Reg(CHIP_ID_1, 10, RegValTab[10]);
  WM8978_Write_Reg(CHIP_ID_2, 10, RegValTab[10]);
}

/**
  ******************************************************************
  * @brief   WM8978 设置前置放大
  * @param   [in]Channel 通道号
  * @param   [in]Gain 增益 -12dB - +35.25dB，Step 0.75
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
void WM8978_Set_Preamp_Gain(CHANNEL_NUM_Typedef_t Channel, float Gain)
{
  float true_gain = ((Gain-1.f) / 100.f) * 35.25f;

  WM8978_SetInPGAVolume(CHIP_ID_1, L_CHANNEL, true_gain);
  WM8978_SetInPGAVolume(CHIP_ID_2, L_CHANNEL, true_gain);

  WM8978_SetInPGAVolume(CHIP_ID_1, R_CHANNEL, true_gain);
  WM8978_SetInPGAVolume(CHIP_ID_2, R_CHANNEL, true_gain);
}

/**
  ******************************************************************
  * @brief   WM8978 设置左右耳增益
  * @param   [in]Channel 通道号
  * @param   [in]Gain 增益
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
void WM8978_Set_Vol_Gain(CHANNEL_NUM_Typedef_t Channel, float Gain)
{
  float true_gain = (Gain / 100.f) * 63.f;
  if(true_gain < 57.f)
  {
    true_gain = (57.f - true_gain)* -1.f;
  }
  else
  {
    true_gain -= 57.f;
  }

  if(Channel == LEFT_CHANNEL)
  {
    WM8978_SetOutPGAVolume_CH1(CHIP_ID_1, L_CHANNEL, true_gain);
    WM8978_SetOutPGAVolume_CH1(CHIP_ID_2, L_CHANNEL, true_gain);
  }
  else if(Channel == RIGHT_CHANNEL)
  {
    WM8978_SetOutPGAVolume_CH1(CHIP_ID_1, R_CHANNEL, true_gain);
    WM8978_SetOutPGAVolume_CH1(CHIP_ID_2, R_CHANNEL, true_gain);
  }

//  float true_gain = ((Gain / 100.f) * 127.5f) - 127.5f;

//  if(Channel == LEFT_CHANNEL)
//  {
//    WM8978_SetDACDigVolume(CHIP_ID_1, L_CHANNEL, true_gain);
//    WM8978_SetDACDigVolume(CHIP_ID_2, L_CHANNEL, true_gain);
//  }
//  else if(Channel == RIGHT_CHANNEL)
//  {
//    WM8978_SetDACDigVolume(CHIP_ID_1, R_CHANNEL, true_gain);
//    WM8978_SetDACDigVolume(CHIP_ID_2, R_CHANNEL, true_gain);
//  }
}

/**
  ******************************************************************
  * @brief   WM8978 设置EQ 5波段中心频率及增益
  * @param   [in]fq 中心频率
  * @param   [in]Gain_dB 波段增益
  * @param   [in]Sig_Path 信号路径，为0x100时使用DAC EQ，为0时使用ADC EQ
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-26
  ******************************************************************
  */
void WM8978_Set_EQ_Band_Gain(const uint16_t *fq, const uint8_t *Gain_dB, uint16_t Sig_Path)
{
  /*EQ波段0*/
  switch(fq[0])
  {
    case 80:
      RegValTab[18] = 0;
      break;
    case 105:
      RegValTab[18] = 1;
      RegValTab[18] <<= 5;
      break;
    case 135:
      RegValTab[18] = 2;
      RegValTab[18] <<= 5;
      break;
    case 175:
      RegValTab[18] = 3;
      RegValTab[18] <<= 5;
      break;
  }

  /*EQ波段1*/
  switch(fq[1])
  {
    case 230:
      RegValTab[19] = 0;
      break;
    case 300:
      RegValTab[19] = 1;
      RegValTab[19] <<= 5;
      break;
    case 385:
      RegValTab[19] = 2;
      RegValTab[19] <<= 5;
      break;
    case 500:
      RegValTab[19] = 3;
      RegValTab[19] <<= 5;
      break;
  }
  // RegValTab[19] |= 0x0100;/**< 宽带*/

  /*EQ波段2*/
  switch(fq[2])
  {
    case 650:
      RegValTab[20] = 0;
      break;
    case 850:
      RegValTab[20] = 1;
      RegValTab[20] <<= 5;
      break;
    case 1100:
      RegValTab[20] = 2;
      RegValTab[20] <<= 5;
      break;
    case 1400:
      RegValTab[20] = 3;
      RegValTab[20] <<= 5;
      break;
  }
  // RegValTab[20] |= 0x0100;/**< 宽带*/

  /*EQ波段3*/
  switch(fq[3])
  {
    case 1800:
      RegValTab[21] = 0;
      break;
    case 2400:
      RegValTab[21] = 1;
      RegValTab[21] <<= 5;
      break;
    case 3200:
      RegValTab[21] = 2;
      RegValTab[21] <<= 5;
      break;
    case 4100:
      RegValTab[21] = 3;
      RegValTab[21] <<= 5;
      break;
  }
  // RegValTab[21] |= 0x0100;/**< 宽带*/

  /*EQ波段4*/
  switch(fq[4])
  {
    case 5300:
      RegValTab[22] = 0;
      break;
    case 6900:
      RegValTab[22] = 1;
      RegValTab[22] <<= 5;
      break;
    case 9000:
      RegValTab[22] = 2;
      RegValTab[22] <<= 5;
      break;
    case 11700:
      RegValTab[22] = 3;
      RegValTab[22] <<= 5;
      break;
  }

  /*波段增益值*/
  RegValTab[18] |= Gain_dB[0];
  RegValTab[19] |= Gain_dB[1];
  RegValTab[20] |= Gain_dB[2];
  RegValTab[21] |= Gain_dB[3];
  RegValTab[22] |= Gain_dB[4];

  /* 设置信号路径 */
  RegValTab[18] |= Sig_Path;
  /* bit8：0设置窄带，1设置宽带 */
  RegValTab[19] &= ~REG_BIT8;
  RegValTab[20] &= ~REG_BIT8;
  RegValTab[21] &= ~REG_BIT8;

  WM8978_Write_Reg(CHIP_ID_1, 18, RegValTab[18]);
  WM8978_Write_Reg(CHIP_ID_1, 19, RegValTab[19]);
  WM8978_Write_Reg(CHIP_ID_1, 20, RegValTab[20]);
  WM8978_Write_Reg(CHIP_ID_1, 21, RegValTab[21]);
  WM8978_Write_Reg(CHIP_ID_1, 22, RegValTab[22]);

  WM8978_Write_Reg(CHIP_ID_2, 18, RegValTab[18]);
  WM8978_Write_Reg(CHIP_ID_2, 19, RegValTab[19]);
  WM8978_Write_Reg(CHIP_ID_2, 20, RegValTab[20]);
  WM8978_Write_Reg(CHIP_ID_2, 21, RegValTab[21]);
  WM8978_Write_Reg(CHIP_ID_2, 22, RegValTab[22]);
}

/**
  ******************************************************************
  * @brief   WM8978 设置高通滤波器
  * @param   [in]High_Pass_Hz 截至频率
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-23
  ******************************************************************
  */
void WM8978_Set_HIPASS(ADC_HIGH_PASS_FILTER_Typedef_t High_Pass_Hz)
{
  if(ADC_HI_PASS_16KHZ_OFF == High_Pass_Hz)
  {
    RegValTab[14] &= ~REG_BIT8;
  }
  else
  {
    /*设置左右通道不反转极性，128倍过采样，使能高通滤波器，配置高通滤波器截至频率*/
    RegValTab[14] = 0x0184 | ((uint16_t)(High_Pass_Hz << 4));
  }

  WM8978_Write_Reg(CHIP_ID_1, 14, RegValTab[14]);
  WM8978_Write_Reg(CHIP_ID_2, 14, RegValTab[14]);
}

/**
  ******************************************************************
  * @brief   WM8978 设置采样率
  * @param   [in]Sample_Rate 采样率
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-08
  ******************************************************************
  */
void WM8978_Set_SampRate(WM8978_SAMP_RATE_Typedef_t Sample_Rate)
{
  RegValTab[7] = 0;
  switch(Sample_Rate)
  {
    case SAMP_RATE_8KHZ:
      RegValTab[7] = RegValTab[7] | (REG_BIT3);
      RegValTab[7] = RegValTab[7] & (~REG_BIT2);
      RegValTab[7] = RegValTab[7] | (REG_BIT1);
      break;
    case SAMP_RATE_12KHZ:
      RegValTab[7] = RegValTab[7] | (REG_BIT3);
      RegValTab[7] = RegValTab[7] & (~REG_BIT2);
      RegValTab[7] = RegValTab[7] & (~REG_BIT1);
      break;
    case SAMP_RATE_16KHZ:
      RegValTab[7] = RegValTab[7] & (~REG_BIT3);
      RegValTab[7] = RegValTab[7] | (REG_BIT2);
      RegValTab[7] = RegValTab[7] | (REG_BIT1);
      break;
    case SAMP_RATE_24KHZ:
      RegValTab[7] = RegValTab[7] & (~REG_BIT3);
      RegValTab[7] = RegValTab[7] | (REG_BIT2);
      RegValTab[7] = RegValTab[7] & (~REG_BIT1);
      break;
    case SAMP_RATE_32KHZ:
      RegValTab[7] = RegValTab[7] & (~REG_BIT3);
      RegValTab[7] = RegValTab[7] & (~REG_BIT2);
      RegValTab[7] = RegValTab[7] | (REG_BIT1);
      break;
    case SAMP_RATE_48KHZ:
      RegValTab[7] = RegValTab[7] & (~REG_BIT3);
      RegValTab[7] = RegValTab[7] & (~REG_BIT2);
      RegValTab[7] = RegValTab[7] & (~REG_BIT1);
      break;
    default:
      RegValTab[7] = RegValTab[7] & (~REG_BIT3);
      RegValTab[7] = RegValTab[7] | (REG_BIT2);
      RegValTab[7] = RegValTab[7] | (REG_BIT1);
      break;
  }

  /*设置采样率*/
  WM8978_Write_Reg(CHIP_ID_1, 7, RegValTab[7]);
  WM8978_Write_Reg(CHIP_ID_2, 7, RegValTab[7]);
}

/**
  ******************************************************************
  * @brief   WM8978 关闭
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-21
  ******************************************************************
  */
void WM8978_DeInit(void)
{
  /*关闭 混响 偏置 输入输出缓冲*/
  RegValTab[1] = 0;
  WM8978_Write_Reg(CHIP_ID_1, 1, RegValTab[1]);
  WM8978_Write_Reg(CHIP_ID_2, 1, RegValTab[1]);

  /*关闭LR 输出1*/
  RegValTab[2] = 0;
  WM8978_Write_Reg(CHIP_ID_1, 2, RegValTab[2]);
  WM8978_Write_Reg(CHIP_ID_2, 2, RegValTab[2]);

  /*关闭LR MIX电源/LR DAC电源*/
  RegValTab[3] = 0;
  WM8978_Write_Reg(CHIP_ID_1, 3, RegValTab[3]);
  WM8978_Write_Reg(CHIP_ID_2, 3, RegValTab[3]);

  /*进入软复位状态*/
  RegValTab[0] = 0xFFFF;
  WM8978_Write_Reg(CHIP_ID_1, 0, RegValTab[0]);
  WM8978_Write_Reg(CHIP_ID_2, 0, RegValTab[0]);

  /*关闭DAC VDD*/
  HAL_GPIO_WritePin(VCC_DAC_3_3V_EN_GPIO_Port, VCC_DAC_3_3V_EN_Pin, GPIO_PIN_RESET);
}

/**
  ******************************************************************
  * @brief   WM8978 配置
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-01
  ******************************************************************
  */
void WM8978_Init_Start(void)
{
  /*开启DAC VDD*/
  HAL_GPIO_WritePin(VCC_DAC_3_3V_EN_GPIO_Port, VCC_DAC_3_3V_EN_Pin, GPIO_PIN_SET);

  WM8978_Init(CHIP_ID_1);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
