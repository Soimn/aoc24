#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  for (;;)
  {
    umm sequence[17];
    umm sequence_len = 0;
    umm expected_result;
    if (!EatUMM(&input, &expected_result) || !EatChar(&input, ':')) break;
    else
    {
      for (;;)
      {
        ASSERT(sequence_len < ARRAY_LEN(sequence));
        EatSpace(&input);
        EatUMM(&input, &sequence[sequence_len++]);

        if (EatPrefix(&input, STRING("\r\n"))) break;
        else                                   continue;
      }

      for (umm i = 0; i < (1ULL << (sequence_len-1)); ++i)
      {
        umm result = sequence[0];
        for (umm j = 0; j < sequence_len-1; ++j)
        {
          if (((i >> j) & 1) != 0) result *= sequence[j+1];
          else                     result += sequence[j+1];
        }

        if (result == expected_result)
        {
          part1_result += expected_result;
          break;
        }
      }

      umm skip_list[ARRAY_LEN(sequence)];
      for (umm i = 0; i < sequence_len; ++i)
      {
        skip_list[i] = 1;
        for (umm x = sequence[i]; x != 0; x /= 10)
        {
          skip_list[i] *= 10;
        }
      }

      for (umm i = 0; i < (1ULL << (2*sequence_len-2)); ++i)
      {
        umm overflow;
        do
        {
          overflow = (i >> 1) & (i & 0x5555555555555555);
          i += overflow;
        } while (overflow != 0);

        umm result = sequence[0];
        for (umm j = 1; j < sequence_len; ++j)
        {
          umm tit = (i >> (j*2 - 2)) & 0x3;
          if      (tit == 0) result *= sequence[j];
          else if (tit == 1) result += sequence[j];
          else               result  = result*skip_list[j] + sequence[j];
        }

        if (result == expected_result)
        {
          part2_result += expected_result;
          break;
        }
      }
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
