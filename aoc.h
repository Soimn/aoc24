#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <intrin.h>

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

#define S8_MIN  0x80
#define S16_MIN 0x8000
#define S32_MIN 0x80000000
#define S64_MIN 0x8000000000000000LL
#define S8_MAX  0x7F
#define S16_MAX 0x7FFF
#define S32_MAX 0x7FFFFFFF
#define S64_MAX 0x7FFFFFFFFFFFFFFFLL

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#define U8_MAX  0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL

typedef s64 smm;
typedef u64 umm;

#define SMM_MIN S64_MIN
#define SMM_MAX S64_MAX
#define UMM_MAX U64_MAX

typedef u8 bool;
#define true 1
#define false 0

typedef float f32;
typedef double f64;

void
AssertHandler(char* file, int line, char* expr)
{
	fprintf(stderr, "ASSERTION FAILED\n%s(%d): %s\n", file, line, expr);
	exit(-1);
}

int IsDebuggerPresent();
#define ASSERT(EX) ((EX) ? 1 : (IsDebuggerPresent() ? *(volatile int*)0 = 0 : AssertHandler(__FILE__, __LINE__, #EX)))
#define NOT_IMPLEMENTED ASSERT(!"NOT_IMPLEMENTED")

#define ARRAY_LEN(A) (sizeof(A)/sizeof(0[A]))

#define ALIGNOF(T) _alignof(T)

typedef struct String
{
	u8* data;
	u32 len;
} String;

#define STRING(S) (String){ .data = (u8*)(S), .len = sizeof(S)-1 }
#define MS_STRING(S) { .data = (u8*)(S), .len = sizeof(S)-1 }

bool
String_Match(String s0, String s1)
{
	bool result = (s0.len == s1.len);

	for (umm i = 0; result && i < s0.len; ++i) result = (s0.data[i] == s1.data[i]);

	return result;
}

bool
String_MatchPrefix(String s0, String s1)
{
	bool result = (s0.len >= s1.len);

	for (umm i = 0; result && i < s1.len; ++i) result = (s0.data[i] == s1.data[i]);

	return result;
}

bool
Char_IsAlpha(u8 c)
{
	return ((u8)((c&0xDF)-'A') <= (u8)('Z' - 'A'));
}

bool
Char_IsDigit(u8 c)
{
	return ((u8)(c-'0') < (u8)10);
}

u8
Char_UncheckedToUpper(u8 c)
{
	return c&0xDF;
}

void
Copy(void* dst, void* src, umm len)
{
  u8* bdst = dst;
  u8* bsrc = src;
  for (umm i = 0; i < len; ++i) bdst[i] = bsrc[i];
}

bool
LoadInput(int argc, char** argv, String* input)
{
	bool succeeded = false;

	if (argc != 2) fprintf(stderr, "Invalid Arguments. Expected: %s input_file\n", argv[0]);
	else
	{
		u8* input_file_content = 0;
		u32 input_file_size    = 0;

		FILE* input_file = 0;
		if (fopen_s(&input_file, argv[1], "rb") == 0)
		{
			struct __stat64 input_stat;
			if (_stat64(argv[1], &input_stat) == 0 && input_stat.st_size < U32_MAX)
			{
				input_file_size    = (u32)input_stat.st_size;
				input_file_content = malloc(input_file_size + 1);

				if (input_file_content != 0 && fread(input_file_content, 1, input_file_size, input_file) == input_file_size)
				{
					input_file_content[input_file_size] = 0;
					succeeded = true;
				}
			}

			fclose(input_file);
		}

		if (!succeeded) fprintf(stderr, "Failed to read input file\n");
		else
		{
			*input = (String){
				.data = input_file_content,
				.len  = input_file_size,
			};
		}
	}

	return succeeded;
}

void
Advance(String* input, smm amount)
{
	if (amount > input->len) amount = input->len;
	input->data += amount;
	input->len  -= (u32)amount;
}

bool
EatSpace(String* input)
{
	u32 i = 0;
	while (i < input->len && (input->data[i] == ' ' || input->data[i] == '\t' || input->data[i] == '\v')) ++i;

	input->data += i;
	input->len  -= i;

	return (i > 0);
}

