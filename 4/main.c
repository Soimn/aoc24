#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

#define MAP_PAD 3
#define MAP_PITCH (140 + MAP_PAD)
	u8 map[MAP_PITCH*MAP_PITCH] = {0};
	umm map_size = 0;

	while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_PITCH);

	for (umm j = 0; j < map_size; ++j)
	{
		for (umm i = 0; i < map_size; ++i)
		{
			ASSERT(j*(map_size+2) + i < input.len);
			map[j*MAP_PITCH + i] = input.data[j*(map_size+2) + i];
		}
	}

	u32 xmas = *(u32*)"XMAS";
	u32 samx = *(u32*)"SAMX";
	u32 mas  = *(u32*)"MAS";
	u32 sam  = *(u32*)"SAM";
	for (umm j = 0; j < map_size - 2; ++j)
	{
		for (umm i = 0; i < map_size - 2; ++i)
		{
			u32 kernel_rows[4];
			for (umm k = 0; k < 4; ++k) kernel_rows[k] = *(u32*)&map[(j + k)*MAP_PITCH + i];

			u32 kernel_cols[4] = {0};
			for (umm k = 0; k < 4; ++k)
			{
				for (umm w = 0; w < 4; ++w)
				{
					kernel_cols[k] |= ((kernel_rows[w] >> (k*8))&0xFF) << (w*8);
				}
			}

			part1_result += (kernel_rows[0] == xmas || kernel_rows[0] == samx);
			if (j == map_size - 3)
			{
				part1_result += (kernel_rows[1] == xmas || kernel_rows[1] == samx);
				part1_result += (kernel_rows[2] == xmas || kernel_rows[2] == samx);
			}

			part1_result += (kernel_cols[0] == xmas || kernel_cols[0] == samx);
			if (i == map_size - 3)
			{
				part1_result += (kernel_cols[1] == xmas || kernel_cols[1] == samx);
				part1_result += (kernel_cols[2] == xmas || kernel_cols[2] == samx);
			}

			u32 diag     = (kernel_rows[0]&0x000000FF) | (kernel_rows[1]&0x0000FF00) | (kernel_rows[2]&0x00FF0000) | (kernel_rows[3]&0xFF000000);
			u32 antidiag = (kernel_rows[0]&0xFF000000) | (kernel_rows[1]&0x00FF0000) | (kernel_rows[2]&0x0000FF00) | (kernel_rows[3]&0x000000FF);
			part1_result += (diag == xmas || diag == samx);
			part1_result += (antidiag == xmas || antidiag == samx);

			u32 short_diag     = diag&0x00FFFFFF;
			u32 short_antidiag = (kernel_rows[0]&0x00FF0000) | (kernel_rows[1]&0x0000FF00) | (kernel_rows[2]&0x000000FF);
			part2_result += ((short_diag == mas || short_diag == sam) && (short_antidiag == mas || short_antidiag == sam));
		}
	}

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
