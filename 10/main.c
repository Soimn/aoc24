#include "../aoc.h"

typedef struct Trail_End
{
  V2S pos;
  u32 visited;
} Trail_End;

void
DFS(u8* map, umm map_size, umm x, umm y, Trail_End* ends, umm ends_len)
{
  umm height = map[y*map_size + x];

  if (height == 9)
  {
    for (umm i = 0; i < ends_len; ++i)
    {
      if (V2S_Equal(ends[i].pos, V2S((s32)x, (s32)y)))
      {
        ends[i].visited += 1;
        break;
      }
    }
  }
  else
  {
    if (x > 0        && map[   y*map_size  + x-1] == height+1) DFS(map, map_size, x-1,   y, ends, ends_len);
    if (y > 0        && map[(y-1)*map_size +   x] == height+1) DFS(map, map_size,   x, y-1, ends, ends_len);
    if (x < map_size && map[    y*map_size + x+1] == height+1) DFS(map, map_size, x+1,   y, ends, ends_len);
    if (y < map_size && map[(y+1)*map_size +   x] == height+1) DFS(map, map_size,   x, y+1, ends, ends_len);
  }
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  #define MAP_MAX_SIZE 60
  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm map_size = 0;

  V2S starts[512];
  umm starts_len = 0;

  Trail_End ends[512] = {0};
  umm ends_len = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 height = input.data[j*(map_size+2) + i] & 0xF;
      map[j*map_size + i] = height;

      if (height == 0)
      {
        ASSERT(starts_len < ARRAY_LEN(starts));
        starts[starts_len++] = V2S((s32)i, (s32)j);
      }
      else if (height == 9)
      {
        ASSERT(ends_len < ARRAY_LEN(ends));
        ends[ends_len++] = (Trail_End){ .pos = V2S((s32)i, (s32)j), .visited = false };
      }
    }
  }

  for (umm i = 0; i < starts_len; ++i)
  {
    DFS(map, map_size, starts[i].x, starts[i].y, ends, ends_len);

    for (umm j = 0; j < ends_len; ++j)
    {
      part1_result += (ends[j].visited != 0 ? 1 : 0);
      part2_result += ends[j].visited;
      ends[j].visited = 0;
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
