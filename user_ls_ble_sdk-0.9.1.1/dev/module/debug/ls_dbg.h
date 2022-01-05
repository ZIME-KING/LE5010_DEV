#ifndef LS_DBG_H_
#define LS_DBG_H_
//#define NDEBUG

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef NDEBUG
#define LS_ASSERT(e) 
#define LS_RAM_ASSERT(e) 
#else
#if ROM_CODE == 1 && !defined(BOOT_ROM)
extern void (*stack_assert_asm_fn)(uint32_t,uint32_t,uint32_t);
#define LS_ASSERT(e) if(!(e)) stack_assert_asm_fn(1,0,0) 
#define LS_RAM_ASSERT(e) LS_ASSERT(e)
#else
#define LS_ASSERT(e) ((e)? (void)0 : ls_assert(#e,__FILE__,__LINE__))
#define LS_RAM_ASSERT(e) ((e)? (void)0 : ls_ram_assert())
#endif
#endif

void ls_assert(const char *expr,const char *file,int line);

void ls_ram_assert(void);

#endif /* LS_DBG_H_ */
