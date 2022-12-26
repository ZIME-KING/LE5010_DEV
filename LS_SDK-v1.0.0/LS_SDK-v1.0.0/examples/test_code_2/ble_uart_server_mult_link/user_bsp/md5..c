/*
 * File:		sha.c
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

#include "md5.h"

#define NULL 0


#define CAVAN_ROR(value, bits) \
	((value) >> (bits) | (value) << ((sizeof(value) << 3) - (bits)))

#define CAVAN_ROL(value, bits) \
	((value) << (bits) | (value) >> ((sizeof(value) << 3) - (bits)))

#define CAVAN_SWAP32(value) \
	((value) >> 24 | (value) << 24 | ((value) & 0x00FF0000) >> 8 | ((value) & 0x0000FF00) << 8)

#define SHA1_FUNC1_BASE(A, E, V) \
	(CAVAN_ROL(A, 5) + (V))

#define SHA1_FUNC2_BASE(B, E, V) \
	do { \
		(E) += (V); \
		(B) = CAVAN_ROR(B, 2); \
	} while (0)

#define SHA1_FUNC11(A, B, C, D, E, V) \
	SHA1_FUNC1_BASE(A, E, (V) + (((D) ^ ((B) & ((C) ^ (D)))) + 0x5A827999))

#define SHA1_FUNC21(A, B, C, D, E, V) \
	SHA1_FUNC2_BASE(B, E, SHA1_FUNC11(A, B, C, D, E, V))

#define SHA1_FUNC12(A, B, C, D, E, V) \
	SHA1_FUNC1_BASE(A, E, (V) + (((B) ^ (C) ^ (D)) + 0x6ED9EBA1))

#define SHA1_FUNC22(A, B, C, D, E, V) \
	SHA1_FUNC2_BASE(B, E, SHA1_FUNC12(A, B, C, D, E, V))

#define SHA1_FUNC13(A, B, C, D, E, V) \
	SHA1_FUNC1_BASE(A, E, (V) + ((((B) & (C)) | ((D) & ((B) | (C)))) + 0x8F1BBCDC))

#define SHA1_FUNC23(A, B, C, D, E, V) \
	SHA1_FUNC2_BASE(B, E, SHA1_FUNC13(A, B, C, D, E, V))

#define SHA1_FUNC14(A, B, C, D, E, V) \
	SHA1_FUNC1_BASE(A, E, (V) + (((B) ^ (C) ^ (D)) + 0xCA62C1D6))

#define SHA1_FUNC24(A, B, C, D, E, V) \
	SHA1_FUNC2_BASE(B, E, SHA1_FUNC14(A, B, C, D, E, V))

#define SHA1_TRANSFROM(A, B, C, D, E, W, F) \
	do { \
		register const u32 *p, *ep; \
		for (p = W, ep = p + 20; p < ep; p += 5) { \
			SHA1_FUNC2##F(A, B, C, D, E, p[0]); \
			SHA1_FUNC2##F(E, A, B, C, D, p[1]); \
			SHA1_FUNC2##F(D, E, A, B, C, p[2]); \
			SHA1_FUNC2##F(C, D, E, A, B, p[3]); \
			SHA1_FUNC2##F(B, C, D, E, A, p[4]); \
		} \
	} while (0)

// ============================================================

#define MD5_FUNC1(B, C, D) \
	(((B) & (C)) | (~(B) & (D)))

#define MD5_FUNC2(B, C, D) \
	(((B) & (D)) | ((C) & ~(D)))

#define MD5_FUNC3(B, C, D) \
	((B) ^ (C) ^ (D))

#define MD5_FUNC4(B, C, D) \
	((C) ^ ((B) | ~(D)))

#define MD5_TRANSFORM(A, B, C, D, V, S, I, F) \
	do { \
		(A) = (B) + CAVAN_ROL((A) + MD5_FUNC##F(B, C, D) + (V) + (I), S); \
	} while (0)

// ============================================================

void mem_CAVAN_SWAP32(u32 *dest, const u32 *src, u16 length)
{
	const u32 *src_end;

	for (src_end = src + length; src < src_end; dest++, src++) {
		*dest = CAVAN_SWAP32(*src);
	}
}

u8 *memcpy_invert8(u8 *dest, const u8 *src, u16 length)
{
	const u8 *last;

	for (last = src + length - 1; last >= src; dest++, last--) {
		*dest = *last;
	}

	return dest;
}

// ============================================================

int cavan_sha_init(struct cavan_sha_context *context)
{
	if (context->init == NULL) {
		return -1;
	}

	if (context->transform == NULL) {
		return -1;
	}

	context->count = 0;
	context->remain = 0;
	context->init(context, context->digest);

	return 0;
}

void cavan_sha_update(struct cavan_sha_context *context, const u8 *buff, size_t size)
{
	size_t remain;
	const u8 *buff_end = buff + size;

	if (context->remain > 0) {
		size_t padding;

		padding = sizeof(context->buff) - context->remain;
		if (padding <= size) {
			memcpy(context->buff + context->remain, buff, padding);
			context->transform(context->digest, context->dwbuff);
			buff += padding;
			context->remain = 0;
		}
	}

	while (1) {
		remain = buff_end - buff;
		if (remain < sizeof(context->buff)) {
			break;
		}

		context->transform(context->digest, (const u32 *) buff);
		buff += sizeof(context->buff);
	}

	if (remain) {
		memcpy(context->buff + context->remain, buff, remain);
		context->remain += remain;
	}

	context->count += size;
}


void cavan_sha_finish(struct cavan_sha_context *context, u8 *digest)
{
	u64 bits = context->count << 3;

	cavan_sha_update(context, "\x80", 1);

	if (context->remain > sizeof(context->buff) - sizeof(bits)) {
		memset(context->buff + context->remain, 0, sizeof(context->buff) - context->remain);
		context->transform(context->digest, context->dwbuff);
		memset(context->buff, 0, context->remain);
	} else {
		memset(context->buff + context->remain, 0, sizeof(context->buff) - context->remain - sizeof(bits));
	}

	if (context->flags & SHA_FLAG_SWAP) {
		memcpy_invert8(context->buff + sizeof(context->buff) - sizeof(bits), (void *) &bits, sizeof(bits));
	} else {
		memcpy(context->buff + sizeof(context->buff) - sizeof(bits), (void *) &bits, sizeof(bits));
	}

	context->transform(context->digest, context->dwbuff);

	if (context->flags & SHA_FLAG_SWAP) {
		mem_CAVAN_SWAP32((u32 *) digest, context->digest, context->digest_size >> 2);
	} else {
		memcpy(digest, context->digest, context->digest_size);
	}
}

int cavan_shasum(struct cavan_sha_context *context, const void *buff, size_t size, u8 *digest)
{
	int ret;

	ret = cavan_sha_init(context);
	if (ret < 0) {
		return ret;
	}

	cavan_sha_update(context, buff, size);
	cavan_sha_finish(context, digest);

	return 0;
}

// ============================================================

static void cavan_md5_init(struct cavan_sha_context *context, u32 digest[4])
{
	context->flags = 0;
	context->digest_size = MD5_DIGEST_SIZE;

	digest[0] = 0x67452301;
	digest[1] = 0xEFCDAB89;
	digest[2] = 0x98BADCFE;
	digest[3] = 0x10325476;
}

static void cavan_md5_transform(u32 digest[4], const u32 *buff)
{
	register u32 A, B, C, D;

	A = digest[0];
	B = digest[1];
	C = digest[2];
	D = digest[3];

	/* Round 1 */

	MD5_TRANSFORM(A, B, C, D, buff[0], 7, 0xD76AA478, 1);
	MD5_TRANSFORM(D, A, B, C, buff[1], 12, 0xE8C7B756, 1);
	MD5_TRANSFORM(C, D, A, B, buff[2], 17, 0x242070DB, 1);
	MD5_TRANSFORM(B, C, D, A, buff[3], 22, 0xC1BDCEEE, 1);

	MD5_TRANSFORM(A, B, C, D, buff[4], 7, 0xF57C0FAF, 1);
	MD5_TRANSFORM(D, A, B, C, buff[5], 12, 0x4787C62A, 1);
	MD5_TRANSFORM(C, D, A, B, buff[6], 17, 0xA8304613, 1);
	MD5_TRANSFORM(B, C, D, A, buff[7], 22, 0xFD469501, 1);

	MD5_TRANSFORM(A, B, C, D, buff[8], 7, 0x698098D8, 1);
	MD5_TRANSFORM(D, A, B, C, buff[9], 12, 0x8B44F7AF, 1);
	MD5_TRANSFORM(C, D, A, B, buff[10], 17, 0xFFFF5BB1, 1);
	MD5_TRANSFORM(B, C, D, A, buff[11], 22, 0x895CD7BE, 1);

	MD5_TRANSFORM(A, B, C, D, buff[12], 7, 0x6B901122, 1);
	MD5_TRANSFORM(D, A, B, C, buff[13], 12, 0xFD987193, 1);
	MD5_TRANSFORM(C, D, A, B, buff[14], 17, 0xA679438E, 1);
	MD5_TRANSFORM(B, C, D, A, buff[15], 22, 0x49B40821, 1);

	/* Round 2 */

	MD5_TRANSFORM(A, B, C, D, buff[1], 5, 0xF61E2562, 2);
	MD5_TRANSFORM(D, A, B, C, buff[6], 9, 0xC040B340, 2);
	MD5_TRANSFORM(C, D, A, B, buff[11], 14, 0x265E5A51, 2);
	MD5_TRANSFORM(B, C, D, A, buff[0], 20, 0xE9B6C7AA, 2);

	MD5_TRANSFORM(A, B, C, D, buff[5], 5, 0xD62F105D, 2);
	MD5_TRANSFORM(D, A, B, C, buff[10], 9, 0x2441453, 2);
	MD5_TRANSFORM(C, D, A, B, buff[15], 14, 0xD8A1E681, 2);
	MD5_TRANSFORM(B, C, D, A, buff[4], 20, 0xE7D3FBC8, 2);

	MD5_TRANSFORM(A, B, C, D, buff[9], 5, 0x21E1CDE6, 2);
	MD5_TRANSFORM(D, A, B, C, buff[14], 9, 0xC33707D6, 2);
	MD5_TRANSFORM(C, D, A, B, buff[3], 14, 0xF4D50D87, 2);
	MD5_TRANSFORM(B, C, D, A, buff[8], 20, 0x455A14ED, 2);

	MD5_TRANSFORM(A, B, C, D, buff[13], 5, 0xA9E3E905, 2);
	MD5_TRANSFORM(D, A, B, C, buff[2], 9, 0xFCEFA3F8, 2);
	MD5_TRANSFORM(C, D, A, B, buff[7], 14, 0x676F02D9, 2);
	MD5_TRANSFORM(B, C, D, A, buff[12], 20, 0x8D2A4C8A, 2);

	/* Round 3 */

	MD5_TRANSFORM(A, B, C, D, buff[5], 4, 0xFFFA3942, 3);
	MD5_TRANSFORM(D, A, B, C, buff[8], 11, 0x8771F681, 3);
	MD5_TRANSFORM(C, D, A, B, buff[11], 16, 0x6D9D6122, 3);
	MD5_TRANSFORM(B, C, D, A, buff[14], 23, 0xFDE5380C, 3);

	MD5_TRANSFORM(A, B, C, D, buff[1], 4, 0xA4BEEA44, 3);
	MD5_TRANSFORM(D, A, B, C, buff[4], 11, 0x4BDECFA9, 3);
	MD5_TRANSFORM(C, D, A, B, buff[7], 16, 0xF6BB4B60, 3);
	MD5_TRANSFORM(B, C, D, A, buff[10], 23, 0xBEBFBC70, 3);

	MD5_TRANSFORM(A, B, C, D, buff[13], 4, 0x289B7EC6, 3);
	MD5_TRANSFORM(D, A, B, C, buff[0], 11, 0xEAA127FA, 3);
	MD5_TRANSFORM(C, D, A, B, buff[3], 16, 0xD4EF3085, 3);
	MD5_TRANSFORM(B, C, D, A, buff[6], 23, 0x4881D05, 3);

	MD5_TRANSFORM(A, B, C, D, buff[9], 4, 0xD9D4D039, 3);
	MD5_TRANSFORM(D, A, B, C, buff[12], 11, 0xE6DB99E5, 3);
	MD5_TRANSFORM(C, D, A, B, buff[15], 16, 0x1FA27CF8, 3);
	MD5_TRANSFORM(B, C, D, A, buff[2], 23, 0xC4AC5665, 3);

	/* Round 4 */

	MD5_TRANSFORM(A, B, C, D, buff[0], 6, 0xF4292244, 4);
	MD5_TRANSFORM(D, A, B, C, buff[7], 10, 0x432AFF97, 4);
	MD5_TRANSFORM(C, D, A, B, buff[14], 15, 0xAB9423A7, 4);
	MD5_TRANSFORM(B, C, D, A, buff[5], 21, 0xFC93A039, 4);

	MD5_TRANSFORM(A, B, C, D, buff[12], 6, 0x655B59C3, 4);
	MD5_TRANSFORM(D, A, B, C, buff[3], 10, 0x8F0CCC92, 4);
	MD5_TRANSFORM(C, D, A, B, buff[10], 15, 0xFFEFF47D, 4);
	MD5_TRANSFORM(B, C, D, A, buff[1], 21, 0x85845DD1, 4);

	MD5_TRANSFORM(A, B, C, D, buff[8], 6, 0x6FA87E4F, 4);
	MD5_TRANSFORM(D, A, B, C, buff[15], 10, 0xFE2CE6E0, 4);
	MD5_TRANSFORM(C, D, A, B, buff[6], 15, 0xA3014314, 4);
	MD5_TRANSFORM(B, C, D, A, buff[13], 21, 0x4E0811A1, 4);

	MD5_TRANSFORM(A, B, C, D, buff[4], 6, 0xF7537E82, 4);
	MD5_TRANSFORM(D, A, B, C, buff[11], 10, 0xBD3AF235, 4);
	MD5_TRANSFORM(C, D, A, B, buff[2], 15, 0x2AD7D2BB, 4);
	MD5_TRANSFORM(B, C, D, A, buff[9], 21, 0xEB86D391, 4);

	digest[0] += A;
	digest[1] += B;
	digest[2] += C;
	digest[3] += D;
}

