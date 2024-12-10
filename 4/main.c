#include "../aoc.h"

umm
SearchXMAS(u8* map, umm map_size, smm x_step, smm y_step)
{
	ASSERT((umm)(x_step+1) <= 2 && (umm)y_step <= 1);

	umm result = 0;

	u32 xmas = *(u32*)"XMAS";
	u32 smax = *(u32*)"SAMX";

	for (umm j = 0; j < map_size - (y_step > 0 ? 3 : 0); ++j)
	{
		for (umm i = (x_step < 0 ? 3 : 0); i < map_size - (x_step > 0 ? 3 : 0); ++i)
		{
			u32 sample = 0;
			for (umm k = 0; k < 4; ++k) sample |= (u32)map[(j + k*y_step)*map_size + (i + k*x_step)] << k*8;

			if (sample == xmas || sample == smax) result += 1;
		}
	}

	return result;
}

umm
SearchMAS(u8* map, umm map_size)
{
	umm result = 0;

	u32 mas = *(u32*)"MAS";
	u32 sam = *(u32*)"SAM";

	for (umm j = 0; j < map_size - 3; ++j)
	{
		for (umm i = 0; i < map_size - 3; ++i)
		{
			u32 sample1 = 0;
			u32 sample2 = 0;
			for (umm k = 0; k < 3; ++k)
			{
				sample1 |= (u32)map[(j+k)*map_size + (i+k)]   << k*8;
				sample2 |= (u32)map[(j+k)*map_size + (i+2-k)] << k*8;
			}

			result += ((sample1 == mas || sample1 == sam) && (sample2 == mas || sample2 == sam));
		}
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

	u8 map[140*140];
	umm map_size = 0;

	while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

	for (umm j = 0; j < map_size; ++j)
	{
		for (umm i = 0; i < map_size; ++i)
		{
			ASSERT(input.len > 0);
			map[j*map_size + i] = input.data[0];
			Advance(&input, 1);
		}
		Advance(&input, sizeof("\r\n")-1);
	}

	part1_result = SearchXMAS(map, map_size, 1, 0) + SearchXMAS(map, map_size, 0, 1) + SearchXMAS(map, map_size, 1, 1) + SearchXMAS(map, map_size, -1, 1);
	part2_result = SearchMAS(map, map_size);

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
