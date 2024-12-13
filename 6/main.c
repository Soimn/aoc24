#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

#define VISITED 2
#define WALL 1
#define MAP_MAX_SIZE 130
  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm map_size = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  V2S guard_pos = V2S(-1, -1);
  V2S guard_dir = V2S(0, 1);

  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);
  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];
      if      (c == '#') map[j*map_size + i] = WALL;
      else if (c == '^') guard_pos = V2S((s32)i, (s32)j);
    }
  }

  ASSERT(!V2S_Equal(guard_pos, V2S(-1, -1)));

  for (;;)
  {
    map[guard_pos.y*map_size + guard_pos.x] |= VISITED;
    V2S new_pos = V2S_Add(guard_pos, guard_dir);

    if (new_pos.x < 0 || new_pos.x >= map_size || new_pos.y < 0 || new_pos.y >= map_size) break;
    else if ((map[new_pos.y*map_size + new_pos.x] & WALL) != 0)
    {
      guard_dir = V2S_
    }
  }

  for (umm i = 0; i < map_size*map_size; ++i) part1_result += ((map[i]&VISTED) != 0);

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
