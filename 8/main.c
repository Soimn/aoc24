#include "../aoc.h"

#define ANTINODE_MASK 0x80

typedef struct Antenna_Array
{
  V2S antennas[4];
  umm antennas_len;
} Antenna_Array;

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  Antenna_Array antenna_arrays[62] = {0};

#define MAP_MAX_SIZE 50
  u8 part1_map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  u8 part2_map[MAP_MAX_SIZE*MAP_MAX_SIZE];

  s32 map_size = 0;
  while ((umm)map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT((umm)((map_size-1)*(map_size+2) + map_size-1) < input.len);
  for (s32 j = 0; j < map_size; ++j)
  {
    for (s32 i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      if (c != '.')
      {
        umm idx;
        if (Char_IsDigit(c)) idx = c&0xF;
        else                 idx = 9 + (c&0x1F) + (c&0x20 ? 26 : 0);
        
        ASSERT(antenna_arrays[idx].antennas_len < ARRAY_LEN(antenna_arrays[idx].antennas));
        antenna_arrays[idx].antennas[antenna_arrays[idx].antennas_len++] = V2S(i, j);
      }

      part1_map[j*map_size + i] = c;
      part2_map[j*map_size + i] = c;
    }
  }

  for (umm i = 0; i < ARRAY_LEN(antenna_arrays); ++i)
  {
    V2S* antennas    = antenna_arrays[i].antennas;
    umm antennas_len = antenna_arrays[i].antennas_len;
    for (smm j = 0; j < (smm)antennas_len-1; ++j)
    {
      for (umm k = j+1; k < antennas_len; ++k)
      {
        V2S diff = V2S_Sub(antennas[k], antennas[j]);

        {
          V2S nodes[2] = { V2S_Add(antennas[k], diff), V2S_Sub(antennas[j], diff) };

          for (umm w = 0; w < ARRAY_LEN(nodes); ++w)
          {
            if (V2S_InBounds(nodes[w], 0, map_size))
            {
              part1_map[nodes[w].y*map_size + nodes[w].x] |= ANTINODE_MASK;
            }
          }
        }

        for (V2S a = antennas[j]; V2S_InBounds(a, 0, map_size); a = V2S_Add(a, diff)) part2_map[a.y*map_size + a.x] |= ANTINODE_MASK;
        for (V2S a = antennas[k]; V2S_InBounds(a, 0, map_size); a = V2S_Sub(a, diff)) part2_map[a.y*map_size + a.x] |= ANTINODE_MASK;
      }
    }
  }

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = part1_map[j*map_size + i];

      part1_result += ((c&ANTINODE_MASK) != 0);

      putchar(c == ('.' | ANTINODE_MASK) ? '#' : (c & ~ANTINODE_MASK));
    }
    putchar('\n');
  }

  putchar('\n');

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = part2_map[j*map_size + i];

      part2_result += ((c&ANTINODE_MASK) != 0);

      putchar(c == ('.' | ANTINODE_MASK) ? '#' : (c & ~ANTINODE_MASK));
    }
    putchar('\n');
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
