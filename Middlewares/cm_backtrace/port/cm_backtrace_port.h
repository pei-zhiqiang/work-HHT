/**
 *  @file cm_backtrace_port.h
 *
 *  @date 2021/2/6
 *
 *  @author aron566
 *
 *  @brief 错误追踪库接口
 *  
 *  @version v1.0
 */
#ifndef CM_BACKTRACE_PORT_H
#define CM_BACKTRACE_PORT_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t*/
#include <stddef.h> /**< need definition of NULL*/
#include <stdbool.h>/**< need definition of BOOL*/
#include <stdio.h>  /**< if need printf*/
#include <stdlib.h>
#include <string.h>
#include <limits.h>/**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
#include "cm_backtrace.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/* include or export for supported cmb_get_lr function */
#if defined(__CC_ARM)
    static __inline __asm uint32_t cmb_get_lr(void) {
        mov r0, lr
        bx lr
    }
#elif defined(__CLANG_ARM)
    __attribute__( (always_inline) ) static __inline uint32_t cmb_get_lr(void) {
        uint32_t result;
        __asm volatile ("mov %0, lr" : "=r" (result) );
        return (result);
    }
#elif defined(__ICCARM__)
/* IAR iccarm specific functions */
/* Close Raw Asm Code Warning */  
#pragma diag_suppress=Pe940    
    static uint32_t cmb_get_lr(void)
    {
      __asm("mov r0, lr");
      __asm("bx lr");///< 作用等同于【MOV PC,LR，即跳转到LR中存放的地址处】
    }
#pragma diag_default=Pe940  
#elif defined(__GNUC__)
    __attribute__( ( always_inline ) ) static inline uint32_t cmb_get_lr(void) {
        register uint32_t result;
        __asm volatile ("MOV %0, lr\n" : "=r" (result) );
        return(result);
    }
#else
    #error "not supported compiler"
#endif
    
/**
  ******************************************************************
  * @brief   错误追踪库接口获取函数调用栈
  * @param   [in]sp sp堆栈指针
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/6
  ******************************************************************
  */
#define cm_backtrace_port_get_call_stack() \
do{ \
  /* 建立深度为 16 的函数调用栈缓冲区，深度大小不应该超过 CMB_CALL_STACK_MAX_DEPTH（默认16） */ \
  uint32_t call_stack[16] = {0};  \
  size_t i, depth = 0;            \
                                  \
  /* 获取当前环境下的函数调用栈，每个元素将会以 32 位地址形式存储， depth 为函数调用栈实际深度 */\
  depth = cm_backtrace_call_stack(call_stack, sizeof(call_stack), cmb_get_sp());\
  \
  /* 输出当前函数调用栈信息
   * 注意：查看函数名称及具体行号时，需要使用 addr2line 工具转换
   */\
  for(i = 0; i < depth; i++) \
  {  \
      printf("%08x ", call_stack[i]);\
  }\
}while(0)

/**
  ******************************************************************
  * @brief   错误追踪库接口追踪断言错误信息
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/6
  ******************************************************************
  */
#define cm_backtrace_port_assert() cm_backtrace_assert(cmb_get_sp())

/**
  ******************************************************************
  * @brief   错误追踪库接口追踪故障错误信息
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/7
  ******************************************************************
  */
typedef void (*CM_BACKTRACE_FAULT_FUNC)(void);
#if defined(__ICCARM__)
#define cm_backtrace_port_fault() \
do{ \
  __asm("mov r0, lr"); \
  __asm("mov r1, sp"); \
  __asm("adds r1, r1, #8"); \
  ((CM_BACKTRACE_FAULT_FUNC)cm_backtrace_fault)();\
}while(0)
#elif defined(__CC_ARM)
#define cm_backtrace_port_fault() \
do{ \
  uint32_t LR_Val = cmb_get_lr(); \
  uint32_t SP_Val = cmb_get_sp(); \
  cm_backtrace_fault(LR_Val, SP_Val);\
}while(0)
#elif defined(__GNUC__)
#define cm_backtrace_port_fault() \
do{ \
  register uint32_t LR_Val; \
  register uint32_t SP_Val; \
  __asm volatile ("MOV %0, lr\n" : "=r" (LR_Val) ); \
  __asm volatile ("MOV %0, sp\n" : "=r" (SP_Val) ); \
  cm_backtrace_fault(LR_Val, SP_Val);\
}while(0)
#endif
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*错误追踪库接口初始化*/
void CM_Backtrace_Port_Init(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
