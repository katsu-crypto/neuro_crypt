/**
 * magma.c
 *
 */

#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>

#include "magma.h"

struct magma_subkeys
{
	uint32_t k[8];
};

struct magma_subkeys *keys_magma_ctx(struct crypto_magma_ctx *magma_ctx)
{
	return magma_ctx->keys;
}

struct crypto_magma_ctx *create_magma_ctx()
{
	struct crypto_magma_ctx *this = malloc(sizeof(struct crypto_magma_ctx));
	this->keys = malloc(sizeof(struct magma_subkeys));
	return this;
}

void delete_magma_ctx(struct crypto_magma_ctx *magma_ctx)
{
	//memset((uint8_t *)magma_ctx->keys->k, 0, 8 * sizeof(uint32_t));
	free(magma_ctx->keys);
	magma_ctx->keys = NULL;
	free(magma_ctx);
}

static unsigned char const k8[16] = {
	14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};
static unsigned char const k7[16] = {
	15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10};
static unsigned char const k6[16] = {
	10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8};
static unsigned char const k5[16] = {
	7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15};
static unsigned char const k4[16] = {
	2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9};
static unsigned char const k3[16] = {
	12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11};
static unsigned char const k2[16] = {
	4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1};
static unsigned char const k1[16] = {
	13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7};

/* Compressed and already rotated s-boxes */
static uint32_t pi87[256] = {
	0x000000c0, 0x000000f0, 0x00000090, 0x000000a8, 0x000000b0, 0x000000c8,
	0x00000088, 0x000000e0, 0x000000f8, 0x000000a0, 0x000000d8, 0x00000080,
	0x000000e8, 0x000000d0, 0x00000098, 0x000000b8, 0x000003c0, 0x000003f0,
	0x00000390, 0x000003a8, 0x000003b0, 0x000003c8, 0x00000388, 0x000003e0,
	0x000003f8, 0x000003a0, 0x000003d8, 0x00000380, 0x000003e8, 0x000003d0,
	0x00000398, 0x000003b8, 0x00000740, 0x00000770, 0x00000710, 0x00000728,
	0x00000730, 0x00000748, 0x00000708, 0x00000760, 0x00000778, 0x00000720,
	0x00000758, 0x00000700, 0x00000768, 0x00000750, 0x00000718, 0x00000738,
	0x000006c0, 0x000006f0, 0x00000690, 0x000006a8, 0x000006b0, 0x000006c8,
	0x00000688, 0x000006e0, 0x000006f8, 0x000006a0, 0x000006d8, 0x00000680,
	0x000006e8, 0x000006d0, 0x00000698, 0x000006b8, 0x00000040, 0x00000070,
	0x00000010, 0x00000028, 0x00000030, 0x00000048, 0x00000008, 0x00000060,
	0x00000078, 0x00000020, 0x00000058, 0x00000000, 0x00000068, 0x00000050,
	0x00000018, 0x00000038, 0x000002c0, 0x000002f0, 0x00000290, 0x000002a8,
	0x000002b0, 0x000002c8, 0x00000288, 0x000002e0, 0x000002f8, 0x000002a0,
	0x000002d8, 0x00000280, 0x000002e8, 0x000002d0, 0x00000298, 0x000002b8,
	0x00000440, 0x00000470, 0x00000410, 0x00000428, 0x00000430, 0x00000448,
	0x00000408, 0x00000460, 0x00000478, 0x00000420, 0x00000458, 0x00000400,
	0x00000468, 0x00000450, 0x00000418, 0x00000438, 0x000001c0, 0x000001f0,
	0x00000190, 0x000001a8, 0x000001b0, 0x000001c8, 0x00000188, 0x000001e0,
	0x000001f8, 0x000001a0, 0x000001d8, 0x00000180, 0x000001e8, 0x000001d0,
	0x00000198, 0x000001b8, 0x00000240, 0x00000270, 0x00000210, 0x00000228,
	0x00000230, 0x00000248, 0x00000208, 0x00000260, 0x00000278, 0x00000220,
	0x00000258, 0x00000200, 0x00000268, 0x00000250, 0x00000218, 0x00000238,
	0x000007c0, 0x000007f0, 0x00000790, 0x000007a8, 0x000007b0, 0x000007c8,
	0x00000788, 0x000007e0, 0x000007f8, 0x000007a0, 0x000007d8, 0x00000780,
	0x000007e8, 0x000007d0, 0x00000798, 0x000007b8, 0x00000540, 0x00000570,
	0x00000510, 0x00000528, 0x00000530, 0x00000548, 0x00000508, 0x00000560,
	0x00000578, 0x00000520, 0x00000558, 0x00000500, 0x00000568, 0x00000550,
	0x00000518, 0x00000538, 0x00000340, 0x00000370, 0x00000310, 0x00000328,
	0x00000330, 0x00000348, 0x00000308, 0x00000360, 0x00000378, 0x00000320,
	0x00000358, 0x00000300, 0x00000368, 0x00000350, 0x00000318, 0x00000338,
	0x000004c0, 0x000004f0, 0x00000490, 0x000004a8, 0x000004b0, 0x000004c8,
	0x00000488, 0x000004e0, 0x000004f8, 0x000004a0, 0x000004d8, 0x00000480,
	0x000004e8, 0x000004d0, 0x00000498, 0x000004b8, 0x00000640, 0x00000670,
	0x00000610, 0x00000628, 0x00000630, 0x00000648, 0x00000608, 0x00000660,
	0x00000678, 0x00000620, 0x00000658, 0x00000600, 0x00000668, 0x00000650,
	0x00000618, 0x00000638, 0x000005c0, 0x000005f0, 0x00000590, 0x000005a8,
	0x000005b0, 0x000005c8, 0x00000588, 0x000005e0, 0x000005f8, 0x000005a0,
	0x000005d8, 0x00000580, 0x000005e8, 0x000005d0, 0x00000598, 0x000005b8,
	0x00000140, 0x00000170, 0x00000110, 0x00000128, 0x00000130, 0x00000148,
	0x00000108, 0x00000160, 0x00000178, 0x00000120, 0x00000158, 0x00000100,
	0x00000168, 0x00000150, 0x00000118, 0x00000138};

