#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "magma.h"
#include "generator.h"

#define gen_n(n)

void generate_random_key(uint8_t *key)
{
	srand(time(NULL));
	for (uint8_t i = 0; i < 32; ++i)
	{
		key[i] = 0x86; //rand();
	}
}

void print_models()
{
	printf("Models:\n\n");
	for (uint8_t i = 0; i < sizeof(models) / sizeof(models[0]); ++i)
	{
		printf("\t%s\t-\t%s (default files: %s, %s)\n",
			   models[i].name, models[i].description,
			   models[i].default_input, models[i].default_output);
	}
	printf("\n");
}

void print_generators()
{
	printf("Generators:\n\n");
	for (uint8_t i = 0; i < sizeof(generators) / sizeof(generators[0]); ++i)
	{
		printf("\t%s\t-\t%s\n", generators[i].name, generators[i].description);
	}
	printf("\n");
}

generator_type_t *get_generator_by_name(const char *name)
{
	size_t size = sizeof(generators) / sizeof(generators[0]);
	generator_type_t *list = generators;

	for (uint8_t i = 0; i < size; ++i)
	{
		if (strcmp(list[i].name, name) == 0)
		{
			return &list[i];
		}
	}
	return NULL;
}

model_type_t *get_model_by_name(const char *name)
{
	size_t size = sizeof(models) / sizeof(models[0]);
	model_type_t *list = models;

	for (uint8_t i = 0; i < size; ++i)
	{
		if (strcmp(list[i].name, name) == 0)
		{
			return &list[i];
		}
	}
	return NULL;
}

/* Consecutive generator */
void iterate_generator(crypto_tfm *ctx, uint64_t size,
					   const char *filename_x, const char *filename_y,
					   void (*generator)(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in))
{
	FILE *out_file_x, *out_file_y;
	uint64_t in;

	out_file_x = fopen(filename_x, "wb");
	out_file_y = fopen(filename_y, "wb");

	if (out_file_x == NULL || out_file_y == NULL)
	{
		printf("error: cannot open file!\n");
		exit(1);
	}

	for (in = 0; in < size; ++in)
	{
		generator(ctx, out_file_x, out_file_y, in);
	}

	fclose(out_file_x);
	fclose(out_file_y);
}

/* Consecutive generator */
void iterate_parallel_generator(crypto_tfm *ctx, uint64_t size,
								const char *filename_x, const char *filename_y,
								void (*generator)(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in))
{

	FILE *out_file_x, *out_file_y;
	uint64_t in;
	uint32_t n1, n2;

	out_file_x = fopen(filename_x, "wb");
	out_file_y = fopen(filename_y, "wb");

	if (out_file_x == NULL || out_file_y == NULL)
	{
		printf("error: cannot open file!\n");
		exit(1);
	}

	for (n1 = 0; n1 < size; ++n1)
	{
		for (n2 = 0; n2 < size; ++n2)
		{
			in = 0;
			// ((uint32_t *)&in)[0] = SWAP_32(n1);
			// ((uint32_t *)&in)[1] = SWAP_32(n2);
			((uint8_t *)&in)[0] = n1;
			((uint8_t *)&in)[1] = n2;
			generator(ctx, out_file_x, out_file_y, in);
		}
	}

	fclose(out_file_x);
	fclose(out_file_y);
}

/* Random generator */
void random_generator(crypto_tfm *ctx, uint64_t size,
					  const char *filename_x, const char *filename_y,
					  void (*generator)(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in))
{
	FILE *out_file_x, *out_file_y;
	uint64_t in;

	out_file_x = fopen(filename_x, "wb");
	out_file_y = fopen(filename_y, "wb");

	if (out_file_x == NULL || out_file_y == NULL)
	{
		printf("error: cannot open file!\n");
		exit(1);
	}

	srand(time(NULL));

	for (uint64_t i = 0; i < size; ++i)
	{
		in = 0;
		((uint32_t *)&in)[0] = rand();
		((uint32_t *)&in)[1] = rand();
		generator(ctx, out_file_x, out_file_y, in);
	}

	fclose(out_file_x);
	fclose(out_file_y);
}

