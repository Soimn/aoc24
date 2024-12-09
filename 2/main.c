#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

char*
EatSpace(char* scan)
{
	while ((unsigned char)(*scan-1) < (unsigned char)0x20) ++scan;
	return scan;
}

char*
EatInt(char* scan, int* val)
{
	int result = 0;

	while ((unsigned char)(*scan-'0') < (unsigned char)10)
	{
		result *= 10;
		result += *scan&0xF;
		++scan;
	}

	*val = result;
	return scan;
}

unsigned int
Part1(char* input)
{
	unsigned int result = 0;

	for (char* scan = input;;)
	{
		scan = EatSpace(scan);
		if (*scan == 0) break;

		int first, second;
		scan = EatInt(scan, &first);
		scan = EatSpace(scan);
		scan = EatInt(scan, &second);

		int diff     = second - first;
		int abs_diff = (diff < 0 ? -diff : diff);
		bool should_be_neg = (diff < 0);
		bool is_safe = (abs_diff >= 1 && abs_diff <= 3);

		int prev = second;
		while (*scan != '\r')
		{
			int cur;
			scan = EatSpace(scan);
			scan = EatInt(scan, &cur);

			int diff     = cur - prev;
			int abs_diff = (diff < 0 ? -diff : diff);
			is_safe = (is_safe && diff < 0 == should_be_neg && abs_diff >= 1 && abs_diff <= 3);

			prev = cur;
		}

		if (is_safe) result += 1;

		scan += sizeof("\r\n")-1;
	}

	return result;
}

unsigned int
Part2(char* input)
{
	return 0;
}

int
main(int argc, char** argv)
{
	if (argc != 2) fprintf(stderr, "Invalid Arguments. Expected: day2 input_file\n");
	else
	{
		FILE* input_file = fopen(argv[1], "rb");
		if (input_file == 0) fprintf(stderr, "Failed to open input file\n");
		else
		{
			fseek(input_file, 0, SEEK_END);
			size_t input_file_size = ftell(input_file);
			rewind(input_file);
			
			char* input = malloc(input_file_size + 1);
			if (input == 0 || fread(input, 1, input_file_size, input_file) != input_file_size) fprintf(stderr, "Failed to read input file\n");
			else
			{
				printf("Part 1 result: %u\n", Part1(input));
				printf("Part 2 result: %u\n", Part2(input));
			}

			fclose(input_file);
		}
	}

	return 0;
}