void cavan_md5_init_context(struct cavan_sha_context *context)
{
	context->init = cavan_md5_init;
	context->transform = cavan_md5_transform;
}

// ============================================================

static void cavan_sha1_init(struct cavan_sha_context *context, u32 digest[5])
{
	context->flags = SHA_FLAG_SWAP;
	context->digest_size = SHA1_DIGEST_SIZE;

	digest[0] = 0x67452301;
	digest[1] = 0xEFCDAB89;
	digest[2] = 0x98BADCFE;
	digest[3] = 0x10325476;
	digest[4] = 0xC3D2E1F0;
}

static void cavan_sha1_transform(u32 digest[5], const u32 *buff)
{
	int i;
	u32 W[80];
	register u32 A, B, C, D, E;

	mem_CAVAN_SWAP32(W, buff, 16);

	for(i = 16; i < 80; i++) {
		W[i] = CAVAN_ROL(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
	}

	A = digest[0];
	B = digest[1];
	C = digest[2];
	D = digest[3];
	E = digest[4];

	SHA1_TRANSFROM(A, B, C, D, E, W, 1);
	SHA1_TRANSFROM(A, B, C, D, E, W + 20, 2);
	SHA1_TRANSFROM(A, B, C, D, E, W + 40, 3);
	SHA1_TRANSFROM(A, B, C, D, E, W + 60, 4);

	digest[0] += A;
	digest[1] += B;
	digest[2] += C;
	digest[3] += D;
	digest[4] += E;
}

void cavan_sha1_context_init(struct cavan_sha_context *context)
{
	context->init = cavan_sha1_init;
	context->transform = cavan_sha1_transform;
}
