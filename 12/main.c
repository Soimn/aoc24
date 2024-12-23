#include "../aoc.h"

u64
CornerSurvey(u8* map, umm map_size, u8 c, s32 x, s32 y)
{
  if (map[y*map_size + x] & 0x80) return 0;
  map[y*map_size + x] |= 0x80;

  u64 result = 0;

  bool n  = false;
  bool ne = false;
  bool e  = false;
  bool se = false;
  bool s  = false;
  bool sw = false;
  bool w  = false;
  bool nw = false;

  if (y < map_size-1 && (map[(y+1)*map_size + x]&0x7F) == c)
  {
    n = true;
    result += CornerSurvey(map, map_size, c, x, y+1);
  }

  ne = (x < map_size-1 && y < map_size-1 && (map[(y+1)*map_size + x+1]&0x7F) == c);

  if (x < map_size-1 && (map[y*map_size + x+1]&0x7F) == c)
  {
    e = true;
    result += CornerSurvey(map, map_size, c, x+1, y);
  }

  se = (x < map_size-1 && y > 0 && (map[(y-1)*map_size + x+1]&0x7F) == c);

  if (y > 0 && (map[(y-1)*map_size + x]&0x7F) == c)
  {
    s = true;
    result += CornerSurvey(map, map_size, c, x, y-1);
  }

  sw = (x > 0 && y > 0 && (map[(y-1)*map_size + x-1]&0x7F) == c);

  if (x > 0 && (map[y*map_size + x-1]&0x7F) == c)
  {
    w = true;
    result += CornerSurvey(map, map_size, c, x-1, y);
  }

  nw = (x > 0 && y < map_size-1 && (map[(y+1)*map_size + x-1]&0x7F) == c);

  u32 corners = 0;

  // outside corners
  corners += (!n && !w); // T
  corners += (!n && !e); // 7
  corners += (!s && !e); // J
  corners += (!s && !w); // L

  // inside corners
  corners += (!se && s && e); // T
  corners += (!sw && s && w); // 7
  corners += (!nw && n && w); // J
  corners += (!ne && n && e); // L

  result += ((u64)1 << 32) | corners;

  return result;
}

u64
Survey(u8* map, umm map_size, u8 c, s32 x, s32 y)
{
  if (map[y*map_size + x] & 0x80) return 0;
  map[y*map_size + x] |= 0x80;

  u64 result      = 0;
  u32 neighbours = 0;

  if (x > 0 && (map[y*map_size + x-1]&0x7F) == c)
  {
    neighbours += 1;
    result     += Survey(map, map_size, c, x-1, y);
  }

  if (x < map_size-1 && (map[y*map_size + x+1]&0x7F) == c)
  {
    neighbours += 1;
    result     += Survey(map, map_size, c, x+1, y);
  }

  if (y > 0 && (map[(y-1)*map_size + x]&0x7F) == c)
  {
    neighbours += 1;
    result     += Survey(map, map_size, c, x, y-1);
  }

  if (y < map_size-1 && (map[(y+1)*map_size + x]&0x7F) == c)
  {
    neighbours += 1;
    result     += Survey(map, map_size, c, x, y+1);
  }

  return (((u64)1 << 32) | neighbours) + result;
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

#define MAP_MAX_SIZE 140
  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm map_size = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);
  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      map[j*map_size + i] = input.data[j*(map_size+2) + i];
    }
  }

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = map[j*map_size + i];

      u64 survey = Survey(map, map_size, c, (s32)i, (s32)j);
      umm area       = survey >> 32;
      umm neighbours = (u32)survey;

      part1_result += area*(4*area - neighbours);
    }
  }

  for (umm i = 0; i < map_size*map_size; ++i) map[i] &= 0x7F;

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = map[j*map_size + i];

      u64 survey = CornerSurvey(map, map_size, c, (s32)i, (s32)j);
      umm area       = survey >> 32;
      umm corners = (u32)survey;

      part2_result += area*corners;
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
