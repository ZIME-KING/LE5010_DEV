//#pragma once
#ifndef MD5_H_
#define MD5_H_
/*
 * File:		sha.h
 * Author:		Fuang.Cao <cavan.cfa@gmail.com>
 * Created:		2014-05-28 17:50:42
 *
 * Copyright (c) 2014 Fuang.Cao <cavan.cfa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#pragma anon_unions

#include "user_main.h"



#define SHA_FLAG_SWAP		(1 << 0)

#define SHA1_DIGEST_SIZE	20
#define MD5_DIGEST_SIZE		16

struct cavan_sha_context {
	u64 count;
	int flags;
	u32 digest[8];
	size_t remain;
	size_t digest_size;

	union {
		u8 buff[64];
		u16 wbuff[32];
		u32 dwbuff[16];
	};

	void (*init)(struct cavan_sha_context *context, u32 *digest);
	void (*transform)(u32 *digest, const u32 *buff);
};

int cavan_sha_init(struct cavan_sha_context *context);
void cavan_sha_update(struct cavan_sha_context *context, const u8 *buff, size_t size);
void cavan_sha_finish(struct cavan_sha_context *context, u8 *digest);
int cavan_shasum(struct cavan_sha_context *context, const void *buff, size_t size, u8 *digest);

void cavan_md5_init_context(struct cavan_sha_context *context);
void cavan_sha1_context_init(struct cavan_sha_context *context);

#endif