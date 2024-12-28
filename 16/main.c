#include "../aoc.h"

#define MAP_MAX_SIZE 141

typedef struct Cell
{
  u64 keys[4];
  u64 o;
} Cell;

Cell map[MAP_MAX_SIZE*MAP_MAX_SIZE];

typedef struct Queue_Entry
{
  u64 key;
  s32 pos;
  u8 dir;
} Queue_Entry;

#define QUEUE_CAP (MAP_MAX_SIZE*MAP_MAX_SIZE*4*3)
Queue_Entry Queue[QUEUE_CAP];

void
Enqueue(Queue_Entry queue[QUEUE_CAP], umm* queue_len, Queue_Entry entry)
{
  ASSERT(*queue_len < QUEUE_CAP);
  queue[*queue_len] = entry;
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

int
main(int argc, char** argv)
{
  String input;
  if (!LoadInput(argc, argv, &input)) return -1;

  umm part1_result = 0;
  umm part2_result = 0;

  s32 map_size = 0;

  while ((umm)map_size < input.len && input.data[map_size] != '\r') ++map_size;

  ASSERT(map_size <= MAP_MAX_SIZE);
  ASSERT((map_size-1)*(map_size+2) + map_size-1 > 0);
  ASSERT((umm)((map_size-1)*(map_size+2) + map_size-1) < input.len);

  V2S start = V2S(-1, -1);
  V2S end   = V2S(-1, -1);

  for (s32 j = 0; j < map_size; ++j)
  {
    for (s32 i = 0; i < map_size; ++i)
    {
      u8 c = input.data[j*(map_size+2) + i];

      map[j*map_size + i] = (Cell){
        .keys = { U64_MAX, U64_MAX, U64_MAX, U64_MAX },
        .o    = 0,
      };

      if      (c == 'S') start = V2S(i, j);
      else if (c == 'E') end   = V2S(i, j);

      if (c == '#')
      {
        map[j*map_size + i] = (Cell){0};
      }
    }
  }

  ASSERT(start.x != -1 && start.y != -1);
  ASSERT(end.x != -1 && end.y != -1);

  umm queue_len = 0;
  Enqueue(Queue, &queue_len, (Queue_Entry){ .key =    0, .pos = start.y*map_size + start.x, .dir = 0 });
  Enqueue(Queue, &queue_len, (Queue_Entry){ .key = 2000, .pos = start.y*map_size + start.x, .dir = 2 });

  V2S dir_map[4] = { V2S(1, 0), V2S(0, 1), V2S(-1, 0), V2S(0, -1) };

  u64 min_path_len = U64_MAX;
  while (queue_len > 0)
  {
    Queue_Entry entry = Dequeue(Queue, &queue_len);

    V2S pos = V2S(entry.pos % map_size, entry.pos / map_size);

    if      (entry.key > min_path_len) break;
    else if (V2S_Equal(pos, end)) min_path_len = entry.key;

    V2S npos   = V2S_Add(pos, dir_map[entry.dir]);
    s32 npos_i = npos.y*map_size + npos.x;
    if (V2S_InBounds(npos, 0, map_size) && entry.key + 1 < map[npos_i].keys[entry.dir])
    {
      map[npos_i].keys[entry.dir] = entry.key + 1;

      Enqueue(Queue, &queue_len, (Queue_Entry){ .key = entry.key + 1, .pos = npos_i, .dir = entry.dir });
    }

    for (umm i = 0; i < 2; ++i)
    {
      u8 dir = (entry.dir + 2*i + 1) & 3;

      if (entry.key + 1000 < map[entry.pos].keys[dir])
      {
        map[entry.pos].keys[dir] = entry.key + 1000;

        Enqueue(Queue, &queue_len, (Queue_Entry){ .key = entry.key + 1000, .pos = entry.pos, .dir = dir });
      }
    }
  }

  part1_result = min_path_len;

  queue_len = 0;
  for (u8 i = 0; i < 4; ++i)
  {
    if (map[end.y*map_size + end.x].keys[i] == min_path_len)
    {
      Enqueue(Queue, &queue_len, (Queue_Entry){ .key = min_path_len, .pos = end.y*map_size + end.x, .dir = i });
    }
  }

  map[end.y*map_size + end.x].o = 1;
  part2_result += 1;

  while (queue_len > 0)
  {
    Queue_Entry entry = Dequeue(Queue, &queue_len);

    V2S pos = V2S(entry.pos % map_size, entry.pos / map_size);

    if (entry.key > 1)
    {
      V2S npos   = V2S_Sub(pos, dir_map[entry.dir]);
      s32 npos_i = npos.y*map_size + npos.x;

      if (V2S_InBounds(npos, 0, map_size) && entry.key - 1 == map[npos_i].keys[entry.dir])
      {
        part2_result += (map[npos_i].o == 0);

        if (!V2S_Equal(npos, start))
        {
          map[npos_i].keys[entry.dir] = UMM_MAX;
          map[npos_i].o               = 1;
          Enqueue(Queue, &queue_len, (Queue_Entry){ .key = entry.key - 1, .pos = npos_i, .dir = entry.dir });
        }
      }
    }

    if (entry.key > 1001)
    {
      for (umm i = 0; i < 2; ++i)
      {
        u8 dir     = (entry.dir + 2*i + 1) & 3;
        V2S npos   = V2S_Sub(pos, dir_map[dir]);
        s32 npos_i = npos.y*map_size + npos.x;

        if (V2S_InBounds(npos, 0, map_size) && entry.key - 1001 == map[npos_i].keys[dir])
        {
          part2_result += (map[npos_i].o == 0);

          if (!V2S_Equal(npos, start))
          {
            map[npos_i].keys[dir] = UMM_MAX;
            map[npos_i].o         = 1;
            Enqueue(Queue, &queue_len, (Queue_Entry){ .key = entry.key - 1001, .pos = npos_i, .dir = dir });
          }
        }
      }
    }
  }

  printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

  return 0;
}
