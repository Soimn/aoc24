#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

	bool enabled = true;
	while (input.len != 0)
	{
		if      (EatPrefix(&input, STRING("do()")))    enabled = true;
		else if (EatPrefix(&input, STRING("don't()"))) enabled = false;
		else if (EatPrefix(&input, STRING("mul(")))
		{
			umm x, y;
			if (EatUMM(&input, &x) && x <= 999 && EatChar(&input, ',') && EatUMM(&input, &y) && y <= 999 && EatChar(&input, ')'))
			{
				part1_result += x*y;
				if (enabled) part2_result += x*y;
			}
		}
		else Advance(&input, 1);
	}

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
