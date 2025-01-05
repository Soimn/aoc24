#include "..\aoc.h"

int
main(int argc, char** argv)
{
  String input;
  if (!LoadInput(argc, argv, &input)) return -1;

  umm part1_result = 0;
  umm part2_result = 0;

#define MAP_MAX_SIZE 141
#define MAP_MAX_SIZE_NLG2 8
  V2S map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  s32 map_size = 0;

  while ((u32)map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT(((umm)map_size-1)*((umm)map_size+2) + (umm)map_size-1 < input.len);

  s32 start = -1;
  s32 end   = -1;

  for (s32 j = 0; j < map_size; ++j)
  {
    for (s32 i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      if      (c == 'S') start = j*map_size + i;
      else if (c == 'E') end   = j*map_size + i;

      s32 d;

      if (c == '#') d = -1;
      else          d = S32_MAX;

      map[j*map_size + i] = V2S(d, d);
    }
  }

  ASSERT(start != -1 && end != -1);

  map[start].x = 0;
  map[end].y   = 0;

#define QUEUE_CAP (1ULL << MAP_MAX_SIZE_NLG2)
#define QUEUE_MASK (QUEUE_CAP - 1)
  struct { s32 idx; s32 d; } queue[QUEUE_CAP];
  umm queue_head = 0;
  umm queue_tail = 0;

  queue[queue_tail].idx = start;
  queue[queue_tail].d   = 0;
  queue_tail += 1;

  while (queue_head != queue_tail)
  {
    s32 idx = queue[queue_head].idx;
    s32 d   = queue[queue_head].d;

    queue_head = (queue_head + 1) & QUEUE_MASK;

    for (umm i = 0; i < 4; ++i)
    {
      s32 nidx = (s32[4]){ idx + 1, idx + map_size, idx - 1, idx - map_size }[i];

      if (nidx >= 0 && nidx < map_size*map_size && map[nidx].x == S32_MAX)
      {
        map[nidx].x = d + 1;

        queue[queue_tail].idx = nidx;
        queue[queue_tail].d   = d + 1;
        queue_tail = (queue_tail + 1) & QUEUE_MASK;
      }
    }
  }

  queue[queue_tail].idx = end;
  queue[queue_tail].d   = 0;
  queue_tail += 1;

  while (queue_head != queue_tail)
  {
    s32 idx = queue[queue_head].idx;
    s32 d   = queue[queue_head].d;

    queue_head = (queue_head + 1) & QUEUE_MASK;

    for (umm i = 0; i < 4; ++i)
    {
      s32 nidx = (s32[4]){ idx + 1, idx + map_size, idx - 1, idx - map_size }[i];

      if (nidx >= 0 && nidx < map_size*map_size && map[nidx].y == S32_MAX)
      {
        map[nidx].y = d + 1;

        queue[queue_tail].idx = nidx;
        queue[queue_tail].d   = d + 1;
        queue_tail = (queue_tail + 1) & QUEUE_MASK;
      }
    }
  }

  V2S diffs[841 - 5];
  umm diffs_len = 0;

  for (s32 j = -20; j <= 20; ++j)
  {
    for (s32 i = -20; i <= 20; ++i)
    {
      s32 steps =  (i < 0 ? -i : i) + (j < 0 ? -j : j);

      if (steps >= 2 && steps <= 20)
      {
        ASSERT(diffs_len < ARRAY_LEN(diffs));
        diffs[diffs_len++] = V2S(i, j);
      }
    }
  }

  ASSERT(diffs_len == ARRAY_LEN(diffs));

  for (s32 j = 0; j < map_size; ++j)
  {
    for (s32 i = 0; i < map_size; ++i)
    {
      s32 idx = j*map_size + i;
      s32 d   = map[idx].x;

      if (d == -1) continue;

      s32 x = idx % map_size;
      s32 y = idx / map_size;

      for (umm k = 0; k < ARRAY_LEN(diffs); ++k)
      {
        s32 dx    = diffs[k].x;
        s32 dy    = diffs[k].y;
        s32 steps = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
        s32 nidx  = idx + dy*map_size + dx;

        if (x + dx >= 0 && x + dx < map_size && y + dy >= 0 && y + dy < map_size)
        {
          s32 nd = map[nidx].y;

          if (nd == -1) continue;

          s32 cd = d + nd + steps;

          if (map[end].x - cd >= 100)
          {
            part1_result += (steps <= 2);
            part2_result += 1;
          }
        }
      }
    }
  }

  printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

  return 0;
}