static uint32_t pi65[256] = {
	0xb8000002, 0xf8000002, 0xa8000002, 0xd0000002, 0xc0000002, 0x88000002,
	0xb0000002, 0xe8000002, 0x80000002, 0xc8000002, 0x98000002, 0xf0000002,
	0xd8000002, 0xa0000002, 0x90000002, 0xe0000002, 0xb8000006, 0xf8000006,
	0xa8000006, 0xd0000006, 0xc0000006, 0x88000006, 0xb0000006, 0xe8000006,
	0x80000006, 0xc8000006, 0x98000006, 0xf0000006, 0xd8000006, 0xa0000006,
	0x90000006, 0xe0000006, 0xb8000007, 0xf8000007, 0xa8000007, 0xd0000007,
	0xc0000007, 0x88000007, 0xb0000007, 0xe8000007, 0x80000007, 0xc8000007,
	0x98000007, 0xf0000007, 0xd8000007, 0xa0000007, 0x90000007, 0xe0000007,
	0x38000003, 0x78000003, 0x28000003, 0x50000003, 0x40000003, 0x08000003,
	0x30000003, 0x68000003, 0x00000003, 0x48000003, 0x18000003, 0x70000003,
	0x58000003, 0x20000003, 0x10000003, 0x60000003, 0xb8000004, 0xf8000004,
	0xa8000004, 0xd0000004, 0xc0000004, 0x88000004, 0xb0000004, 0xe8000004,
	0x80000004, 0xc8000004, 0x98000004, 0xf0000004, 0xd8000004, 0xa0000004,
	0x90000004, 0xe0000004, 0x38000001, 0x78000001, 0x28000001, 0x50000001,
	0x40000001, 0x08000001, 0x30000001, 0x68000001, 0x00000001, 0x48000001,
	0x18000001, 0x70000001, 0x58000001, 0x20000001, 0x10000001, 0x60000001,
	0x38000006, 0x78000006, 0x28000006, 0x50000006, 0x40000006, 0x08000006,
	0x30000006, 0x68000006, 0x00000006, 0x48000006, 0x18000006, 0x70000006,
	0x58000006, 0x20000006, 0x10000006, 0x60000006, 0x38000005, 0x78000005,
	0x28000005, 0x50000005, 0x40000005, 0x08000005, 0x30000005, 0x68000005,
	0x00000005, 0x48000005, 0x18000005, 0x70000005, 0x58000005, 0x20000005,
	0x10000005, 0x60000005, 0xb8000005, 0xf8000005, 0xa8000005, 0xd0000005,
	0xc0000005, 0x88000005, 0xb0000005, 0xe8000005, 0x80000005, 0xc8000005,
	0x98000005, 0xf0000005, 0xd8000005, 0xa0000005, 0x90000005, 0xe0000005,
	0xb8000003, 0xf8000003, 0xa8000003, 0xd0000003, 0xc0000003, 0x88000003,
	0xb0000003, 0xe8000003, 0x80000003, 0xc8000003, 0x98000003, 0xf0000003,
	0xd8000003, 0xa0000003, 0x90000003, 0xe0000003, 0x38000004, 0x78000004,
	0x28000004, 0x50000004, 0x40000004, 0x08000004, 0x30000004, 0x68000004,
	0x00000004, 0x48000004, 0x18000004, 0x70000004, 0x58000004, 0x20000004,
	0x10000004, 0x60000004, 0xb8000000, 0xf8000000, 0xa8000000, 0xd0000000,
	0xc0000000, 0x88000000, 0xb0000000, 0xe8000000, 0x80000000, 0xc8000000,
	0x98000000, 0xf0000000, 0xd8000000, 0xa0000000, 0x90000000, 0xe0000000,
	0x38000002, 0x78000002, 0x28000002, 0x50000002, 0x40000002, 0x08000002,
	0x30000002, 0x68000002, 0x00000002, 0x48000002, 0x18000002, 0x70000002,
	0x58000002, 0x20000002, 0x10000002, 0x60000002, 0xb8000001, 0xf8000001,
	0xa8000001, 0xd0000001, 0xc0000001, 0x88000001, 0xb0000001, 0xe8000001,
	0x80000001, 0xc8000001, 0x98000001, 0xf0000001, 0xd8000001, 0xa0000001,
	0x90000001, 0xe0000001, 0x38000007, 0x78000007, 0x28000007, 0x50000007,
	0x40000007, 0x08000007, 0x30000007, 0x68000007, 0x00000007, 0x48000007,
	0x18000007, 0x70000007, 0x58000007, 0x20000007, 0x10000007, 0x60000007,
	0x38000000, 0x78000000, 0x28000000, 0x50000000, 0x40000000, 0x08000000,
	0x30000000, 0x68000000, 0x00000000, 0x48000000, 0x18000000, 0x70000000,
	0x58000000, 0x20000000, 0x10000000, 0x60000000};