bool
EatWhitespace(String* input)
{
	u32 i = 0;
	while (i < input->len && (u8)(input->data[i]-1) < (u8)0x20) ++i;

	input->data += i;
	input->len  -= i;

	return (i > 0);
}

bool
EatChar(String* input, u8 c)
{
	bool result = false;

	if (input->len != 0 && input->data[0] == c)
	{
		result = true;
		input->data += 1;
		input->len  -= 1;
	}

	return result;
}

bool
EatUMM(String* input, umm* result)
{
	u32 i = 0;
	umm value = 0;
	for (; i < input->len && Char_IsDigit(input->data[i]); ++i)
	{
		umm val_x_10     = value*10;
		umm val_x_10_p_d = val_x_10 + (input->data[i]&0xF);
		ASSERT(val_x_10 >= value && val_x_10_p_d >= val_x_10);

		value = val_x_10_p_d;
	}

	*result = value;

	input->data += i;
	input->len  -= i;

	return (i > 0);
}

bool
EatSMM(String* input, smm* result)
{
	bool is_neg = EatChar(input, '-');
	ASSERT(input->len != 0 && Char_IsDigit(input->data[0]));

	umm unsigned_result;
	bool ate = EatUMM(input, &unsigned_result);

	ASSERT((smm)unsigned_result >= 0);

	*result = (is_neg ? -(smm)unsigned_result : (smm)unsigned_result);

	return ate;
}

bool
EatPrefix(String* input, String prefix)
{
	bool result = false;

	if (String_MatchPrefix(*input, prefix))
	{
		result = true;
		input->data += prefix.len;
		input->len  -= prefix.len;
	}

	return result;
}

// Taken from http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-1a
// Licensed under CC0 Public Domain
u64
FNV1A(String string)
{
  u64 hash = 14695981039346656037;

  for (umm i = 0; i < string.len; ++i)
  {
    hash ^= string.data[i];
    hash *= 1099511628211;
  }

  return hash;
}

typedef union u128
{
	struct { u64 lo; u64 hi; };
	u32 e[4];
} u128;

#define U128(LO, HI) (u128){ (LO), (HI) }

bool
U128_IsSet(u128 n, umm bit)
{
	bool result;

	ASSERT(bit < 128);
	if (bit < 64) result = (n.lo >> bit) & 1;
	else          result = (n.hi >> (bit&63)) & 1;

	return result;
}

u128
U128_Set(u128 n, umm bit)
{
	ASSERT(bit < 128);
	if (bit < 64) n.lo |= 1ULL << bit;
	else          n.hi |= 1ULL << (bit&63);

	return n;
}

bool
U128_Equal(u128 a, u128 b)
{
	return (a.lo == b.lo && a.hi == b.hi);
}

u128
U128_And(u128 a, u128 b)
{
	return (u128){
		.lo = a.lo & b.lo,
		.hi = a.hi & b.hi,
	};
}

u128
U128_Or(u128 a, u128 b)
{
	return (u128){
		.lo = a.lo | b.lo,
		.hi = a.hi | b.hi,
	};
}

u128
U128_Xor(u128 a, u128 b)
{
	return (u128){
		.lo = a.lo ^ b.lo,
		.hi = a.hi ^ b.hi,
	};
}

typedef struct V2S
{
  s32 x;
  s32 y;
} V2S;

#define V2S(X, Y) (V2S){ .x = (X), .y = (Y) }

V2S
V2S_Add(V2S a, V2S b)
{
  return V2S(a.x + b.x, a.y + b.y);
}

V2S
V2S_Sub(V2S a, V2S b)
{
  return V2S(a.x - b.x, a.y - b.y);
}

bool
V2S_Equal(V2S a, V2S b)
{
  return (a.x == b.x && a.y == b.y);
}

bool
V2S_InBounds(V2S v, s32 lo_incl, s32 hi_excl)
{
  return (v.x >= lo_incl && v.x < hi_excl && v.y >= lo_incl && v.y < hi_excl);
}
