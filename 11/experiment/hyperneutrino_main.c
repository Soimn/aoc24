// NOTE: Implementation inspired by HyperNeutrino (https://www.youtube.com/watch?v=pVfsmQSlVOQ)

#include "../aoc.h"

#include <windows.h>

// -----------------------------------------------------------------------
//
/*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#include <stdint.h>

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and 
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state. */

static uint64_t x; /* The state can be seeded with any value. */

uint64_t next() {
	uint64_t z = (x += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}
//
// -----------------------------------------------------------------------


typedef struct Memo_Entry
{
  u64 stone;
  s64 step;
  u64 result;
} Memo_Entry;

#define MEMO_TABLE_SIZE_LG2 26
#define MEMO_TABLE_MASK ((1ULL << MEMO_TABLE_SIZE_LG2) - 1)
Memo_Entry* MemoTable   = 0;
umm MemoTableEntryCount = 0;

u64
Memo__Idx(u64 stone, s64 step)
{
  x = stone * step;
  return next() & MEMO_TABLE_MASK;
}

bool
Memo_Get(u64 stone, s64 step, u64* result)
{
  bool found = false;

  u64 idx      = Memo__Idx(stone, step);
  u64 idx_step = 1;

  while (MemoTable[idx].result != 0)
  {
    if (MemoTable[idx].stone == stone && MemoTable[idx].step == step)
    {
      *result = MemoTable[idx].result;
      found = true;
      break;
    }
    else
    {
      idx = (idx + idx_step++) & MEMO_TABLE_MASK;
    }
  }

  return found;
}

void
Memo_Put(u64 stone, s64 step, u64 result)
{
  u64 idx      = Memo__Idx(stone, step);
  u64 idx_step = 1;

  while (MemoTable[idx].result != 0)
  {
    if (MemoTable[idx].stone == stone && MemoTable[idx].step == step) break;
    else
    {
      idx = (idx + idx_step++) & MEMO_TABLE_MASK;
    }
  }

  if (MemoTable[idx].result == 0)
  {
    MemoTable[idx] = (Memo_Entry){
      .stone  = stone,
      .step   = step,
      .result = result,
    };

    MemoTableEntryCount += 1;
  }
}

u64
Count(u64 stone, s64 step)
{
  u64 result;

  if (!Memo_Get(stone, step, &result))
  {
    ASSERT(step >= 0);
    if      (step == 0)  result = 1;
    else if (stone == 0) result = Count(1, step-1);
    else
    {
      umm num_digits = 1;
      for (u64 x = stone/10; x != 0; x/= 10) ++num_digits;

      if (num_digits % 2 == 0)
      {
        u64 mult = 1;
        for (umm k = 0; k < num_digits/2; ++k) mult *= 10;

        result = Count(stone/mult, step-1) + Count(stone%mult, step-1);
      }
      else
      {
        ASSERT(stone <= U64_MAX/2024);
        result = Count(stone*2024, step-1);
      }
    }

    Memo_Put(stone, step, result);
  }

  return result;
}

int
main(int argc, char** argv)
{
  LARGE_INTEGER start_t;
  QueryPerformanceCounter(&start_t);

	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

  umm iterations = 5000;
	umm result     = 0;

  MemoTable = VirtualAlloc(0, (1ULL << MEMO_TABLE_SIZE_LG2)*sizeof(Memo_Entry), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

  for (;;)
  {
    umm val;
    if (!EatUMM(&input, &val)) break;
    else
    {
      result += Count(val, iterations);
      EatSpace(&input);
    }
  }

  LARGE_INTEGER end_t;
  QueryPerformanceCounter(&end_t);

  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);

  printf("Result of %llu iterations: %llu\n", iterations, result);
  printf("Took %llu ms\n", ((end_t.QuadPart - start_t.QuadPart)*1000)/freq.QuadPart);

  printf("Entries: %llu\n", MemoTableEntryCount);

	return 0;
}
