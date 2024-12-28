#include "../aoc.h"

#define MAP_SIZE 71
#define QUEUE_CAP (MAP_SIZE*MAP_SIZE)

typedef struct Queue_Entry
{
  u16 key;
  u16 pos;
} Queue_Entry;

void
Enqueue(Queue_Entry queue[QUEUE_CAP], umm* queue_len, u16 key, u16 pos)
{
  ASSERT(*queue_len < QUEUE_CAP);
  queue[*queue_len] = (Queue_Entry){ .key = key, .pos = pos };
  *queue_len += 1;

  for (umm i = (*queue_len-2)/2; i < *queue_len; i = (i-1)/2)
  {
    umm swap_idx = i;
    if (2*i+1 < *queue_len && queue[2*i+1].key < queue[swap_idx].key) swap_idx = 2*i+1;
    if (2*i+2 < *queue_len && queue[2*i+2].key < queue[swap_idx].key) swap_idx = 2*i+2;

    if (swap_idx == i) break;

    Queue_Entry tmp = queue[i];
    queue[i]        = queue[swap_idx];
    queue[swap_idx] = tmp;
  }
}

Queue_Entry
Dequeue(Queue_Entry queue[QUEUE_CAP], umm* queue_len)
{
  ASSERT(*queue_len > 0);
  Queue_Entry result = queue[0];

  *queue_len -= 1;
  queue[0] = queue[*queue_len];

  for (umm i = 0; i < *queue_len-1;)
  {
    umm swap_idx = i;
    if (2*i+1 < *queue_len && queue[2*i+1].key < queue[swap_idx].key) swap_idx = 2*i+1;
    if (2*i+2 < *queue_len && queue[2*i+2].key < queue[swap_idx].key) swap_idx = 2*i+2;

    if (swap_idx == i) break;

    Queue_Entry tmp = queue[i];
    queue[i]        = queue[swap_idx];
    queue[swap_idx] = tmp;

    i = swap_idx;
  }

  return result;
}
  Queue_Entry queue[QUEUE_CAP];

bool
HasPath(u8 template_map[MAP_SIZE*MAP_SIZE], umm* path_len)
{
  u8 map[MAP_SIZE*MAP_SIZE];
  for (umm i = 0; i < ARRAY_LEN(map); ++i) map[i] = template_map[i];

  umm queue_len = 0;

  map[0] = '#';
  Enqueue(queue, &queue_len, 0, 0);

  bool result = false;
  while (queue_len > 0)
  {
    Queue_Entry entry = Dequeue(queue, &queue_len);

    V2S pos = V2S(entry.pos % MAP_SIZE, entry.pos / MAP_SIZE);

    if (V2S_Equal(pos, V2S(MAP_SIZE-1, MAP_SIZE-1)))
    {
      *path_len = entry.key;
      result    = true;
      break;
    }

    for (umm i = 0; i < 4; ++i)
    {
      V2S npos   = V2S_Add(pos, (V2S[4]){ V2S(0, 1), V2S(0, -1), V2S(-1, 0), V2S(1, 0) }[i]);
      u16 npos_i = (u16)(npos.y*MAP_SIZE + npos.x);

      if (V2S_InBounds(npos, 0, MAP_SIZE) && map[npos_i] != '#')
      {
        map[npos_i] = '#';
        Enqueue(queue, &queue_len, entry.key + 1, npos_i);
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
  V2S part2_result = V2S(0, 0);

  u8 template_map[MAP_SIZE*MAP_SIZE] = {0};

  for (umm i = 0; i < 1024; ++i)
  {
    umm x, y;
    if (EatUMM(&input, &x) && EatChar(&input, ',') && EatUMM(&input, &y))
    {
      EatWhitespace(&input);

      template_map[y*MAP_SIZE + x] = '#';
    }
    else ASSERT(!"Missing input");
  }

  bool part1_ok = HasPath(template_map, &part1_result);
  ASSERT(part1_ok);

  for (;;)
  {
    umm x, y;
    if (EatUMM(&input, &x) && EatChar(&input, ',') && EatUMM(&input, &y))
    {
      EatWhitespace(&input);

      template_map[y*MAP_SIZE + x] = '#';

      umm neighbour_count = (x == 0 || x == MAP_SIZE-1 || y == 0 || y == MAP_SIZE-1);
      neighbour_count += (x > 0          && template_map[y*MAP_SIZE + (x-1)] == '#');
      neighbour_count += (x < MAP_SIZE-1 && template_map[y*MAP_SIZE + (x+1)] == '#');
      neighbour_count += (y > 0          && template_map[(y-1)*MAP_SIZE + x] == '#');
      neighbour_count += (y < MAP_SIZE-1 && template_map[(y+1)*MAP_SIZE + x] == '#');
      
      if (neighbour_count < 2) continue;

      if (!HasPath(template_map, &(umm){0}))
      {
        part2_result = V2S((s32)x, (s32)y);
        break;
      }
    }
    else ASSERT(!"Missing input");
  }

  printf("Part 1 result: %llu\nPart 2 result: %d,%d\n", part1_result, part2_result.x, part2_result.y);

  return 0;
}