static uint32_t pi43[256] = {
	0x06580000, 0x06180000, 0x06280000, 0x06400000, 0x06100000, 0x06780000,
	0x06500000, 0x06680000, 0x06700000, 0x06080000, 0x06380000, 0x06200000,
	0x06600000, 0x06480000, 0x06300000, 0x06000000, 0x04580000, 0x04180000,
	0x04280000, 0x04400000, 0x04100000, 0x04780000, 0x04500000, 0x04680000,
	0x04700000, 0x04080000, 0x04380000, 0x04200000, 0x04600000, 0x04480000,
	0x04300000, 0x04000000, 0x01580000, 0x01180000, 0x01280000, 0x01400000,
	0x01100000, 0x01780000, 0x01500000, 0x01680000, 0x01700000, 0x01080000,
	0x01380000, 0x01200000, 0x01600000, 0x01480000, 0x01300000, 0x01000000,
	0x00d80000, 0x00980000, 0x00a80000, 0x00c00000, 0x00900000, 0x00f80000,
	0x00d00000, 0x00e80000, 0x00f00000, 0x00880000, 0x00b80000, 0x00a00000,
	0x00e00000, 0x00c80000, 0x00b00000, 0x00800000, 0x06d80000, 0x06980000,
	0x06a80000, 0x06c00000, 0x06900000, 0x06f80000, 0x06d00000, 0x06e80000,
	0x06f00000, 0x06880000, 0x06b80000, 0x06a00000, 0x06e00000, 0x06c80000,
	0x06b00000, 0x06800000, 0x02580000, 0x02180000, 0x02280000, 0x02400000,
	0x02100000, 0x02780000, 0x02500000, 0x02680000, 0x02700000, 0x02080000,
	0x02380000, 0x02200000, 0x02600000, 0x02480000, 0x02300000, 0x02000000,
	0x07d80000, 0x07980000, 0x07a80000, 0x07c00000, 0x07900000, 0x07f80000,
	0x07d00000, 0x07e80000, 0x07f00000, 0x07880000, 0x07b80000, 0x07a00000,
	0x07e00000, 0x07c80000, 0x07b00000, 0x07800000, 0x03580000, 0x03180000,
	0x03280000, 0x03400000, 0x03100000, 0x03780000, 0x03500000, 0x03680000,
	0x03700000, 0x03080000, 0x03380000, 0x03200000, 0x03600000, 0x03480000,
	0x03300000, 0x03000000, 0x03d80000, 0x03980000, 0x03a80000, 0x03c00000,
	0x03900000, 0x03f80000, 0x03d00000, 0x03e80000, 0x03f00000, 0x03880000,
	0x03b80000, 0x03a00000, 0x03e00000, 0x03c80000, 0x03b00000, 0x03800000,
	0x00580000, 0x00180000, 0x00280000, 0x00400000, 0x00100000, 0x00780000,
	0x00500000, 0x00680000, 0x00700000, 0x00080000, 0x00380000, 0x00200000,
	0x00600000, 0x00480000, 0x00300000, 0x00000000, 0x05580000, 0x05180000,
	0x05280000, 0x05400000, 0x05100000, 0x05780000, 0x05500000, 0x05680000,
	0x05700000, 0x05080000, 0x05380000, 0x05200000, 0x05600000, 0x05480000,
	0x05300000, 0x05000000, 0x02d80000, 0x02980000, 0x02a80000, 0x02c00000,
	0x02900000, 0x02f80000, 0x02d00000, 0x02e80000, 0x02f00000, 0x02880000,
	0x02b80000, 0x02a00000, 0x02e00000, 0x02c80000, 0x02b00000, 0x02800000,
	0x01d80000, 0x01980000, 0x01a80000, 0x01c00000, 0x01900000, 0x01f80000,
	0x01d00000, 0x01e80000, 0x01f00000, 0x01880000, 0x01b80000, 0x01a00000,
	0x01e00000, 0x01c80000, 0x01b00000, 0x01800000, 0x07580000, 0x07180000,
	0x07280000, 0x07400000, 0x07100000, 0x07780000, 0x07500000, 0x07680000,
	0x07700000, 0x07080000, 0x07380000, 0x07200000, 0x07600000, 0x07480000,
	0x07300000, 0x07000000, 0x04d80000, 0x04980000, 0x04a80000, 0x04c00000,
	0x04900000, 0x04f80000, 0x04d00000, 0x04e80000, 0x04f00000, 0x04880000,
	0x04b80000, 0x04a00000, 0x04e00000, 0x04c80000, 0x04b00000, 0x04800000,
	0x05d80000, 0x05980000, 0x05a80000, 0x05c00000, 0x05900000, 0x05f80000,
	0x05d00000, 0x05e80000, 0x05f00000, 0x05880000, 0x05b80000, 0x05a00000,
	0x05e00000, 0x05c80000, 0x05b00000, 0x05800000};

