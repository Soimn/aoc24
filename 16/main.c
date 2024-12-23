#include "../aoc.h"

#define MAP_MAX_SIZE 141

typedef struct Queue_Entry
{
  umm key;
  V2S pos;
  V2S dir;
} Queue_Entry;

typedef struct Queue
{
  umm max_key;
  umm len;
  Queue_Entry entries[MAP_MAX_SIZE*MAP_MAX_SIZE];
} Queue;

void
Heapify(Queue* queue)
{
  for (umm i = (queue->len-1)/2; i >= 0; i /= 2)
  {
    umm k        = queue->entries[i].key;
    umm swap_idx = i;
    if (2*i < queue->len && k > queue->entries[2*i].key)
    {
      k        = queue->entries[2*i].key;
      swap_idx = 2*i;
    }

    if (2*i+1 < queue->len && k > queue->entries[2*i+1].key)
    {
      k        = queue->entries[2*i+1].key;
      swap_idx = 2*i+1;
    }

    Queue_Entry tmp = queue->entries[swap_idx];
    queue->entries[swap_idx] = queue->entries[i];
    queue->entries[i]        = tmp;

    if (swap_idx == i || i == 0) break;
  }
}

void
Explore(Queue* queue, umm seen[MAP_MAX_SIZE*MAP_MAX_SIZE][4], umm map_size, umm key, V2S pos, V2S dir)
{
  u8 dir_idx;
  if (dir.y == 0) dir_idx = (dir.x == 1 ? 0 : 1);
  else            dir_idx = (dir.y == 1 ? 2 : 3);

  if (seen[pos.y*map_size + pos.x][dir_idx] < key || key >= queue->max_key) return;
  seen[pos.y*map_size + pos.x][dir_idx] = key;

  ASSERT(queue->len < ARRAY_LEN(queue->entries));
  queue->entries[queue->len++] = (Queue_Entry){ .key = key, .pos = pos, .dir = dir };

  Heapify(queue);
}

Queue_Entry
Visit(Queue* queue)
{
  ASSERT(queue->len > 0);

  Queue_Entry entry = queue->entries[0];

  queue->entries[0] = queue->entries[--queue->len];

  Heapify(queue);

  return entry;
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm seen[MAP_MAX_SIZE*MAP_MAX_SIZE][4];
  umm map_size = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);

  V2S start_pos = V2S(-1, -1);
  V2S end_pos   = V2S(-1, -1);
  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      seen[j*map_size + i][0] = U32_MAX;
      seen[j*map_size + i][1] = U32_MAX;
      seen[j*map_size + i][2] = U32_MAX;
      seen[j*map_size + i][3] = U32_MAX;

      if (c == 'S') start_pos = V2S((s32)i, (s32)j);
      if (c == 'E') end_pos   = V2S((s32)i, (s32)j);
      if (c == '#')
      {
        seen[j*map_size + i][0] = 0;
        seen[j*map_size + i][1] = 0;
        seen[j*map_size + i][2] = 0;
        seen[j*map_size + i][3] = 0;
      }

      map[j*map_size + i] = c;
    }
  }

  ASSERT(start_pos.x != -1 && start_pos.y != -1);
  ASSERT(end_pos.x   != -1 && end_pos.y   != -1);

  Queue queue = {
    .max_key = U32_MAX,
    .len     = 0,
  };
  
  Explore(&queue, seen, map_size, 0,    start_pos, V2S( 1,  0));
  Explore(&queue, seen, map_size, 1000, start_pos, V2S( 0,  1));
  Explore(&queue, seen, map_size, 1000, start_pos, V2S( 0, -1));
  Explore(&queue, seen, map_size, 2000, start_pos, V2S(-1,  0));

  while (queue.len > 0)
  {
    Queue_Entry entry = Visit(&queue);

    if (V2S_Equal(entry.pos, end_pos)) queue.max_key = MIN(queue.max_key, entry.key);

    V2S ldir = V2S(-entry.dir.y, entry.dir.x);
    V2S rdir = V2S(entry.dir.y, -entry.dir.x);

    Explore(&queue, seen, map_size, entry.key + 1,    V2S_Add(entry.pos, entry.dir), entry.dir);
    Explore(&queue, seen, map_size, entry.key + 1000, entry.pos,                          ldir);
    Explore(&queue, seen, map_size, entry.key + 1000, entry.pos,                          rdir);
  }

  part1_result = queue.max_key;

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
