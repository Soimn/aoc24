// NOTE: Implementation inspired by William Y. Feng (https://www.youtube.com/watch?v=FLl7s0uPbOo)

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

#define DICT_SIZE_LG2 13
#define DICT_MASK ((1ULL << DICT_SIZE_LG2)-1)

typedef struct Dict_Entry
{
  u64 stone;
  u64 count;
} Dict_Entry;

Dict_Entry* OldFrequencyDict = 0;
Dict_Entry* NewFrequencyDict = 0;
umm MaxEntryCount = 0;

void
FreqDict_Add(Dict_Entry* dict, u64* entry_count, u64 stone, u64 count)
{
  x = stone;
  u64 idx  = next() & DICT_MASK;
  u64 step = 1;

  while (dict[idx].count == 0)
  {
    if (dict[idx].stone == stone) break;
    else
    {
      idx = (idx + step++) & DICT_MASK;
    }
  }

  if (dict[idx].count == 0)
  {
    dict[idx].stone = stone;
    *entry_count += 1;
  }

  dict[idx].count += count;
}

int
main(int argc, char** argv)
{
  LARGE_INTEGER start_t;
  QueryPerformanceCounter(&start_t);

	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

  OldFrequencyDict = VirtualAlloc(0, (1ULL << DICT_SIZE_LG2)*sizeof(Dict_Entry), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  NewFrequencyDict = VirtualAlloc(0, (1ULL << DICT_SIZE_LG2)*sizeof(Dict_Entry), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

  umm iterations = 5000;
	umm result     = 0;

  for (;;)
  {
    umm val;
    if (!EatUMM(&input, &val)) break;
    else
    {
      FreqDict_Add(OldFrequencyDict, val, 1);
      EatSpace(&input);
    }
  }

  for (umm i = 0; i < iterations; ++i)
  {
    for (umm j = 0; j < 1ULL << DICT_SIZE_LG2; ++j)
    {
      if (OldFrequencyDict[j].count == 0) continue;


    }

    Dict_Entry* tmp = OldFrequencyDict;
    OldFrequencyDict = NewFrequencyDict;
    NewFrequencyDict = tmp;
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
