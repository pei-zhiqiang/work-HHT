/**
 *  @file fal_flash_sfud_port.c
 *
 *  @date 2021-05-14
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief GD Flash FAL分区实现
 *
 *  @details 1、提供对FAL组件操作接口
 *  - 页面编程时间：典型值0.6ms
 *
 *  - 扇区擦除时间：典型值50ms
 *
 *  - 块擦除时间：典型值0.15/0.20s
 *
 *  - 芯片擦除时间：典型值25s
 *
 *  - 统一的4K字节扇区
 *
 *  - 统一的32/64K字节块
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#define FAL_USING_SFUD_PORT
#ifdef FAL_USING_SFUD_PORT
#include <fal.h>
#include <sfud.h>
#ifdef RT_USING_SFUD
  #include <spi_flash_sfud.h>
#endif
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#ifndef FAL_USING_NOR_FLASH_DEV0_NAME
  #define FAL_USING_NOR_FLASH_DEV0_NAME             "GD25Q64C0"
#endif

#ifndef FAL_USING_NOR_FLASH_DEV1_NAME
  #define FAL_USING_NOR_FLASH_DEV1_NAME             "GD25Q64C1"
#endif
/** Private constants --------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/
static sfud_flash_t sfud_dev0 = NULL;
static sfud_flash_t sfud_dev1 = NULL;
/** Private function prototypes ----------------------------------------------*/
static int flash0_init(void);
static int flash0_read(long offset, uint8_t *buf, size_t size);
static int flash0_write(long offset, const uint8_t *buf, size_t size);
static int flash0_erase(long offset, size_t size);
static int flash1_init(void);
static int flash1_read(long offset, uint8_t *buf, size_t size);
static int flash1_write(long offset, const uint8_t *buf, size_t size);
static int flash1_erase(long offset, size_t size);
/** Public variables ---------------------------------------------------------*/
struct fal_flash_dev nor_flash0 =
{
    .name       = FAL_USING_NOR_FLASH_DEV0_NAME,
    .addr       = 0,
    .len        = 8 * 1024 * 1024,
    .blk_size   = 4096,
    .ops        = {flash0_init, flash0_read, flash0_write, flash0_erase},
    .write_gran = 1
};

struct fal_flash_dev nor_flash1 =
{
    .name       = FAL_USING_NOR_FLASH_DEV1_NAME,
    .addr       = 0,
    .len        = 8 * 1024 * 1024,
    .blk_size   = 4096,
    .ops        = {flash1_init, flash1_read, flash1_write, flash1_erase},
    .write_gran = 1
};
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
  * @brief   Flash初始化
  * @param   [in]None
  * @return  0成功
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash0_init(void)
{
  sfud_init();

#ifdef RT_USING_SFUD
  /* RT-Thread RTOS platform */
  sfud_dev0 = rt_sfud_flash_find_by_dev_name(FAL_USING_NOR_FLASH_DEV_NAME);
#else
  /* bare metal platform */
  sfud_dev0 = sfud_get_device(SFUD_GD25Q64C_DEVICE0_INDEX);
#endif

  if(NULL == sfud_dev0)
  {
    return -1;
  }

  /* 初始化 */
//  sfud_device_init(sfud_dev0);

  /* update the flash chip information */
  nor_flash0.blk_size = sfud_dev0->chip.erase_gran;
  nor_flash0.len = sfud_dev0->chip.capacity;

  return 0;
}

/**
  ******************************************************************
  * @brief   Flash读取
  * @param   [in]None
  * @return  读取大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash0_read(long offset, uint8_t *buf, size_t size)
{
  assert(sfud_dev0);
  assert(sfud_dev0->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev0, &status);
  // printf("fresh sfud_read Status: %d\r\n", ret);

  if(sfud_read(sfud_dev0, nor_flash0.addr + offset, size, buf) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/**
  ******************************************************************
  * @brief   Flash写入
  * @param   [in]None
  * @return  写入大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash0_write(long offset, const uint8_t *buf, size_t size)
{
  assert(sfud_dev0);
  assert(sfud_dev0->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev0, &status);
  // printf("fresh sfud_write Status: %d\r\n", ret);

  if(sfud_write(sfud_dev0, nor_flash0.addr + offset, size, buf) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/**
  ******************************************************************
  * @brief   Flash擦除
  * @param   [in]None
  * @return  擦除大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash0_erase(long offset, size_t size)
{
  assert(sfud_dev0);
  assert(sfud_dev0->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev0, &status);
  // printf("fresh sfud_erase Status: %d\r\n", ret);

  if (sfud_erase(sfud_dev0, nor_flash0.addr + offset, size) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/**
  ******************************************************************
  * @brief   Flash初始化
  * @param   [in]None
  * @return  0成功
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash1_init(void)
{
  //sfud_init();

#ifdef RT_USING_SFUD
  /* RT-Thread RTOS platform */
  sfud_dev1 = rt_sfud_flash_find_by_dev_name(FAL_USING_NOR_FLASH_DEV_NAME);
#else
  /* bare metal platform */
  sfud_dev1 = sfud_get_device(SFUD_GD25Q64C_DEVICE1_INDEX);
#endif

  if(NULL == sfud_dev1)
  {
    return -1;
  }

  /* 初始化 */
//  sfud_device_init(sfud_dev1);

  /* update the flash chip information */
  nor_flash1.blk_size = sfud_dev1->chip.erase_gran;
  nor_flash1.len = sfud_dev1->chip.capacity;

  return 0;
}

/**
  ******************************************************************
  * @brief   Flash读取
  * @param   [in]None
  * @return  读取大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash1_read(long offset, uint8_t *buf, size_t size)
{
  assert(sfud_dev1);
  assert(sfud_dev1->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev1, &status);
  // printf("fresh sfud_read Status: %d\r\n", ret);

  if(sfud_read(sfud_dev1, nor_flash1.addr + offset, size, buf) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/**
  ******************************************************************
  * @brief   Flash写入
  * @param   [in]None
  * @return  写入大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash1_write(long offset, const uint8_t *buf, size_t size)
{
  assert(sfud_dev1);
  assert(sfud_dev1->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev1, &status);
  // printf("fresh sfud_write Status: %d\r\n", ret);

  if(sfud_write(sfud_dev1, nor_flash1.addr + offset, size, buf) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/**
  ******************************************************************
  * @brief   Flash擦除
  * @param   [in]None
  * @return  擦除大小
  * @author  aron566
  * @version V1.0
  * @date    2021-05-14
  ******************************************************************
  */
static int flash1_erase(long offset, size_t size)
{
  assert(sfud_dev1);
  assert(sfud_dev1->init_ok);

  uint8_t status;
  // sfud_err ret;
  sfud_read_status(sfud_dev1, &status);
  // printf("fresh sfud_erase Status: %d\r\n", ret);

  if (sfud_erase(sfud_dev1, nor_flash1.addr + offset, size) != SFUD_SUCCESS)
  {
    return -1;
  }

  return size;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

#ifdef __cplusplus ///<end extern c
}
#endif
#endif /* FAL_USING_SFUD_PORT */
/******************************** End of file *********************************/
