#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  {
    String pages = input;
    pages.len -= sizeof("\r\n")-1;

    umm i    = 0;
    umm pos  = 0;
    umm sval = pages.data[pages.len-1] & 0xF;
    while (i < pages.len)
    {
      umm count;
      if (i < pages.len-1) count = pages.data[i] & 0xF;
      else                 count = sval;

      for (umm j = 0; j < count; ++j, ++pos)
      {
        part1_result += pos*(i>>1);
      }

      ++i;

      if (i >= pages.len) break;

      umm space = pages.data[i] & 0xF;

      for (umm j = 0; j < space; ++j, ++pos)
      {
        while (sval == 0 && pages.len > 1)
        {
          pages.len -= 2;
          sval = pages.data[pages.len-1] & 0xF;
        }

        if (sval == 0) break;
        else
        {
          part1_result += pos*((pages.len-1) >> 1);
          sval -= 1;
        }
      }

      ++i;
    }
  }

  {
    input.len -= sizeof("\r\n")-1;

    for (umm i = 0, pos = 0; i < input.len;)
    {
      umm count = input.data[i];

      if (count & 0x80) pos += count & 0xF;
      else
      {
        count &= 0xF;

        for (umm j = 0; j < count; ++j, ++pos)
        {
          part2_result += pos*(i >> 1);
        }
      }

      ++i;

      umm space = input.data[i] & 0xF;

      for (umm j = input.len-1; space != 0 && j > i; j -= 2)
      {
        if (input.data[j] & 0x80) continue;

        umm size = input.data[j] & 0xF;
        if (size <= space)
        {
          if (j == input.len-1) input.len     -= 2;
          else                  input.data[j] |= 0x80;

          for (umm k = 0; k < size; ++k, ++pos)
          {
            part2_result += pos*(j >> 1);
          }

          space -= size;
        }
      }

      pos += space;
      ++i;
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
