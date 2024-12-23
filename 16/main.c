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
  umm len;
  umm cap;
  Queue_Entry* e;
} Queue;

void
Queue_Enqueue(Queue* queue, umm key, V2S pos, V2S dir)
{
  ASSERT(queue->len < queue->cap);

  queue->e[queue->len++] = (Queue_Entry){ .key = key, .pos = pos, .dir = dir };

  for (umm i = (queue->len-2)/2; i < queue->len; i = (i-1)/2)
  {
    umm swap_idx = i;
    if (2*i+1 < queue->len && queue->e[2*i+1].key < queue->e[swap_idx].key) swap_idx = 2*i+1;
    if (2*i+2 < queue->len && queue->e[2*i+2].key < queue->e[swap_idx].key) swap_idx = 2*i+2;

    if (swap_idx == i) break;

    Queue_Entry tmp = queue->e[i];
    queue->e[i]        = queue->e[swap_idx];
    queue->e[swap_idx] = tmp;

    if (i == 0) break;
  }
}

Queue_Entry
Queue_Dequeue(Queue* queue)
{
  ASSERT(queue->len > 0);

  Queue_Entry result = queue->e[0];
  queue->e[0] = queue->e[--queue->len];

  for (umm i = 0; i < queue->len;)
  {
    umm swap_idx = i;
    if (2*i+1 < queue->len && queue->e[2*i+1].key < queue->e[swap_idx].key) swap_idx = 2*i+1;
    if (2*i+2 < queue->len && queue->e[2*i+2].key < queue->e[swap_idx].key) swap_idx = 2*i+2;

    if (swap_idx == i) break;

    Queue_Entry tmp = queue->e[i];
    queue->e[i]        = queue->e[swap_idx];
    queue->e[swap_idx] = tmp;

    i = swap_idx;
  }

  return result;
}

void
TracePath(umm (*min_map)[4], umm map_size, bool* seen, V2S pos, umm key, umm dir)
{
  if (key == 0) return;

  seen[pos.y*map_size + pos.x] = true;

  V2S dir_map[4] = { V2S(1, 0), V2S(0, 1), V2S(-1, 0), V2S(0, -1) };

  V2S npos = V2S_Sub(pos, dir_map[dir]);
  if (min_map[npos.y*map_size + npos.x][dir] == key - 1)
  {
    TracePath(min_map, map_size, seen, npos, key-1, dir);
  }

  if (key > 1000)
  {
    umm ldir = (dir+1)%4;
    V2S lpos = V2S_Sub(pos, dir_map[ldir]);
    if (min_map[lpos.y*map_size + lpos.x][ldir] == key - 1001)
    {
      TracePath(min_map, map_size, seen, lpos, key-1001, ldir);
    }

    umm rdir = (dir-1)%4;
    V2S rpos = V2S_Sub(pos, dir_map[rdir]);
    if (min_map[rpos.y*map_size + rpos.x][rdir] == key - 1001)
    {
      TracePath(min_map, map_size, seen, rpos, key-1001, rdir);
    }
  }
}

int
main(int argc, char** argv)
{
  String input;
  if (!LoadInput(argc, argv, &input)) return -1;

  umm part1_result = 0;
  umm part2_result = 0;

  u8 map[MAP_MAX_SIZE*MAP_MAX_SIZE];
  umm min_map[MAP_MAX_SIZE*MAP_MAX_SIZE][4];
  umm map_size = 0;

  while (map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT((map_size-1)*(map_size+2) + map_size-1 < input.len);

  V2S start = V2S(-1, -1);
  V2S end   = V2S(-1, -1);

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      if (c == 'S')
      {
        start = V2S((s32)i, (s32)j);
        c     = '.';
      }
      else if (c == 'E')
      {
        end = V2S((s32)i, (s32)j);
        c   = '.';
      }

      map[j*map_size + i] = c;
      min_map[j*map_size + i][0] = UMM_MAX;
      min_map[j*map_size + i][1] = UMM_MAX;
      min_map[j*map_size + i][2] = UMM_MAX;
      min_map[j*map_size + i][3] = UMM_MAX;
    }
  }

  umm queue_cap = MAP_MAX_SIZE*MAP_MAX_SIZE*4;
  Queue queue = {
    .len = 0,
    .cap = queue_cap,
    .e   = malloc(sizeof(Queue_Entry)*queue_cap),
  };

  Queue_Enqueue(&queue,    0, start, V2S( 1,  0));
  Queue_Enqueue(&queue, 2000, start, V2S(-1,  0));

  umm min_key = UMM_MAX;

  while (queue.len > 0)
  {
    Queue_Entry entry = Queue_Dequeue(&queue);

    u8 dir_idx;
    if (entry.dir.y == 0) dir_idx = (entry.dir.x == 1 ? 0 : 2);
    else                  dir_idx = (entry.dir.y == 1 ? 1 : 3);

    umm* min_map_key = &min_map[entry.pos.y*map_size + entry.pos.x][0];
    if (entry.key < min_map_key[dir_idx]) min_map_key[dir_idx] = entry.key;

    if (V2S_Equal(entry.pos, end) && entry.key < min_key) min_key = entry.key;

    V2S npos = V2S_Add(entry.pos, entry.dir);
    if (map[npos.y*map_size + npos.x] != '#' &&
        entry.key + 1 <= min_key             &&
        entry.key + 1 <= min_map[npos.y*map_size + npos.x][dir_idx])
    {
      Queue_Enqueue(&queue, entry.key + 1, npos, entry.dir);
    }

    if (entry.key + 1000 <= min_key)
    {
      if (entry.key + 1000 <= min_map_key[(dir_idx+1) % 4])
      {
        Queue_Enqueue(&queue, entry.key + 1000, entry.pos, V2S(-entry.dir.y,  entry.dir.x));
      }

      if (entry.key + 1000 <= min_map_key[(dir_idx-1)%4])
      {
        Queue_Enqueue(&queue, entry.key + 1000, entry.pos, V2S( entry.dir.y, -entry.dir.x));
      }
    }
  }

  part1_result = min_key;

  bool seen[MAP_MAX_SIZE*MAP_MAX_SIZE] = {0};
  for (umm i = 0; i < 4; ++i)
  {
    umm min_map_key = min_map[end.y*map_size + end.x][i];

    if (min_map_key == min_key)
    {
      TracePath(min_map, map_size, seen, end, min_map_key, i);
    }
  }

  for (umm j = 0; j < map_size; ++j)
  {
    for (umm i = 0; i < map_size; ++i)
    {
      part2_result += (seen[j*map_size + i] ? 1 : 0);
    }
  }

  printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

  return 0;
}
