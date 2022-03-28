/**
 * @file lv_port_disp.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "LCD_Port.h"
#include "gpio.h"
#include "spi.h"
/*********************
 *      DEFINES
 *********************/
#define LCD_FRAMEBUF_SIZE		(LV_HOR_RES_MAX * 180)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
__attribute__ ((section("USE_LCD_DMA_BUF_SPACE1"))) lv_color_t buf1_1[LCD_FRAMEBUF_SIZE];                      /*A buffer for 10 rows*/
__attribute__ ((section("USE_LCD_DMA_BUF_SPACE2"))) lv_color_t buf1_2[LCD_FRAMEBUF_SIZE];                      /*A buffer for 10 rows*/

unsigned int flush_cnt = 0;
unsigned int int_cnt = 0;
		
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_color_t * dest, uint32_t length, lv_color_t color);
#endif


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/***********************
* @brief  lvgl 显示接口初始化
 *
 *
 */
void lv_port_disp_init(void)
{
	static lv_disp_buf_t disp_buf_1;
	/*-------------------------
	* Initialize your display
	* -----------------------*/
	disp_init();  // 函数内部可以用来放LCD的初始化,但没必要

	/*-----------------------------
	* Create a buffer for drawing
	*----------------------------*/
	/* Example for 1) 显示缓冲区初始化 */
	lv_disp_buf_init(&disp_buf_1, buf1_1, buf1_2, LCD_FRAMEBUF_SIZE);   /*Initialize the display buffer*/

	/*-----------------------------------
	* Register the display in LittlevGL
	*----------------------------------*/
  /* 显示驱动默认值初始化 */
	lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/ // 定义了一个显示驱动
	lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	/* 设置屏幕的显示大小 不设置的话，默认值在lv_conf.h LV_HOR_RES_MAX 和 LV_VER_RES_MAX 宏定义的值 */
	//disp_drv.hor_res = 360;//lcddev.width;
	//disp_drv.ver_res = 360;//lcddev.height;
  disp_drv.hor_res = 240;
	disp_drv.ver_res = 240;
  

	/*Used to copy the buffer's content to the display*/
  /* 注册显示驱动回调 */
	disp_drv.flush_cb = disp_flush;

	/*Set a display buffer*/
  /* 注册显示缓冲区 */
	disp_drv.buffer = &disp_buf_1;

#if LV_USE_GPU
	/*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

	/*Blend two color array using opacity*/
	disp_drv.gpu_blend = gpu_blend;

	/*Fill a memory array with a color*/
	disp_drv.gpu_fill = gpu_fill;
#endif

	/*Finally register the driver*/
  /* 注册显示驱动到lvgl中 */
	lv_disp_drv_register(&disp_drv);
}

static lv_disp_drv_t * disp_drv_p;


void LCD_SPI_TxCpltCallback()
{
	LCD_CS_SET;
	lv_disp_flush_ready(disp_drv_p);	
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    /*You code here*/
  // lcd_init();  // 可以用来放LCD的初始化，但没必要
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
/* 把指定区域的显示缓冲区内容写入到屏幕上，你可以使用DMA或者其他的硬件加速器
 * 在后台去完成这个操作但是完成之后，必须得调用lv_disp_flush_ready();
 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	  flush_cnt++;
#ifdef LCD_DMA_ENABLE
	  disp_drv_p = disp_drv;
	  LCD_Color_Fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);
#else
	  LCD_Color_Fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);
     /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
     lv_disp_flush_ready(disp_drv);
#endif
}


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill_cb(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color);
{
    /*It's an example code which should be done by your GPU*/
    uint32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y < fill_area->y2; y++) {
        for(x = fill_area->x1; x < fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
