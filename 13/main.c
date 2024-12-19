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
    if (!EatPrefix(&input, STRING("Button A: "))) break;
    else
    {
      V2S a, b, c;
      EatPrefix(&input, STRING("X"));
      EatS32(&input, &a.x);
      EatPrefix(&input, STRING(", Y"));
      EatS32(&input, &a.y);

      EatPrefix(&input, STRING("\r\nButton B: X"));
      EatS32(&input, &b.x);
      EatPrefix(&input, STRING(", Y"));
      EatS32(&input, &b.y);

      EatPrefix(&input, STRING("\r\nPrize: X="));
      EatS32(&input, &c.x);
      EatPrefix(&input, STRING(", Y="));
      EatS32(&input, &c.y);

      EatWhitespace(&input);

      {
        s32 det = a.x*b.y - a.y*b.x;

        s32 as = (c.x*b.y - c.y*b.x);
        s32 bs = (c.y*a.x - c.x*a.y);

        if (as % det == 0 && bs % det == 0)
        {
          part1_result += (as/det)*3 + (bs/det);
        }
      }

      {
        s64 det = a.x*b.y - a.y*b.x;

        s64 c_x = c.x + 10000000000000LL;
        s64 c_y = c.y + 10000000000000LL;

        s64 as = (c_x*b.y - c_y*b.x);
        s64 bs = (c_y*a.x - c_x*a.y);

        if (as % det == 0 && bs % det == 0)
        {
          part2_result += (as/det)*3 + (bs/det);
        }
      }
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