static uint32_t pi21[256] = {
	0x00036000, 0x00032000, 0x00033000, 0x00031000, 0x00035000, 0x00032800,
	0x00035800, 0x00034800, 0x00037000, 0x00034000, 0x00036800, 0x00033800,
	0x00030000, 0x00031800, 0x00037800, 0x00030800, 0x00046000, 0x00042000,
	0x00043000, 0x00041000, 0x00045000, 0x00042800, 0x00045800, 0x00044800,
	0x00047000, 0x00044000, 0x00046800, 0x00043800, 0x00040000, 0x00041800,
	0x00047800, 0x00040800, 0x00016000, 0x00012000, 0x00013000, 0x00011000,
	0x00015000, 0x00012800, 0x00015800, 0x00014800, 0x00017000, 0x00014000,
	0x00016800, 0x00013800, 0x00010000, 0x00011800, 0x00017800, 0x00010800,
	0x0001e000, 0x0001a000, 0x0001b000, 0x00019000, 0x0001d000, 0x0001a800,
	0x0001d800, 0x0001c800, 0x0001f000, 0x0001c000, 0x0001e800, 0x0001b800,
	0x00018000, 0x00019800, 0x0001f800, 0x00018800, 0x0004e000, 0x0004a000,
	0x0004b000, 0x00049000, 0x0004d000, 0x0004a800, 0x0004d800, 0x0004c800,
	0x0004f000, 0x0004c000, 0x0004e800, 0x0004b800, 0x00048000, 0x00049800,
	0x0004f800, 0x00048800, 0x00056000, 0x00052000, 0x00053000, 0x00051000,
	0x00055000, 0x00052800, 0x00055800, 0x00054800, 0x00057000, 0x00054000,
	0x00056800, 0x00053800, 0x00050000, 0x00051800, 0x00057800, 0x00050800,
	0x0002e000, 0x0002a000, 0x0002b000, 0x00029000, 0x0002d000, 0x0002a800,
	0x0002d800, 0x0002c800, 0x0002f000, 0x0002c000, 0x0002e800, 0x0002b800,
	0x00028000, 0x00029800, 0x0002f800, 0x00028800, 0x00066000, 0x00062000,
	0x00063000, 0x00061000, 0x00065000, 0x00062800, 0x00065800, 0x00064800,
	0x00067000, 0x00064000, 0x00066800, 0x00063800, 0x00060000, 0x00061800,
	0x00067800, 0x00060800, 0x0000e000, 0x0000a000, 0x0000b000, 0x00009000,
	0x0000d000, 0x0000a800, 0x0000d800, 0x0000c800, 0x0000f000, 0x0000c000,
	0x0000e800, 0x0000b800, 0x00008000, 0x00009800, 0x0000f800, 0x00008800,
	0x00076000, 0x00072000, 0x00073000, 0x00071000, 0x00075000, 0x00072800,
	0x00075800, 0x00074800, 0x00077000, 0x00074000, 0x00076800, 0x00073800,
	0x00070000, 0x00071800, 0x00077800, 0x00070800, 0x00026000, 0x00022000,
	0x00023000, 0x00021000, 0x00025000, 0x00022800, 0x00025800, 0x00024800,
	0x00027000, 0x00024000, 0x00026800, 0x00023800, 0x00020000, 0x00021800,
	0x00027800, 0x00020800, 0x0003e000, 0x0003a000, 0x0003b000, 0x00039000,
	0x0003d000, 0x0003a800, 0x0003d800, 0x0003c800, 0x0003f000, 0x0003c000,
	0x0003e800, 0x0003b800, 0x00038000, 0x00039800, 0x0003f800, 0x00038800,
	0x0005e000, 0x0005a000, 0x0005b000, 0x00059000, 0x0005d000, 0x0005a800,
	0x0005d800, 0x0005c800, 0x0005f000, 0x0005c000, 0x0005e800, 0x0005b800,
	0x00058000, 0x00059800, 0x0005f800, 0x00058800, 0x0006e000, 0x0006a000,
	0x0006b000, 0x00069000, 0x0006d000, 0x0006a800, 0x0006d800, 0x0006c800,
	0x0006f000, 0x0006c000, 0x0006e800, 0x0006b800, 0x00068000, 0x00069800,
	0x0006f800, 0x00068800, 0x00006000, 0x00002000, 0x00003000, 0x00001000,
	0x00005000, 0x00002800, 0x00005800, 0x00004800, 0x00007000, 0x00004000,
	0x00006800, 0x00003800, 0x00000000, 0x00001800, 0x00007800, 0x00000800,
	0x0007e000, 0x0007a000, 0x0007b000, 0x00079000, 0x0007d000, 0x0007a800,
	0x0007d800, 0x0007c800, 0x0007f000, 0x0007c000, 0x0007e800, 0x0007b800,
	0x00078000, 0x00079800, 0x0007f800, 0x00078800};

