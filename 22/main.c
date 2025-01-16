#include "../aoc.h"

int
main(int argc, char** argv)
{
  String input;
  if (!LoadInput(argc, argv, &input)) return -1;

  umm part1_result = 0;
  umm part2_result = 0;

  for (umm n; EatUMM(&input, &n); EatWhitespace(&input))
  {
    /*
    n = ((n*64) ^ n) % 16777216;
    n = ((n/32) ^ n) % 16777216;
    n = ((n*2048) ^ n) % 16777216;

    n = ((n << 6) ^ n) & 0xFFFFFF;
    n = ((n >> 5) ^ n) & 0xFFFFFF;
    n = ((n << 11) ^ n) & 0xFFFFFF;

    shl
    xor
    and
    shr
    xor
    and
    shl
    xor
    and

    9 cycles * 2000 = 18000 cycles

    1.8e4 cycles / 3.2e9 cycles / second ~= 0.5626e-5 seconds

    2000 * 0.5626e-5 = 0.01125 seconds
    */

    /*
    for (umm i = 0; i < 2000; ++i)
    {
      n ^= n*64;
      n %= 16777216;
      n ^= n/32;
      n %= 16777216;
      n ^= n*2048;
      n %= 16777216;
    }
    */

    for (umm i = 0; i < 2000; ++i)
    {
      n ^= n << 6;
      n &= 0xFFFFFF;
      n ^= n >> 5;
      n ^= n << 11;
      n &= 0xFFFFFF;
    }

    part1_result += n;
  }

  printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

  return 0;
}
