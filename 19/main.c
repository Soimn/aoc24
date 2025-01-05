#include "../aoc.h"

typedef struct Memo
{
  u64 hash;
  String pattern;
  umm result;
} Memo;

#define MEMOS_SIZE_LG2 20
#define MEMOS_MASK ((1ULL << MEMOS_SIZE_LG2) - 1)
Memo Memos[1ULL << MEMOS_SIZE_LG2] = {0};
umm MemosFill = 0;

bool
Memos_Get(String pattern, umm* result)
{
  u64 hash = FNV1A(pattern);
  u64 idx  = (hash == 0 ? 1 : hash & MEMOS_MASK);
  u64 step = 1;

  while (Memos[idx].hash != 0)
  {
    if (Memos[idx].hash == hash && String_Match(Memos[idx].pattern, pattern))
    {
      break;
    }

    idx = (idx + step++) & MEMOS_MASK;
  }

  *result = Memos[idx].result;

  return (Memos[idx].hash != 0);
}

void
Memos_Put(String pattern, umm result)
{
  ASSERT(MemosFill < (umm)((1ULL << MEMOS_SIZE_LG2) * 0.7f));

  u64 hash = FNV1A(pattern);
  u64 idx  = (hash == 0 ? 1 : hash & MEMOS_MASK);
  u64 step = 1;

  while (Memos[idx].hash != 0)
  {
    if (Memos[idx].hash == hash && String_Match(Memos[idx].pattern, pattern))
    {
      break;
    }

    idx = (idx + step++) & MEMOS_MASK;
  }

  ASSERT(Memos[idx].hash == 0);

  Memos[idx] = (Memo){
    .hash    = hash,
    .pattern = pattern,
    .result  = result,
  };

  MemosFill += 1;
}

umm
IsPossible(String pattern, String towels[8][128], umm towels_len[8])
{
  umm result = 0;

  if (!Memos_Get(pattern, &result))
  {
    if (pattern.len == 0) result = 1;
    else
    {
      for (u32 i = 8-1; i < 8; --i)
      {
        String pattern_lo = (String){ .data = pattern.data,       .len = MIN(i+1, pattern.len) };
        String pattern_hi = (String){ .data = pattern.data + i+1, .len = (pattern.len > i ? pattern.len - (i+1) : 0)};
        for (umm j = 0; j < towels_len[i]; ++j)
        {
          umm hi_matches = IsPossible(pattern_hi, towels, towels_len);
          if (String_Match(pattern_lo, towels[i][j]) && hi_matches)
          {
            result += hi_matches;
          }
        }
      }
    }

    Memos_Put(pattern, result);
  }

  return result;
}

int
main(int argc, char** argv)
{
  String input;
  if (!LoadInput(argc, argv, &input)) return -1;

  umm part1_result = 0;
  umm part2_result = 0;

  String towels[8][128];
  umm towels_len[8] = {0};

  for (;;)
  {
    String towel;
    bool ok = EatIdent(&input, &towel);
    ASSERT(ok);

    ASSERT(towel.len >= 1 && towel.len <= 8);
    towels[towel.len-1][towels_len[towel.len-1]++] = towel;

    if (EatPrefix(&input, STRING(", "))) continue;
    else                                 break;
  }

  EatWhitespace(&input);
  while (input.len > 0)
  {
    String pattern;
    bool ok = EatIdent(&input, &pattern);
    ASSERT(ok);

    umm matches = IsPossible(pattern, towels, towels_len);

    part1_result += (matches > 0);
    part2_result += matches;

    EatWhitespace(&input);
  }
  
  printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

  printf("%llu\n", MemosFill);

  return 0;
}
