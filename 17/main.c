#include "../aoc.h"

enum
{
  ADV = 0,
  BXL = 1,
  BST = 2,
  JNZ = 3,
  BXC = 4,
  OUT = 5,
  BDV = 6,
  CDV = 7,
};

enum
{
  REG_A = 4,
  REG_B = 5,
  REG_C = 6,
};

typedef struct Requirement
{
  u128 vals[8];
  u128 masks[8];
  umm len;
} Requirement;

u32 iii = 0;

u128
Satisfy(umm* program, umm program_len, Requirement* req, umm i, u128 n, u128 mask)
{
  u128 result = U128(0, 0);

  if (i == (umm)-1)
  {
    iii += 1;
  }
  else
  {
    for (umm j = 0; j < req[i].len; ++j)
    {
      u128 x     = U128_Or(n, req[i].vals[j]);
      u128 xmask = U128_Or(mask, req[i].masks[j]);

      if (!U128_Equal(U128_And(x, xmask), req[i].vals[j]) || !U128_Equal(U128_And(x, mask), n)) continue;

      result = Satisfy(program, program_len, req, i-1, x, xmask);
      if (result.lo != 0 || result.hi != 0) break;
    }
  }

  return result;
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

  umm init_a, init_b, init_c;
  bool ok = true;
  ok = ok && EatPrefix(&input, STRING("Register A: "));
  ok = ok && EatUMM(&input, &init_a);
  ok = ok && EatPrefix(&input, STRING("\r\nRegister B: "));
  ok = ok && EatUMM(&input, &init_b);
  ok = ok && EatPrefix(&input, STRING("\r\nRegister C: "));
  ok = ok && EatUMM(&input, &init_c);
  ok = ok && EatPrefix(&input, STRING("\r\n\r\nProgram: "));
  ASSERT(ok);

  umm program[31];
  umm program_len = 0;

  for (;;)
  {
    ASSERT(program_len < ARRAY_LEN(program));
    ok = EatUMM(&input, &program[program_len++]);
    ASSERT(ok);

    if (EatChar(&input, ',')) continue;
    else                      break;
  }

  EatWhitespace(&input);
  ASSERT(input.len == 0);

  umm regs[8] = { 0, 1, 2, 3, init_a, init_b, init_c, 0 };
  umm output[1024];
  umm output_cursor = 0;

  ASSERT(program_len%2 == 0);
  for (umm ip = 0; ip < program_len; ip += 2)
  {
    umm instruction = program[ip];
    umm operand     = program[ip+1];

    switch (instruction)
    {
      case ADV: regs[REG_A] = regs[REG_A] / (1ULL << regs[operand]); break;
      case BDV: regs[REG_B] = regs[REG_A] / (1ULL << regs[operand]); break;
      case CDV: regs[REG_C] = regs[REG_A] / (1ULL << regs[operand]); break;
      case BXL: regs[REG_B] = regs[REG_B] ^ operand;                 break;
      case BXC: regs[REG_B] = regs[REG_B] ^ regs[REG_C];             break;
      case BST: regs[REG_B] = regs[operand] & 7;                     break;
      case JNZ: ip = (regs[REG_A] == 0 ? ip : operand - 2);          break;
      case OUT:
      {
        ASSERT(output_cursor < ARRAY_LEN(output));
        output[output_cursor++] = regs[operand] & 7;
      } break;
    }
  }

  printf("Part 1 result: ");
  for (umm i = 0; i < output_cursor; ++i) printf("%llu%c", output[i], (i == output_cursor-1 ? '\n' : ','));

  umm part2_result = 0;

  Requirement req[ARRAY_LEN(program)] = {0};

  // (a ^ (a >> (~a & 7))) & 7
  for (umm i = 0; i < program_len; ++i)
  {
    umm e = program[i];
    // e = (x ^ (x >> (~x & 7))) & 7
    // e = (x_1 ^ x_2) & 7
    // x_2 = x_1 >> (~x_1 & 7)

    for (umm x_1 = 0; x_1 < 8; ++x_1)
    {
      for (umm x_2 = 0; x_2 < 8; ++x_2)
      {
        umm upper_mask = 7ULL << (~x_1 & 7);

        if ((x_1 ^ x_2) != e) continue;
        if ((~x_1 & 7) < 3 && (x_1 & upper_mask) != ((x_2 << (~x_1 & 7)) & upper_mask)) continue;

        req[i].vals[req[i].len]  = U128_Shl(U128(x_1 | (x_2 << (~x_1 & 7)), 0), i*3);
        req[i].masks[req[i].len] = U128_Shl(U128(upper_mask | 7, 0), i*3);
        req[i].len += 1;
      }
    }
  }

  Satisfy(program, program_len, req, program_len-1, U128(0, 0), U128(0, 0));
part2_result = iii;
  printf("Part 2 result: %llu\n", part2_result);

	return 0;
}
