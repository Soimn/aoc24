#include "../aoc.h"

bool
IsSorted(u8* sequence, umm sequence_len, u128 order[99])
{
	for (umm i = 0; i < sequence_len-1; ++i)
	{
		for (umm j = i+1; j < sequence_len; ++j)
		{
			if (!U128_IsSet(order[sequence[i]], sequence[j])) return false;
		}
	}

	return true;
}

#if 0
void
Toposort(u8 stack[32], umm* stack_cursor, umm node, u128 edges[99])
{
	for (umm i = 0; i < 128; ++i)
	{
		if (i != node && U128_IsSet(edges[node], i)) Toposort(stack, stack_cursor, i, edges);
	}

	ASSERT(*stack_cursor < ARRAY_LEN(stack));
	stack[*stack_cursor] = node;
	*stack_cursor += 1;
}

void
Sort(u8 sequence[32], umm sequence_len, u128 order[99])
{
	u128 mask = U128(0, 0);
	for (umm i = 0; i < sequence_len; ++i)
	{
		mask = U128_Set(mask, sequence[i]); 
	}

	u128 edges[99];
	for (umm i = 0; i < ARRAY_LEN(edges); ++i) edges[i] = U128_And(order[i], mask);

	

	u8 stack[32];
	umm stack_cursor;
	Toposort(stack, &stack_cursor, root, edges);
}
#endif

void
Sort(u8 sequence[32], umm sequence_len, u128 order[99])
{
	for (umm i = 0; i < sequence_len-1; ++i)
	{
		umm root = i;

		for (umm j = i+1; j < sequence_len; ++j)
		{
			if (j != root && U128_IsSet(order[sequence[j]], sequence[root]))
			{
				root = j;
				j = i;
			}
		}

		u8 tmp = sequence[i];
		sequence[i] = sequence[root];
		sequence[root] = tmp;
	}
}


int
main(int argc, char** argv)
{
	String input;
	if (!LoadInput(argc, argv, &input)) return -1;

	umm part1_result = 0;
	umm part2_result = 0;

	bool id_present[99] = {0};

	u128 order[99] = {0};

	for (;;)
	{
		umm lhs, rhs;
		if (EatUMM(&input, &lhs) && EatChar(&input, '|') && EatUMM(&input, &rhs))
		{
			ASSERT(lhs > 0 && lhs < 100 && rhs < 100);
			id_present[lhs] = true;
			id_present[rhs] = true;
			order[lhs] = U128_Set(order[lhs], rhs);

			EatPrefix(&input, STRING("\r\n"));
		}
		else break;
	}

	EatWhitespace(&input);

	u8 ids[ARRAY_LEN(id_present)];
	umm ids_len = 0;

	for (umm i = 0; i < ARRAY_LEN(id_present); ++i)
	{
		if (id_present[i])
		{
			order[i] = U128_Set(order[i], i);
			ids[ids_len++] = (u8)i;
		}
	}

#if 0
	for (;;)
	{
		bool made_progress = false;

		for (umm i = 0; i < ids_len; ++i)
		{
			u8 id = ids[i];

			u128 old_order = order[id];

			for (umm j = 0; j < ids_len; ++j)
			{
				order[id] = U128_Or(order[id], order[ids[j]]);
			}

			if (!U128_Equal(order[id], old_order)) made_progress = true;
		}

		if (!made_progress) break;
	}
#endif

	while (input.len != 0)
	{
		u8 sequence[32];
		umm sequence_len = 0;

		for (;;)
		{
			ASSERT(sequence_len < ARRAY_LEN(sequence));
			umm element;
			EatUMM(&input, &element);
			sequence[sequence_len++] = (u8)element;

			if (EatChar(&input, ',')) continue;
			else                      break;
		}

		EatWhitespace(&input);

		if (IsSorted(sequence, sequence_len, order)) part1_result += sequence[sequence_len/2];
		else
		{
			Sort(sequence, sequence_len, order);
			part2_result += sequence[sequence_len/2];
		}
	}

	printf("Part 1 result: %llu\nPart 2 result: %llu\n", part1_result, part2_result);

	return 0;
}