inline static uint32_t f(uint32_t x)
{
	return pi87[x >> 24 & 0xff] | pi65[x >> 16 & 0xff] |
		   pi43[x >> 8 & 0xff] | pi21[x & 0xff];
}
/* Slower version of previuos function */
inline static uint32_t _f(uint32_t x)
{
	x = k8[x >> 28 & 15] << 28 | k7[x >> 24 & 15] << 24 |
		k6[x >> 20 & 15] << 20 | k5[x >> 16 & 15] << 16 |
		k4[x >> 12 & 15] << 12 | k3[x >> 8 & 15] << 8 |
		k2[x >> 4 & 15] << 4 | k1[x & 15];

	/* Rotate left 11 bits */
	return x << 11 | x >> (32 - 11);
}

/* neuro  primitive without rotation*/
inline static uint32_t f_neuro_g1(uint32_t x, uint32_t key)
{
	uint8_t buf, buf1;
	// printf("x: %llx\n", x);
	// printf("key: %llx\n", key);
	for (size_t i = 0; i < 8; i += 2)
	{
		// printf("x_: %x\n", (x >> (4 * i) & 15));
		// printf("key_: %x\n", (key >> (4 * i) & 15));

		// printf("x_: %x\n", (x >> (4 * i + 4) & 15));
		// printf("key_: %x\n", (key >> (4 * i + 4) & 15));

		buf = ((x >> (4 * i) & 15) + (key >> (4 * i) & 15)) & 15;
		buf1 = ((x >> (4 * i + 4) & 15) + (key >> (4 * i + 4) & 15)) & 15;
		// printf("buf: %x\n", buf);
		// printf("buf1: %x\n", buf1);
		// printf("sum: %lx\n", buf | (buf1 << 4));

		((uint8_t *)&x)[i / 2] = buf | (buf1 << 4);
	}
	// printf("x: %llx\n", x);
	x = k8[x >> 28 & 15] << 28 | k7[x >> 24 & 15] << 24 |
		k6[x >> 20 & 15] << 20 | k5[x >> 16 & 15] << 16 |
		k4[x >> 12 & 15] << 12 | k3[x >> 8 & 15] << 8 |
		k2[x >> 4 & 15] << 4 | k1[x & 15];
}

