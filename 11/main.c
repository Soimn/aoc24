#include "../aoc.h"

#include <windows.h>

umm
Part1(String input)
{
  umm buffer_size = 1ULL << 28;
  umm* a_buffer = VirtualAlloc(0, buffer_size*sizeof(umm), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  umm* b_buffer = VirtualAlloc(0, buffer_size*sizeof(umm), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

  umm len = 0;
  for (;;)
  {
    umm val;
    if (!EatUMM(&input, &val)) break;
    else
    {
      ASSERT(len < buffer_size);
      a_buffer[len++] = (umm)val;
      EatSpace(&input);
    }
  }

  for (umm i = 0; i < 25; ++i)
  {
    umm old_len = len;
    len = 0;

    for (umm j = 0; j < old_len; ++j)
    {
      if (a_buffer[j] == 0) b_buffer[len++] = a_buffer[j] + 1;
      else
      {
        umm num_digits = 1;
        for (umm x = a_buffer[j]/10; x != 0; x/= 10) ++num_digits;

        if (num_digits % 2 != 0) b_buffer[len++] = a_buffer[j]*2024;
        else
        {
          umm mult = 1;
          for (umm k = 0; k < num_digits/2; ++k) mult *= 10;

          ASSERT(len+2 < buffer_size);
          b_buffer[len++] = a_buffer[j] / mult;
          b_buffer[len++] = a_buffer[j] % mult;
        }
      }
    }

    umm* tmp = a_buffer;
    a_buffer = b_buffer;
    b_buffer = tmp;
  }

  return len;
}

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  part1_result = Part1(input);

#if 0
todo = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
rules = {}

i = 0;
while len(todo) != 0:
    i += 1
    n = todo.pop(0)
    if n == 0:
        rule = [1]
    elif len(str(n)) % 2 != 0:
        rule = [2024*n]
    else:
        rule = [int(str(n)[:len(str(n))//2]), int(str(n)[len(str(n))//2:])]

    if n not in rules:
        rules[n] = rule

    for x in rule:
        if x not in rules and x not in todo:
            todo.append(x)

ns = sorted(list(rules.keys()))

for n in ns:
    print("{" + f"{n}, {ns.index(rules[n][0])}, {ns.index(rules[n][1]) if len(rules[n]) == 2 else len(ns)}" + "},")
#endif

  umm buffer_size = 1ULL << 20;
  umm* buffers = VirtualAlloc(0, 2*buffer_size*sizeof(umm), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  umm* a_buffer = &buffers[0];
  umm* b_buffer = &buffers[buffer_size];
  umm len = 0;

  for (;;)
  {
    umm val;
    if (!EatUMM(&input, &val)) break;
    else
    {
      ASSERT(len < buffer_size);
      a_buffer[len++] = val;

      EatSpace(&input);
    }
  }

  umm rules[54][3] = {
    {0, 1, 54},
    {1, 30, 54},
    {2, 38, 54},
    {3, 40, 54},
    {4, 41, 54},
    {5, 44, 54},
    {6, 45, 54},
    {7, 46, 54},
    {8, 47, 54},
    {9, 48, 54},
    {20, 2, 0},
    {24, 2, 4},
    {26, 2, 6},
    {28, 2, 8},
    {32, 3, 2},
    {36, 3, 6},
    {40, 4, 0},
    {48, 4, 8},
    {56, 5, 6},
    {57, 5, 7},
    {60, 6, 0},
    {67, 6, 7},
    {72, 7, 2},
    {77, 7, 7},
    {80, 8, 0},
    {84, 8, 4},
    {86, 8, 6},
    {91, 9, 1},
    {94, 9, 4},
    {96, 9, 6},
    {2024, 10, 11},
    {2048, 10, 17},
    {2457, 11, 19},
    {2608, 12, 8},
    {2867, 13, 21},
    {2880, 13, 24},
    {3277, 14, 23},
    {3686, 15, 26},
    {4048, 16, 17},
    {6032, 20, 14},
    {6072, 20, 22},
    {8096, 24, 29},
    {9184, 27, 25},
    {9456, 28, 18},
    {10120, 49, 54},
    {12144, 50, 54},
    {14168, 51, 54},
    {16192, 52, 54},
    {18216, 53, 54},
    {20482880, 31, 35},
    {24579456, 32, 43},
    {28676032, 34, 39},
    {32772608, 36, 33},
    {36869184, 37, 42},
  };

  umm map_buffers[2*(ARRAY_LEN(rules) + 1)] = {0};
  umm* map = &map_buffers[0];
  umm* nmap = &map_buffers[ARRAY_LEN(map_buffers)/2];

  for (umm i = 0; i < 75; ++i)
  {
    umm old_len = len;
    len = 0;
    for (umm j = 0; j < old_len; ++j)
    {
      umm k = 0;
      for (; k < ARRAY_LEN(rules); ++k)
      {
        if (a_buffer[j] == rules[k][0])
        {
          break;
        }
      }

      if (k < ARRAY_LEN(rules)) map[k] += 1;
      else
      {
        umm num_digits = 1;
        for (umm x = a_buffer[j]/10; x != 0; x/= 10) ++num_digits;

        if (num_digits % 2 != 0) b_buffer[len++] = a_buffer[j]*2024;
        else
        {
          umm mult = 1;
          for (umm k = 0; k < num_digits/2; ++k) mult *= 10;

          ASSERT(len+2 < buffer_size);
          b_buffer[len++] = a_buffer[j] / mult;
          b_buffer[len++] = a_buffer[j] % mult;
        }
      }
    }

    umm* tmp = a_buffer;
    a_buffer = b_buffer;
    b_buffer = tmp;

    for (umm j = 0; j < ARRAY_LEN(rules); ++j)
    {
      nmap[rules[j][1]] += map[j];
      nmap[rules[j][2]] += map[j];
    }

    tmp = map;
    map = nmap;
    nmap = tmp;

    for (umm j = 0; j < ARRAY_LEN(rules); ++j) nmap[j] = 0;
  }

  for (umm i = 0; i < ARRAY_LEN(rules); ++i) part2_result += map[i];
  part2_result += len;

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
