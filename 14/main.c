#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

  umm quads[4] = {0};

  s32 sim_time = 100;

  #define WIDTH  101
  #define HEIGHT 103

  typedef struct Robot
  {
    V2S pos;
    V2S vel;
  } Robot;

  Robot robots[500];
  umm robots_len = 0;

  for (;;)
  {
    if (!EatPrefix(&input, STRING("p="))) break;
    else
    {
      V2S p, v;
      EatS32(&input, &p.x);
      EatChar(&input, ',');
      EatS32(&input, &p.y);
      EatPrefix(&input, STRING(" v="));
      EatS32(&input, &v.x);
      EatChar(&input, ',');
      EatS32(&input, &v.y);
      EatWhitespace(&input);

      ASSERT(robots_len < ARRAY_LEN(robots));
      robots[robots_len++] = (Robot){ .pos = p, .vel = v };

      V2S end = V2S_Add(p, V2S_Scale(v, sim_time));
      end.x = (WIDTH  + end.x % WIDTH)  % WIDTH;
      end.y = (HEIGHT + end.y % HEIGHT) % HEIGHT;

      if (end.x < WIDTH/2 && end.y < HEIGHT/2) quads[0] += 1;
      if (end.x > WIDTH/2 && end.y < HEIGHT/2) quads[1] += 1;
      if (end.x > WIDTH/2 && end.y > HEIGHT/2) quads[2] += 1;
      if (end.x < WIDTH/2 && end.y > HEIGHT/2) quads[3] += 1;
    }
  }

  part1_result = quads[0]*quads[1]*quads[2]*quads[3];

  umm min_factor = UMM_MAX;
  umm min_idx    = 0;
  for (umm i = 0; i < WIDTH*HEIGHT; ++i)
  {
    umm quads[4] = {0};

    for (umm j = 0; j < robots_len; ++j)
    {
      V2S end = V2S_Add(robots[j].pos, V2S_Scale(robots[j].vel, (s32)i));
      end.x = (WIDTH  + end.x % WIDTH)  % WIDTH;
      end.y = (HEIGHT + end.y % HEIGHT) % HEIGHT;

      if (end.x < WIDTH/2 && end.y < HEIGHT/2) quads[0] += 1;
      if (end.x > WIDTH/2 && end.y < HEIGHT/2) quads[1] += 1;
      if (end.x > WIDTH/2 && end.y > HEIGHT/2) quads[2] += 1;
      if (end.x < WIDTH/2 && end.y > HEIGHT/2) quads[3] += 1;
    }

    umm safety_factor = quads[0]*quads[1]*quads[2]*quads[3];
    if (safety_factor < min_factor)
    {
      min_factor = safety_factor;
      min_idx    = i;
    }
  }

  part2_result = min_idx;

  u8 map[WIDTH*HEIGHT];
  for (umm i = 0; i < ARRAY_LEN(map); ++i) map[i] = '.';

  for (umm j = 0; j < robots_len; ++j)
  {
    robots[j].pos = V2S_Add(robots[j].pos, V2S_Scale(robots[j].vel, (s32)min_idx));
    map[((HEIGHT + robots[j].pos.y % HEIGHT) % HEIGHT)*WIDTH + (WIDTH + robots[j].pos.x % WIDTH) % WIDTH] = '#';
  }

  for (umm y = 0; y < HEIGHT; ++y)
  {
    printf("%.*s\n", WIDTH, &map[y*WIDTH]);
  }

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