/* neuro  primitive without rotation*/
inline static uint32_t f_neuro_g2(uint32_t x)
{
	x = k8[x >> 28 & 15] << 28 | k7[x >> 24 & 15] << 24 |
		k6[x >> 20 & 15] << 20 | k5[x >> 16 & 15] << 16 |
		k4[x >> 12 & 15] << 12 | k3[x >> 8 & 15] << 8 |
		k2[x >> 4 & 15] << 4 | k1[x & 15];
}

int magma_setkey(struct crypto_magma_ctx *magma_ctx, const uint8_t *key,
				 unsigned int key_len)
{
	if (key_len != 32)
		return 1;

	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	subkeys->k[0] = GETU32_BE(key);
	subkeys->k[1] = GETU32_BE(key + 4);
	subkeys->k[2] = GETU32_BE(key + 8);
	subkeys->k[3] = GETU32_BE(key + 12);
	subkeys->k[4] = GETU32_BE(key + 16);
	subkeys->k[5] = GETU32_BE(key + 20);
	subkeys->k[6] = GETU32_BE(key + 24);
	subkeys->k[7] = GETU32_BE(key + 28);

	return 0;
}

void magma_encrypt(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
				   const uint8_t *in)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);

	n2 ^= f(n1 + subkeys->k[0]);
	n1 ^= f(n2 + subkeys->k[1]);
	n2 ^= f(n1 + subkeys->k[2]);
	n1 ^= f(n2 + subkeys->k[3]);
	n2 ^= f(n1 + subkeys->k[4]);
	n1 ^= f(n2 + subkeys->k[5]);
	n2 ^= f(n1 + subkeys->k[6]);
	n1 ^= f(n2 + subkeys->k[7]);

	n2 ^= f(n1 + subkeys->k[0]);
	n1 ^= f(n2 + subkeys->k[1]);
	n2 ^= f(n1 + subkeys->k[2]);
	n1 ^= f(n2 + subkeys->k[3]);
	n2 ^= f(n1 + subkeys->k[4]);
	n1 ^= f(n2 + subkeys->k[5]);
	n2 ^= f(n1 + subkeys->k[6]);
	n1 ^= f(n2 + subkeys->k[7]);

	n2 ^= f(n1 + subkeys->k[0]);
	n1 ^= f(n2 + subkeys->k[1]);
	n2 ^= f(n1 + subkeys->k[2]);
	n1 ^= f(n2 + subkeys->k[3]);
	n2 ^= f(n1 + subkeys->k[4]);
	n1 ^= f(n2 + subkeys->k[5]);
	n2 ^= f(n1 + subkeys->k[6]);
	n1 ^= f(n2 + subkeys->k[7]);

	n2 ^= f(n1 + subkeys->k[7]);
	n1 ^= f(n2 + subkeys->k[6]);
	n2 ^= f(n1 + subkeys->k[5]);
	n1 ^= f(n2 + subkeys->k[4]);
	n2 ^= f(n1 + subkeys->k[3]);
	n1 ^= f(n2 + subkeys->k[2]);
	n2 ^= f(n1 + subkeys->k[1]);
	n1 ^= f(n2 + subkeys->k[0]);

#ifdef __LITTLE_ENDIAN
	n1 = BSWAP32(n1);
	n2 = BSWAP32(n2);
#endif
	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}

