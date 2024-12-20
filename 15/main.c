#include "../aoc.h"

umm
Part1(String input)
{
#define MAP_MAX_SIZE 50
  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  s32 map_size = 0;

  while (map_size < (s32)input.len && input.data[map_size] != '\r') ++map_size;

  V2S pos = V2S(-1, -1);

  ASSERT((map_size-1)*(map_size+2) + map_size-1 < (s32)input.len);
  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      if (c == '@')
      {
        pos = V2S((s32)i, (s32)j);
        c = '.';
      }

      map[j*map_size + i] = c;

    }
  }

  ASSERT(pos.x != -1 && pos.y != -1);

  Advance(&input, map_size*(map_size+2));
  EatWhitespace(&input);

  while (input.len != 0)
  {
    u8 c = input.data[0];
    Advance(&input, 1);
    EatWhitespace(&input);

    V2S dir;
    if      (c == '^') dir = V2S( 0, -1);
    else if (c == 'v') dir = V2S( 0,  1);
    else if (c == '<') dir = V2S(-1,  0);
    else               dir = V2S( 1,  0);

    V2S scan = V2S_Add(pos, dir);
    while (V2S_InBounds(scan, 0, map_size) && map[scan.y*map_size + scan.x] == 'O') scan = V2S_Add(scan, dir);

    if (V2S_InBounds(scan, 0, map_size) && map[scan.y*map_size + scan.x] != '#')
    {
      while (!V2S_Equal(scan, pos))
      {
        V2S nscan = V2S_Sub(scan, dir);

        u8 tmp = map[scan.y*map_size + scan.x];
        map[scan.y*map_size + scan.x]   = map[nscan.y*map_size + nscan.x];
        map[nscan.y*map_size + nscan.x] = tmp;

        scan = nscan;
      }

      pos = V2S_Add(pos, dir);
    }
  }

  umm result = 0;

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      if (map[j*map_size + i] == 'O')
      {
        result += j*100 + i;
      }
    }
  }

  return result;
}

bool
IsVerticalMoveLegal(u8* map, s32 map_width, s32 map_height, V2S pos, V2S dir)
{
  bool is_legal = false;

  V2S npos = V2S_Add(pos, dir);

  if (V2S_InBounds2(npos, 0, map_width, 0, map_height))
  {
    u8 c = map[npos.y*map_width + npos.x];

    if (c == '.'                                                                                                                                                  ||
        c == '[' && IsVerticalMoveLegal(map, map_width, map_height, npos, dir) && IsVerticalMoveLegal(map, map_width, map_height, V2S_Add(npos, V2S( 1, 0)), dir) ||
        c == ']' && IsVerticalMoveLegal(map, map_width, map_height, npos, dir) && IsVerticalMoveLegal(map, map_width, map_height, V2S_Add(npos, V2S(-1, 0)), dir))
    {
      is_legal = true;
    }
  }

  return is_legal;
}

void
DoVerticalMove(u8* map, s32 map_width, s32 map_height, V2S pos, V2S dir)
{
  V2S npos = V2S_Add(pos, dir);

  ASSERT(V2S_InBounds2(pos, 0, map_width, 0, map_height) && V2S_InBounds2(npos, 0, map_width, 0, map_height));
  u8* c  = &map[pos.y*map_width  + pos.x];
  u8* nc = &map[npos.y*map_width + npos.x];
  ASSERT(*nc == '.' || *nc == '[' || *nc == ']');

  if (*nc == '[')
  {
    DoVerticalMove(map, map_width, map_height, npos, dir);
    DoVerticalMove(map, map_width, map_height, V2S_Add(npos, V2S(1, 0)), dir);
  }
  else if (*nc == ']')
  {
    DoVerticalMove(map, map_width, map_height, npos, dir);
    DoVerticalMove(map, map_width, map_height, V2S_Add(npos, V2S(-1, 0)), dir);
  }

  u8 tmp = *c;
  *c  = *nc;
  *nc = tmp;
}

umm
Part2(String input)
{
#define MAP_MAX_SIZE 50
  u8 map[MAP_MAX_SIZE*2*MAP_MAX_SIZE];
  s32 map_width  = 0;
  s32 map_height = 0;

  while (map_width < (s32)input.len && input.data[map_width] != '\r') ++map_width;
  map_height = map_width;
  map_width *= 2;

  V2S pos = V2S(-1, -1);

  ASSERT((map_height-1)*(map_width/2 + 2) + map_width/2-1 < (s32)input.len);
  for (umm j = 0; j < map_height; ++j)
  {
    for (umm i = 0; i < map_width/2; ++i)
    {
      u8 c = input.data[j*(map_width/2 + 2) + i];

      if (c == '@')
      {
        pos = V2S((s32)i*2, (s32)j);
        c = '.';
      }

      u8 c2 = c;
      if (c == 'O')
      {
        c  = '[';
        c2 = ']';
      }

      map[j*map_width + 2*i]   = c;
      map[j*map_width + 2*i+1] = c2;
    }
  }

  ASSERT(pos.x != -1 && pos.y != -1);

  Advance(&input, (map_width/2)*(map_width/2 + 2));
  EatWhitespace(&input);

  while (input.len != 0)
  {
    u8 c = input.data[0];
    Advance(&input, 1);
    EatWhitespace(&input);

    V2S dir;
    if      (c == '^') dir = V2S( 0, -1);
    else if (c == 'v') dir = V2S( 0,  1);
    else if (c == '<') dir = V2S(-1,  0);
    else               dir = V2S( 1,  0);

    if (dir.y == 0)
    {
      V2S scan = V2S_Add(pos, dir);
      while (V2S_InBounds2(scan, 0, map_width, 0, map_height) && (map[scan.y*map_width + scan.x] == '[' || map[scan.y*map_width + scan.x] == ']')) scan = V2S_Add(scan, dir);

      if (V2S_InBounds2(scan, 0, map_width, 0, map_height) && map[scan.y*map_width + scan.x] != '#')
      {
        while (!V2S_Equal(scan, pos))
        {
          V2S nscan = V2S_Sub(scan, dir);

          u8 tmp = map[scan.y*map_width + scan.x];
          map[scan.y*map_width + scan.x]   = map[nscan.y*map_width + nscan.x];
          map[nscan.y*map_width + nscan.x] = tmp;

          scan = nscan;
        }

        pos = V2S_Add(pos, dir);
      }
    }
    else
    {
      if (IsVerticalMoveLegal(map, map_width, map_height, pos, dir))
      {
        DoVerticalMove(map, map_width, map_height, pos, dir);
        pos = V2S_Add(pos, dir);
      }
    }
  }

  umm result = 0;

  for (umm j = 0; j < map_height; ++j)
  {
    for (umm i = 0; i < map_width; ++i)
    {
      if (map[j*map_width + i] == '[')
      {
        result += j*100 + i;
      }
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

  part1_result = Part1(input);
  part2_result = Part2(input);

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
