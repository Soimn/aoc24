#include "../aoc.h"

typedef struct Cell
{
  V2S pos;
  umm visit_t;
  umm pred;
} Cell;

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

#define MAP_SIZE 71
  struct { umm visit_t; umm pred; } map[MAP_SIZE*MAP_SIZE];
  for (umm i = 0; i < ARRAY_LEN(map); ++i) map[i].visit_t = UMM_MAX;

  umm t = 0;
  while (input.len != 0)
  {
    s32 x, y;
    bool ok = (EatS32(&input, &x) && EatChar(&input, ',') && EatS32(&input, &y));
    ASSERT(ok);

    map[y*MAP_SIZE + x].visit_t = t++;
    map[y*MAP_SIZE + x].pred    = y*MAP_SIZE + x;

    EatWhitespace(&input);
  }

  Cell queue[MAP_SIZE*MAP_SIZE];
  umm queue_len = ARRAY_LEN(queue);
  for (umm j = 0; j < MAP_SIZE; ++j)
  {
    for (umm i = 0; i < MAP_SIZE; ++i)
    {
      queue[j*MAP_SIZE + i] = (Cell){ .pos = V2S((s32)i, (s32)j), .visit_t = UMM_MAX, .pred = UMM_MAX };
    }
  }

  queue[0].visit_t = 0;
  queue[0].pred    = 0;

  for (;;)
  {
    Cell c = queue[0];

    queue[0] = queue[--queue_len];
    for (umm i = 0; i < queue_len;)
    {
      umm swap_idx = i;

      if (2*i+1 < queue_len && queue[2*i+1].visit_t < queue[swap_idx].visit_t) swap_idx = 2*i+1;
      if (2*i+2 < queue_len && queue[2*i+2].visit_t < queue[swap_idx].visit_t) swap_idx = 2*i+2;

      if (swap_idx == i) break;

      Cell tmp = queue[i];
      queue[i]        = queue[swap_idx];
      queue[swap_idx] = tmp;

      i = swap_idx;
    }

    if (V2S_Equal(c.pos, V2S(MAP_SIZE-1, MAP_SIZE-1))) break;

    for (umm i = 0; i < 4; ++i)
    {
      V2S npos = V2S_Add(c.pos, (V2S[4]){ V2S(0, 1), V2S(0, -1), V2S(-1, 0), V2S(1, 0) }[i]);

      if (V2S_InBounds(npos, 0, MAP_SIZE) && c.visit_t + 1 < map[npos.y*MAP_SIZE + npos.x].visit_t)
      {
        umm j = 0;
        while (j < queue_len && !V2S_Equal(queue[j].pos, npos)) ++j;
        ASSERT(j < queue_len);

        queue[j].visit_t = c.visit_t + 1;
        queue[j].pred    = c.pos.y*MAP_SIZE + c.pos.x;

        for (umm k = j/2;; k /= 2)
        {
          umm swap_idx = k;

          if (2*k+1 < queue_len && queue[2*k+1].visit_t < queue[swap_idx].visit_t) swap_idx = 2*k+1;
          if (2*k+2 < queue_len && queue[2*k+2].visit_t < queue[swap_idx].visit_t) swap_idx = 2*k+2;

          if (swap_idx == k) break;

          Cell tmp = queue[k];
          queue[k]        = queue[swap_idx];
          queue[swap_idx] = tmp;

          if (k == 0) break;
        }
      }
    }
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
