#include "../aoc.h"

typedef struct Queue_Entry
{
  V2S pos;
  umm key;
  umm pred;
} Queue_Entry;

Queue_Entry
Dequeue(Queue_Entry* queue, umm* queue_len)
{
  ASSERT(*queue_len > 0);
  Queue_Entry result = queue[0];

  queue[0] = queue[--*queue_len];

  for (umm i = 0;;)
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

void
DecreaseKey(Queue_Entry* queue, umm queue_len, V2S pos, umm key, umm pred)
{
  umm i = 0;
  for (; i < queue_len; ++i)
  {
    if (V2S_Equal(queue[i].pos, pos)) break;
  }

  if (i < queue_len && queue[i].key > key)
  {
    queue[i].key  = key;
    queue[i].pred = pred;

    for (umm j = (i-1)/2; i != 0; j = (j-1)/2, i = (i-1)/2)
    {
      if (queue[j].key <= queue[i].key) break;
      else
      {
        Queue_Entry tmp = queue[j];
        queue[j] = queue[i];
        queue[i] = tmp;
      }
    }
  }
}

umm
Part1(String input, s32 map_size, umm tod_thresh)
{
#define MAP_MAX_SIZE 71
  Queue_Entry queue[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm queue_len = ARRAY_LEN(queue);

  struct { umm tod; umm pred; } map[MAP_MAX_SIZE*MAP_MAX_SIZE];

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      map[j*map_size + i].tod  = UMM_MAX;
      map[j*map_size + i].pred = UMM_MAX;

      queue[j*map_size + i] = (Queue_Entry){ .pos = V2S((s32)i, (s32)j), .key = UMM_MAX, .pred = UMM_MAX };
    }
  }

  for (umm tod = 0; input.len > 0; ++tod)
  {
    if (tod >= tod_thresh) break;
    umm x, y;
    bool ok = (EatUMM(&input, &x) && EatChar(&input, ',') && EatUMM(&input, &y));
    ASSERT(ok);

    map[y*map_size + x].tod = tod;

    EatWhitespace(&input);
  }

  DecreaseKey(queue, queue_len, V2S(0, 0), 0, 0);

  for (;;)
  {
    Queue_Entry c = Dequeue(queue, &queue_len);
    umm c_i = c.pos.y*map_size + c.pos.x;

    ASSERT(c.key < map[c_i].tod);
    ASSERT(map[c_i].pred == UMM_MAX);

    map[c_i].tod  = c.key;
    map[c_i].pred = c.pred;

    if (V2S_Equal(c.pos, V2S(map_size-1, map_size-1))) break;

    for (umm i = 0; i < 4; ++i)
    {
      V2S pos  = V2S_Add(c.pos, (V2S[4]){ V2S(0, 1), V2S(0, -1), V2S(-1, 0), V2S(1, 0) }[i]);
      umm key  = c.key + 1;
      umm pred = c_i;

      if (V2S_InBounds(pos, 0, map_size) && map[pos.y*map_size + pos.x].tod == UMM_MAX)//&& key < map[pos.y*map_size + pos.x].tod)
      {
        DecreaseKey(queue, queue_len, pos, key, pred);
      }
    }
  }

  return map[(map_size-1)*map_size + map_size-1].tod;
}

void
Part2(String input, s32 map_size)
{
#define MAP_MAX_SIZE 71

  u8 template_map[MAP_MAX_SIZE*MAP_MAX_SIZE] = {0};

  for (;;)
  {
    umm x, y;
    bool ok = (EatUMM(&input, &x) && EatChar(&input, ',') && EatUMM(&input, &y));
    ASSERT(ok);

    EatWhitespace(&input);

    template_map[y*map_size + x] = 1;

    u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
    Copy(map, template_map, sizeof(map));

    Queue_Entry queue[MAP_MAX_SIZE*MAP_MAX_SIZE];
    umm queue_len = 1;
    queue[0].key = 0;
    queue[0].pos = V2S(0, 0);
    map[0] = 1;

    bool result = false;
    while (queue_len > 0)
    {
      Queue_Entry c = Dequeue(queue, &queue_len);

      if (V2S_Equal(c.pos, V2S(map_size-1, map_size-1)))
      {
        result = true;
        break;
      }

      for (umm i = 0; i < 4; ++i)
      {
        V2S pos  = V2S_Add(c.pos, (V2S[4]){ V2S(0, 1), V2S(0, -1), V2S(-1, 0), V2S(1, 0) }[i]);
        umm key  = c.key + 1;

        if (V2S_InBounds(pos, 0, map_size) && map[pos.y*map_size + pos.x] == 0)
        {
          map[pos.y*map_size + pos.x] = 1;
          ASSERT(queue_len < ARRAY_LEN(queue));
          queue[queue_len++] = (Queue_Entry){ .pos = pos, .key = key };
          DecreaseKey(queue, queue_len, pos, key, 0);
        }
      }
    }

    if (!result)
    {
      printf("Part 2 result: %llu,%llu\n", x, y);
      break;
    }
  }
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	printf("Part 2 result: %llu\n", Part1(input, 71, 1024));
  Part2(input, 71);

	return 0;
}
