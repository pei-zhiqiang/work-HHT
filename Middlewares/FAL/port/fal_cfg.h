#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_PART_HAS_TABLE_CFG

//#define FAL_PART_TABLE_FLASH_DEV_NAME  "STM32H743VxFlash"
//#define FAL_PART_TABLE_END_OFFSET      (128*1024*8*2L)
#define ON_CHIP_FLASH_DEV_NAME         "stm32_onchip"
#define NOR_FLASH_DEV0_NAME             "GD25Q64C0"
#define NOR_FLASH_DEV1_NAME             "GD25Q64C1"
/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32h7_onchip_flash;
extern struct fal_flash_dev nor_flash0;
extern struct fal_flash_dev nor_flash1;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32h7_onchip_flash,                                           \
    &nor_flash0,                                                     \
    &nor_flash1,                                                     \
}

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                  \
{                                                                                       \
  {FAL_PART_MAGIC_WORD, "bl",               ON_CHIP_FLASH_DEV_NAME, 0,           128*1024,   0},\
  {FAL_PART_MAGIC_WORD, "app",              ON_CHIP_FLASH_DEV_NAME, 128*1024,    128*1024*15, 0},\
  {FAL_PART_MAGIC_WORD, "kvdb",             NOR_FLASH_DEV0_NAME, 0,           512*1024,   0},\
  {FAL_PART_MAGIC_WORD, "entry_config.pcm", NOR_FLASH_DEV0_NAME, 512*1024,    512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "exit_config.pcm",  NOR_FLASH_DEV0_NAME, 512*1024*2,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "reserved_X",       NOR_FLASH_DEV0_NAME, 512*1024*3,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "reserved_0",       NOR_FLASH_DEV0_NAME, 512*1024*4,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "reserved_1",       NOR_FLASH_DEV0_NAME, 512*1024*5,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "reserved_2",       NOR_FLASH_DEV0_NAME, 512*1024*6,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "download",         NOR_FLASH_DEV0_NAME, 512*1024*7,  512*1024*4, 0},\
  {FAL_PART_MAGIC_WORD, "factory",          NOR_FLASH_DEV0_NAME, 512*1024*11, 512*1024*4, 0},\
  {FAL_PART_MAGIC_WORD, "flag",             NOR_FLASH_DEV0_NAME, 512*1024*15, 512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "A",                NOR_FLASH_DEV1_NAME, 0,           512*1024,   0},\
  {FAL_PART_MAGIC_WORD, "AA",               NOR_FLASH_DEV1_NAME, 512*1024,    512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "AAA",              NOR_FLASH_DEV1_NAME, 512*1024*2,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "AAAA",             NOR_FLASH_DEV1_NAME, 512*1024*3,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "B",                NOR_FLASH_DEV1_NAME, 512*1024*4,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "BB",               NOR_FLASH_DEV1_NAME, 512*1024*5,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "BBB",              NOR_FLASH_DEV1_NAME, 512*1024*6,  512*1024*1, 0},\
  {FAL_PART_MAGIC_WORD, "BBBB",             NOR_FLASH_DEV1_NAME, 512*1024*7,  512*1024*4, 0},\
  {FAL_PART_MAGIC_WORD, "C",                NOR_FLASH_DEV1_NAME, 512*1024*11, 512*1024*4, 0},\
  {FAL_PART_MAGIC_WORD, "CC",               NOR_FLASH_DEV1_NAME, 512*1024*15, 512*1024*1, 0},\
}

#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
