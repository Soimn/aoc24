#include "../aoc.h"

typedef enum Cell_Kind
{
  WALL    = 1,
  VISITED = 2,
} Cell_Kind;

bool
Visit(u8* map, umm map_size, V2S pos, V2S dir)
{
  bool stuck_in_cycle = false;

  map[pos.y*map_size + pos.x] = VISITED | (((dir.x+1) + 3*(dir.y+2)) << 2);

  for (;;)
  {
    V2S new_pos = V2S_Add(pos, dir);
    u8 heading  = (dir.x+1) + 3*(dir.y+2);

    if (new_pos.x < 0 || new_pos.x >= map_size || new_pos.y < 0 || new_pos.y >= map_size) break;
    else if (((map[new_pos.y*map_size + new_pos.x] >> 2) & 0xF) == heading)
    {
      stuck_in_cycle = true;
      break;
    }
    else if (map[new_pos.y*map_size + new_pos.x]&WALL)
    {
      dir = V2S(-dir.y, dir.x);
    }
    else
    {
      map[new_pos.y*map_size + new_pos.x] = VISITED | (heading << 2);
      pos = new_pos;
    }
  }

  return stuck_in_cycle;
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

#define MAP_MAX_SIZE 130
  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm map_size = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  V2S start_pos = V2S(-1, -1);
  V2S start_dir = V2S(0, -1);

  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      if (c == '^') start_pos = V2S((s32)i, (s32)j);

      map[j*map_size + i] = (c == '#' ? WALL : 0);
    }
  }

  ASSERT(start_pos.x != -1 && start_pos.y != -1);

  u8 part1_map[ARRAY_LEN(map)];
  Copy(part1_map, map, sizeof(part1_map));
  Visit(part1_map, map_size, start_pos, start_dir);

  for (umm i = 0; i < map_size*map_size; ++i) part1_result += ((part1_map[i]&VISITED) != 0 ? 1 : 0);

  for (umm i = 0; i < map_size*map_size; ++i)
  {
    if ((map[i]&WALL) != 0) continue;

    u8 mod_map[ARRAY_LEN(map)];
    Copy(mod_map, map, sizeof(mod_map));

    mod_map[i] |= WALL;

    if (Visit(mod_map, map_size, start_pos, start_dir)) part2_result += 1;
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