/* Consecutive generator with random order */
void random_iterate_generator(crypto_tfm *ctx, uint64_t size,
							  const char *filename_x, const char *filename_y,
							  void (*generator)(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in))
{
	FILE *out_file_x, *out_file_y;
	uint64_t in, value, size_x, *memory;

	out_file_x = fopen(filename_x, "wb");
	out_file_y = fopen(filename_y, "wb");

	if (out_file_x == NULL || out_file_y == NULL)
	{
		printf("error: cannot open file!\n");
		exit(1);
	}

	srand(time(NULL));
	memory = malloc(sizeof(uint64_t) * size);
	if (memory == NULL)
	{
		printf("error: cannot malloc!\n");
		exit(1);
	}

	for (uint64_t i = 0; i < size; i++)
	{
		memory[i] = i;
	}

	size_x = size;
	for (uint64_t i = 0; i < size - 1; ++i)
	{
		in = 0;
		value = 0;
		((uint32_t *)&in)[0] = rand();
		((uint32_t *)&in)[1] = rand();
		value = memory[in % size_x];
		//printf("%ld\n", value);
		memory[in % size_x] = memory[size_x - 1];
		size_x--;

		generator(ctx, out_file_x, out_file_y, value);
	}

	free(memory);
	fclose(out_file_x);
	fclose(out_file_y);
}

// /* Consecutive generator with random order*/
// void random_iterate_parallel_generator(crypto_tfm *ctx, uint64_t size,
// 					  		  		   const char *filename_x, const char *filename_y,
// 					  		           void (*generator)(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in))
// {
// 	FILE *out_file_x, *out_file_y;
// 	uint64_t in, value, size_x, *memory1, *memory2;

// 	out_file_x = fopen(filename_x, "wb");
// 	out_file_y = fopen(filename_y, "wb");

// 	if (out_file_x == NULL || out_file_y == NULL)
// 	{
// 		printf("error: cannot open file!\n");
// 		exit(1);
// 	}

// 	srand(time(NULL));
// 	memory2 = malloc(sizeof(uint64_t) * size);
// 	memory1 = malloc(sizeof(uint64_t) * size);
// 	if (memory1 == NULL || memory2 == NULL)
// 	{
// 		printf("error: cannot malloc!\n");
// 		exit(1);
// 	}

// 	for (uint64_t i = 0; i < size; i++)
// 	{
// 		memory1[i] = i;
// 	}

// 	for (uint64_t i = 0; i < size; i++)
// 	{
// 		memory2[i] = i;
// 	}

// 	size_x = size;
// 	for (uint64_t i = 0; i < size - 1; ++i)
// 	{
// 		in = 0;
// 		value = 0;
// 		((uint32_t *) &in)[0] = rand();
// 		((uint32_t *) &in)[1] = rand();
// 		value = memory1[in % size_x];
// 		//printf("%ld\n", value);
// 		memory1[in % size_x] = memory1[size_x - 1];
// 		size_x--;

// 		generator(ctx, out_file_x, out_file_y, value);
// 	}

// 	fclose(out_file_x);
// 	fclose(out_file_y);
// }

/* Generator for G3 model/1-round of GOST */
void round_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;

	magma_it(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, 0);
	fwrite((uint8_t *)&in, sizeof(uint8_t), 8, out_file_x);
	fwrite((uint8_t *)&out, sizeof(uint8_t), 8, out_file_y);
}

/* Generator for N-round GOST */
void n_round_generator_(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in, uint8_t n)
{
	uint64_t out;

	magma_it_n(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, n);
	fwrite((uint8_t *)&in, sizeof(uint8_t), 8, out_file_x);
	fwrite((uint8_t *)&out, sizeof(uint8_t), 8, out_file_y);
}

/* Generator for 2-round GOST */
void n_round_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;

	magma_it_n(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, 2);
	fwrite((uint8_t *)&in, sizeof(uint8_t), 8, out_file_x);
	fwrite((uint8_t *)&out, sizeof(uint8_t), 8, out_file_y);
}

