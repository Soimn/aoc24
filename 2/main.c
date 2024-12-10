#include "../aoc.h"

int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

	for (;;)
	{
		EatSpace(&input);
		if (input.len == 0) break;

		smm sequence[8];
		umm sequence_len = 0;
		for (;;)
		{
			ASSERT(input.len != 0 && sequence_len < ARRAY_LEN(sequence));

			EatSpace(&input);
			EatSMM(&input, &sequence[sequence_len++]);

			if (EatPrefix(&input, STRING("\r\n"))) break;
			else                                   continue;
		}

		smm sequence_diffs[ARRAY_LEN(sequence)-1];
		umm sequence_diffs_len = sequence_len-1;

		smm majority_sign = 0;
		for (umm i = 1; i < sequence_len; ++i)
		{
			sequence_diffs[i-1] = sequence[i] - sequence[i-1];
			majority_sign += (sequence[i-1] < sequence[i] ? 1 : -1);
		}

		if (majority_sign < 0)
		{
			for (umm i = 0; i < sequence_diffs_len; ++i) sequence_diffs[i] = -sequence_diffs[i];
		}

		#define IS_SAFE(DIFF) ((umm)((DIFF)-1) < (umm)3)

		umm i = 0;
		while (i < sequence_diffs_len && IS_SAFE(sequence_diffs[i])) ++i;

		bool is_part1_safe = (i == sequence_diffs_len);

		if (i >= sequence_diffs_len-1 || i != 0 && IS_SAFE(sequence_diffs[i-1] + sequence_diffs[i]))
		{
			i += 1;
		}
		else if (i == 0 && IS_SAFE(sequence_diffs[i+1]) || IS_SAFE(sequence_diffs[i] + sequence_diffs[i+1]))
		{
			i += 2;
		}

		while (i < sequence_diffs_len && IS_SAFE(sequence_diffs[i])) ++i;

		bool is_part2_safe = (i >= sequence_diffs_len);

		if (is_part1_safe) part1_result += 1;
		if (is_part2_safe) part2_result += 1;
	}

	printf("Part 1 results: %llu\nPart 2 results: %llu\n", part1_result, part2_result);

	return 0;
}
