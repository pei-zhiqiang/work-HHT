#include "chsc6x_platform.h"
#include "main.h"
#include "i2c.h"
#include "string.h"

#define tl_delay   HAL_Delay

/*????*/
#define TP_SLAVE_ADDR     0x2E/**< ???7Bit??*/

#define TP_SLAVE_ADDR_W   (TP_SLAVE_ADDR<<1)
#define TP_SLAVE_ADDR_R   ((TP_SLAVE_ADDR<<1)|0x01)

extern I2C_HandleTypeDef hi2c3;
#define TP_I2C_HANDLE     &hi2c3  /**< ???I2C?? */

static uint8_t I2C_Send_Buf[64];


/* return: =read lenth succeed; <0 failed 
   read reg addr not need 
   just used for reading xy cord info*/
int chsc6x_i2c_read(unsigned char id, unsigned char *p_data, unsigned short lenth)
{    
   // printf("chsc6x_i2c_read\r\n");
    
  
    while(HAL_I2C_GetState(TP_I2C_HANDLE) != HAL_I2C_STATE_READY);
    HAL_StatusTypeDef  State = HAL_I2C_Master_Receive(TP_I2C_HANDLE, TP_SLAVE_ADDR_R, p_data, lenth, 5000);
    if (State != HAL_OK)
    {
      return -1;
    }
    else
    {
      return lenth;
    }
  
    //return  lenth;
}

/* RETURN:0->pass else->fail */
int chsc6x_read_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
    //printf("i2c read_bytes id = 0x%02X Addr = 0x%04X I2C Adr = 0x%04X.\n", id, adr, TP_SLAVE_ADDR_R);
    
    /* ??????? */
    while(HAL_I2C_GetState(TP_I2C_HANDLE) != HAL_I2C_STATE_READY);//??????
    /* ??REG?? */
    I2C_Send_Buf[0] = (uint8_t)(adr >> 8);
    I2C_Send_Buf[1] = (uint8_t)adr;
    HAL_StatusTypeDef State = HAL_I2C_Master_Transmit(TP_I2C_HANDLE, TP_SLAVE_ADDR_W, I2C_Send_Buf, 2, 5000);
    if(State != HAL_OK)
    {
      return -1;
    }
    
    /* ???????? */
    while(HAL_I2C_GetState(TP_I2C_HANDLE) != HAL_I2C_STATE_READY);//??????
    State = HAL_I2C_Master_Receive(TP_I2C_HANDLE, TP_SLAVE_ADDR_R, rxbuf, lenth, 5000);
  
    if(State != HAL_OK) 
    {
      return -1;
    }
    
    return 0;
}

/* RETURN:0->pass else->fail */
int chsc6x_write_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *txbuf, unsigned short lenth)
{
   //printf("i2c write_bytes id = 0x%02X Addr = 0x%04X I2C Adr = 0x%04X.\n", id, adr, TP_SLAVE_ADDR_W);
    
    while(HAL_I2C_GetState(TP_I2C_HANDLE) != HAL_I2C_STATE_READY);//??????
    
    /* ??REG?? */
    I2C_Send_Buf[0] = (uint8_t)(adr >> 8);
    I2C_Send_Buf[1] = (uint8_t)adr;
    memcpy(I2C_Send_Buf + 2, txbuf, lenth);
    
    HAL_StatusTypeDef State = HAL_I2C_Master_Transmit(TP_I2C_HANDLE, TP_SLAVE_ADDR_W, I2C_Send_Buf, lenth + 2, 5000);
    
    if(State != HAL_OK)
    {
      return -1;
    }
    return 0;
}

void chsc6x_msleep(int ms)
{
   HAL_Delay(ms);
  
//    tl_delay(32*ms);   
}

void chsc6x_tp_reset(void)
{
    TP_RESET_CLR;
    HAL_Delay(30);
    TP_RESET_SET;
    HAL_Delay(30);
  
//    pd7_out0();
//    tl_delay(950);//30ms
//    pd7_out1();
//    tl_delay(950);//30ms
}

void chsc6x_tp_reset_active(void)
{
    TP_RESET_CLR;
    HAL_Delay(2);
    TP_RESET_SET;
  
//    pd7_out0();
//    tl_delay(950);//30ms
//    pd7_out1();
}