void _magma_encrypt(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
					const uint8_t *in)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);

	n2 ^= _f(n1 + subkeys->k[0]);
	n1 ^= _f(n2 + subkeys->k[1]);
	n2 ^= _f(n1 + subkeys->k[2]);
	n1 ^= _f(n2 + subkeys->k[3]);
	n2 ^= _f(n1 + subkeys->k[4]);
	n1 ^= _f(n2 + subkeys->k[5]);
	n2 ^= _f(n1 + subkeys->k[6]);
	n1 ^= _f(n2 + subkeys->k[7]);

	n2 ^= _f(n1 + subkeys->k[0]);
	n1 ^= _f(n2 + subkeys->k[1]);
	n2 ^= _f(n1 + subkeys->k[2]);
	n1 ^= _f(n2 + subkeys->k[3]);
	n2 ^= _f(n1 + subkeys->k[4]);
	n1 ^= _f(n2 + subkeys->k[5]);
	n2 ^= _f(n1 + subkeys->k[6]);
	n1 ^= _f(n2 + subkeys->k[7]);

	n2 ^= _f(n1 + subkeys->k[0]);
	n1 ^= _f(n2 + subkeys->k[1]);
	n2 ^= _f(n1 + subkeys->k[2]);
	n1 ^= _f(n2 + subkeys->k[3]);
	n2 ^= _f(n1 + subkeys->k[4]);
	n1 ^= _f(n2 + subkeys->k[5]);
	n2 ^= _f(n1 + subkeys->k[6]);
	n1 ^= _f(n2 + subkeys->k[7]);

	n2 ^= _f(n1 + subkeys->k[7]);
	n1 ^= _f(n2 + subkeys->k[6]);
	n2 ^= _f(n1 + subkeys->k[5]);
	n1 ^= _f(n2 + subkeys->k[4]);
	n2 ^= _f(n1 + subkeys->k[3]);
	n1 ^= _f(n2 + subkeys->k[2]);
	n2 ^= _f(n1 + subkeys->k[1]);
	n1 ^= _f(n2 + subkeys->k[0]);

#ifdef __LITTLE_ENDIAN
	n1 = BSWAP32(n1);
	n2 = BSWAP32(n2);
#endif
	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}

void magma_neuro_g0(struct crypto_magma_ctx *magma_ctx, uint8_t *out, const uint8_t *in, uint32_t *x, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0, buf2;

	buf = n1;
	n2 = n2 << 21 | n2 >> 11;
	buf = n2 ^ buf;

	*x = BSWAP32(n2);
	*y = BSWAP32(buf);

	buf = buf << 11 | buf >> (32 - 11);

	((uint32_t *)out)[0] = buf;
	((uint32_t *)out)[1] = n1;
}

void magma_neuro_g0_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{

	*y = n1 ^ n2;
}

void magma_neuro_g1(struct crypto_magma_ctx *magma_ctx, uint8_t *out, const uint8_t *in, uint32_t *x, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0, buf2;

	buf = f_neuro_g1(n1, subkeys->k[0]);
	n2 = n2 << 21 | n2 >> 11;
	buf = n2 ^ buf;

	*x = BSWAP32(n2);
	*y = BSWAP32(buf);

	buf = buf << 11 | buf >> (32 - 11);

	((uint32_t *)out)[0] = buf;
	((uint32_t *)out)[1] = n1;
}

void magma_neuro_g1_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t buf = 0;

	//buf = f_neuro_g1(n1, 0);
	buf = f_neuro_g2(n1);
	*y = buf ^ n2;
}

void magma_neuro_g2(struct crypto_magma_ctx *magma_ctx, uint8_t *out, const uint8_t *in, uint32_t *x, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0, buf2;

	buf = f_neuro_g2(n1 + subkeys->k[0]);
	n2 = n2 << 21 | n2 >> 11;
	buf = n2 ^ buf;

	*x = BSWAP32(n2);
	*y = BSWAP32(buf);

	buf = buf << 11 | buf >> (32 - 11);

	((uint32_t *)out)[0] = buf;
	((uint32_t *)out)[1] = n1;
}

void magma_neuro_g2_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t buf = 0;

	buf = f_neuro_g1(n1, subkeys->k[0]);
	*y = buf ^ n2;
}

void magma_neuro_g3_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t buf = 0;

	buf = f_neuro_g2(n1 + subkeys->k[0]);
	*y = buf ^ n2;
}

void magma_neuro_g4_4_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint8_t buf = (uint8_t)(n1 & 0xF);

	buf += subkeys->k[0] & 0xF;
	*y = buf & 0xF;
}

