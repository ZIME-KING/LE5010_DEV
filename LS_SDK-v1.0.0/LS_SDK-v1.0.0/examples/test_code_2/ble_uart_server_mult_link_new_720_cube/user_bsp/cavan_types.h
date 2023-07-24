#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "os_task.h"
#include "os_sync.h"
#include "os_sched.h"

#include "trace.h"
#include "rtl876x.h"
#include "system_rtl876x.h"
#include "platform_utils.h"

#include "cavan_errno.h"

#define CAVAN_DEBUG		0

#define MAX_U8			0xFF
#define MAX_U16			0xFFFF
#define MAX_U32			0xFFFFFFFF
#define MAX_U64			0xFFFFFFFFFFFFFFFF

#define KB(value) \
	((value) << 10)

#define MB(value) \
	((value) << 20)

#define CAST_U8(value) \
	((u8) (value))

#define CAST_U16(value) \
	((u16) (value))

#define CAST_U32(value) \
	((u32) (value))

#define CAST_P8(value) \
	((u8 *) (value))

#define CAST_P16(value) \
	((u16 *) (value))

#define CAST_P32(value) \
	((u32 *) (value))

#define NELEM(a) \
	(sizeof(a) / sizeof((a)[0]))

#define OFFSET(type, member) \
	((int) &(((type *) 0)->member))

#define CONTAINER(pointer, type, member) \
	((type *) (CAST_P8(pointer) - OFFSET(type, member)))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