/* Generator for G2 model */
void primitive_g2_generator_(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{

	uint64_t out;
	uint32_t x, y;
	magma_neuro_g2(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, &x, &y);
	uint8_t p = 0;
	//for (uint8_t p = 0; p < 8; ++p)
	//{
	uint8_t var = ((uint8_t *)&in)[4 + p / 2];
	var = p % 2 ? (var & 0xF) : (var >> 4 & 0xF);

	uint8_t var1 = ((uint8_t *)&x)[p / 2];
	var1 = p % 2 ? (var1 & 0xF) : (var1 >> 4 & 0xF);
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[p / 2];
	var2 = p % 2 ? (var2 & 0xF) : (var2 >> 4 & 0xF);

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
	//}
}

/* Generator for G1 model */
void primitive_g1_generator_(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t x, y;

	magma_neuro_g1(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, &x, &y);

	// printf("%llx\n", x);
	// printf("%llx\n", y);
	// printf("%llx\n", in);
	// printf("%llx\n", out);

	uint8_t p = 0;
	//for (uint8_t p = 0; p < 8; ++p)
	//{
	uint8_t var = ((uint8_t *)&in)[4 + p / 2];
	var = p % 2 ? (var & 0xF) : (var >> 4 & 0xF);

	uint8_t var1 = ((uint8_t *)&x)[p / 2];
	var1 = p % 2 ? (var1 & 0xF) : (var1 >> 4 & 0xF);
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[p / 2];
	var2 = p % 2 ? (var2 & 0xF) : (var2 >> 4 & 0xF);

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
	//}
}

/* Generator for G0 model */
void primitive_g0_generator_(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t x, y;

	magma_neuro_g0(ctx->magma, (uint8_t *)&out, (uint8_t *)&in, &x, &y);

	// printf("%llx\n", x);
	// printf("%llx\n", y);
	// printf("%llx\n", in);
	// printf("%llx\n", out);

	uint8_t p = 0;
	//for (uint8_t p = 0; p < 8; ++p)
	//{
	uint8_t var = ((uint8_t *)&in)[4 + p / 2];
	var = p % 2 ? (var & 0xF) : (var >> 4 & 0xF);

	uint8_t var1 = ((uint8_t *)&x)[p / 2];
	var1 = p % 2 ? (var1 & 0xF) : (var1 >> 4 & 0xF);
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[p / 2];
	var2 = p % 2 ? (var2 & 0xF) : (var2 >> 4 & 0xF);

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
	//}
}

/* Generator for G0 model */
void primitive_g0_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g0_primitive(ctx->magma, n1, n2, &y);

	uint8_t var = ((uint8_t *)&n1)[0];
	var = var & 0xF;

	uint8_t var1 = ((uint8_t *)&n2)[0];
	var1 = var1 & 0xF;
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[0];
	var2 = var2 & 0xF;

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
}

/* Generator for G1 model */
void primitive_g1_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g1_primitive(ctx->magma, n1, n2, &y);

	uint8_t var = ((uint8_t *)&n1)[0];
	var = var & 0xF;

	uint8_t var1 = ((uint8_t *)&n2)[0];
	var1 = var1 & 0xF;
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[0];
	var2 = var2 & 0xF;

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
}

/* Generator for G2 model */
void primitive_g2_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g2_primitive(ctx->magma, n1, n2, &y);

	uint8_t var = ((uint8_t *)&n1)[0];
	var = var & 0xF;

	uint8_t var1 = ((uint8_t *)&n2)[0];
	var1 = var1 & 0xF;
	var = (var << 4) | var1;

	uint8_t var2 = ((uint8_t *)&y)[0];
	var2 = var2 & 0xF;

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
}

/* Generator for G3 model */
void primitive_g3_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g3_primitive(ctx->magma, n1, n2, &y);
	out = SWAP_32(y);

	fwrite((uint8_t *)&in, sizeof(uint8_t), 8, out_file_x);
	fwrite((uint8_t *)&out, sizeof(uint8_t), 8, out_file_y);
}

/* Generator for Feistel model */
void feistel_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint16_t out;

	feistel_generate(ctx->feistel, (int8_t *)&out, (int8_t *)&in);
	//out = SWAP_32(y);

	fwrite((uint8_t *)&in, sizeof(uint8_t), 2, out_file_x);
	fwrite((uint8_t *)&out, sizeof(uint8_t), 2, out_file_y);
}

/* Generator for G4 model */
void primitive_g4_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g4_primitive(ctx->magma, n1, n2, &y);

	uint8_t var = ((uint8_t *)&n1)[0];
	var = var & 0xF;

	uint8_t var2 = ((uint8_t *)&y)[0];
	var2 = var2 & 0xF;

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
}

/* Generator for G4 model */
void primitive_g4l_generator(crypto_tfm *ctx, FILE *out_file_x, FILE *out_file_y, uint64_t in)
{
	uint64_t out;
	uint32_t y;
	uint32_t n1 = GETU32_BE(((uint8_t *)&in));
	uint32_t n2 = GETU32_BE(((uint8_t *)&in) + 4);

	magma_neuro_g4l_primitive(ctx->magma, n1, n2, &y);

	uint8_t var = ((uint8_t *)&n1)[0];
	var = var & 0xFF;

	uint8_t var2 = ((uint8_t *)&y)[0];
	var2 = var2 & 0xFF;

	fwrite(&var, sizeof(uint8_t), 1, out_file_x);
	fwrite(&var2, sizeof(uint8_t), 1, out_file_y);
}