void magma_neuro_g4_8_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint8_t buf = (uint8_t)(n1 & 0xFF);

	buf += subkeys->k[0] & 0xFF;
	*y = buf;
}

void magma_neuro_g4_16_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint16_t buf = (uint16_t)(n1 & 0xFFFF);
	uint16_t key = (subkeys->k[0] << 8) | subkeys->k[1];

	buf += key;
	*y = buf;
}

void magma_neuro_g4_32_primitive(struct crypto_magma_ctx *magma_ctx, const uint32_t n1, const uint32_t n2, uint32_t *y)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t buf = n1;
	uint32_t key = GETU32_BE(subkeys->k);

	buf += key;
	*y = buf;
}

void magma_it(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
			  const uint8_t *in, uint8_t iter)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0;

	if (iter >= 23)
	{
		buf = n2 ^ f(n1 + subkeys->k[7 - iter % 8]);
	}
	else
	{
		buf = n2 ^ f(n1 + subkeys->k[iter % 8]);
	}
	n2 = n1;
	n1 = buf;
	buf = 0;

	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}

void _magma_it(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
			   const uint8_t *in, uint8_t iter)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0;

	if (iter >= 23)
	{
		buf = n2 ^ _f(n1 + subkeys->k[7 - iter % 8]);
	}
	else
	{
		buf = n2 ^ _f(n1 + subkeys->k[iter % 8]);
	}

	n2 = n1;
	n1 = buf;
	buf = 0;

	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}

void magma_it_n(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
				const uint8_t *in, uint8_t num)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);
	uint32_t buf = 0;

	for (uint8_t it = 0; it < num; ++it)
	{

		buf = n2 ^ f(n1 + subkeys->k[(it >= 23) ? (7 - it % 8) : (it % 8)]);
		n2 = n1;
		n1 = buf;
	}

	if (num % 2)
	{
		n1 ^= n2;
		n2 ^= n1;
		n1 ^= n2;
	}

	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}

void magma_decrypt(struct crypto_magma_ctx *magma_ctx, uint8_t *out,
				   const uint8_t *in)
{
	magma_subkeys *subkeys = keys_magma_ctx(magma_ctx);
	uint32_t n2 = GETU32_BE(in);
	uint32_t n1 = GETU32_BE(in + 4);

	n2 ^= f(n1 + subkeys->k[0]);
	n1 ^= f(n2 + subkeys->k[1]);
	n2 ^= f(n1 + subkeys->k[2]);
	n1 ^= f(n2 + subkeys->k[3]);
	n2 ^= f(n1 + subkeys->k[4]);
	n1 ^= f(n2 + subkeys->k[5]);
	n2 ^= f(n1 + subkeys->k[6]);
	n1 ^= f(n2 + subkeys->k[7]);

	n2 ^= f(n1 + subkeys->k[7]);
	n1 ^= f(n2 + subkeys->k[6]);
	n2 ^= f(n1 + subkeys->k[5]);
	n1 ^= f(n2 + subkeys->k[4]);
	n2 ^= f(n1 + subkeys->k[3]);
	n1 ^= f(n2 + subkeys->k[2]);
	n2 ^= f(n1 + subkeys->k[1]);
	n1 ^= f(n2 + subkeys->k[0]);

	n2 ^= f(n1 + subkeys->k[7]);
	n1 ^= f(n2 + subkeys->k[6]);
	n2 ^= f(n1 + subkeys->k[5]);
	n1 ^= f(n2 + subkeys->k[4]);
	n2 ^= f(n1 + subkeys->k[3]);
	n1 ^= f(n2 + subkeys->k[2]);
	n2 ^= f(n1 + subkeys->k[1]);
	n1 ^= f(n2 + subkeys->k[0]);

	n2 ^= f(n1 + subkeys->k[7]);
	n1 ^= f(n2 + subkeys->k[6]);
	n2 ^= f(n1 + subkeys->k[5]);
	n1 ^= f(n2 + subkeys->k[4]);
	n2 ^= f(n1 + subkeys->k[3]);
	n1 ^= f(n2 + subkeys->k[2]);
	n2 ^= f(n1 + subkeys->k[1]);
	n1 ^= f(n2 + subkeys->k[0]);

#ifdef __LITTLE_ENDIAN
	n1 = BSWAP32(n1);
	n2 = BSWAP32(n2);
#endif
	((uint32_t *)out)[0] = n1;
	((uint32_t *)out)[1] = n2;
}
