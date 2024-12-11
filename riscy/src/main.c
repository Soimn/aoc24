#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

#define S8_MIN  0x80
#define S16_MIN 0x8000
#define S32_MIN 0x80000000
#define S64_MIN 0x8000000000000000LL
#define S8_MAX  0x7F
#define S16_MAX 0x7FFF
#define S32_MAX 0x7FFFFFFF
#define S64_MAX 0x7FFFFFFFFFFFFFFFLL

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#define U8_MAX  0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL

typedef s64 smm;
typedef u64 umm;

#define SMM_MIN S64_MIN
#define SMM_MAX S64_MAX
#define UMM_MAX U64_MAX

typedef u8 bool;
#define true 1
#define false 0

typedef float f32;
typedef double f64;

void
AssertHandler(char* file, int line, char* expr)
{
	fprintf(stderr, "ASSERTION FAILED\n%s(%d): %s\n", file, line, expr);
	exit(-1);
}

int IsDebuggerPresent();

#define ASSERT(EX) ((EX) ? 1 : (IsDebuggerPresent() ? *(volatile int*)0 = 0 : AssertHandler(__FILE__, __LINE__, #EX)))
#define NOT_IMPLEMENTED ASSERT(!"NOT_IMPLEMENTED")

#define ARRAY_LEN(A) (sizeof(A)/sizeof(0[A]))

#define ALIGNOF(T) _alignof(T)

typedef struct String
{
	u8* data;
	u32 len;
} String;

#define STRING(S) (String){ .data = (u8*)(S), .len = sizeof(S)-1 }
#define MS_STRING(S) { .data = (u8*)(S), .len = sizeof(S)-1 }

bool
String_Match(String s0, String s1)
{
	bool result = (s0.len == s1.len);

	for (umm i = 0; result && i < s0.len; ++i) result = (s0.data[i] == s1.data[i]);

	return result;
}

bool
Char_IsAlpha(u8 c)
{
	return ((u8)((c&0xDF)-'A') <= (u8)('Z' - 'A'));
}

bool
Char_IsDigit(u8 c)
{
	return ((u8)(c-'0') < (u8)10);
}

u8
Char_UncheckedToUpper(u8 c)
{
	return c&0xDF;
}

bool
String_MatchCaseInsensitive(String s0, String s1)
{
	bool result = (s0.len == s1.len);

	for (umm i = 0; result && i < s0.len; ++i)
	{
		u8 mask = (Char_IsAlpha(s0.data[i]) ? 0xDF : 0xFF);
		result = ((u8)((s0.data[i] ^ s1.data[i]) & mask) == 0);
	}

	return result;
}

bool
String_MatchCaseInsensitiveUnchecked(String s0, String s1)
{
	bool result = (s0.len == s1.len);

	for (umm i = 0; result && i < s0.len; ++i)
	{
		result = ((u8)((s0.data[i] ^ s1.data[i]) & 0xDF) == 0);
	}

	return result;
}

void
Copy(void* dst, void* src, umm len)
{
	u8* bdst = dst;
	u8* bsrc = src;
	for (umm i = 0; i < len; ++i) bdst[i] = bsrc[i];
}

u8*
LoadFile(char* filepath)
{
	u8* result = 0;

	FILE* file = fopen(filepath, "rb");
	
	struct __stat64 stat;
	if (file != 0 && _stat64(filepath, &stat) == 0)
	{
		u8* content = malloc(stat.st_size + 1);

		if (fread(content, 1, stat.st_size, file) != stat.st_size)
		{
			free(content);
		}
		else
		{
			content[stat.st_size] = 0;
			result = content;
		}
	}

	fclose(file);

	return result;
}

void
Error(u32 line, u8* line_pos, u8* pos, s64 len, char* message_format, ...)
{

	char* postfix = pos + len;
	char* scan = postfix;
	while (*scan != 0 && *scan != '\n' && *scan != '\r') ++scan;

	va_list args;
	va_start(args, message_format);
	fprintf(stderr, "(%u:%lld): ", line, 1 + (pos - line_pos));
	vfprintf(stderr, message_format, args);
	if (*pos == 0) fprintf(stderr, "\x1B[31;1mEnd of File\x1B[0m");
	else           fprintf(stderr, "\n%.*s\x1B[31;1m%.*s\x1B[0m\x1B[1m%.*s\n", (int)(pos - line_pos), line_pos, (int)len, pos, (int)(scan - postfix), postfix);
	va_end(args);
}

typedef enum Mnemonic
{
	Mnemonic_lui,
	Mnemonic_auipc,
	Mnemonic_addi,
	Mnemonic_slti,
	Mnemonic_sltiu,
	Mnemonic_xori,
	Mnemonic_ori,
	Mnemonic_andi,
	Mnemonic_slli,
	Mnemonic_srli,
	Mnemonic_srai,
	Mnemonic_add,
	Mnemonic_sub,
	Mnemonic_sll,
	Mnemonic_slt,
	Mnemonic_sltu,
	Mnemonic_xor,
	Mnemonic_srl,
	Mnemonic_sra,
	Mnemonic_or,
	Mnemonic_and,
	Mnemonic_lb,
	Mnemonic_lh,
	Mnemonic_lw,
	Mnemonic_lbu,
	Mnemonic_lhu,
	Mnemonic_sb,
	Mnemonic_sh,
	Mnemonic_sw,
	Mnemonic_jal,
	Mnemonic_jalr,
	Mnemonic_beq,
	Mnemonic_bne,
	Mnemonic_blt,
	Mnemonic_bge,
	Mnemonic_bltu,
	Mnemonic_bgeu,
	Mnemonic_call,
	Mnemonic_li,
} Mnemonic;

String Mnemonic_Strings[] = {
	[Mnemonic_lui]   = MS_STRING("lui"),
	[Mnemonic_auipc] = MS_STRING("auipc"),
	[Mnemonic_addi]  = MS_STRING("addi"),
	[Mnemonic_slti]  = MS_STRING("slti"),
	[Mnemonic_sltiu] = MS_STRING("sltiu"),
	[Mnemonic_xori]  = MS_STRING("xori"),
	[Mnemonic_ori]   = MS_STRING("ori"),
	[Mnemonic_andi]  = MS_STRING("andi"),
	[Mnemonic_slli]  = MS_STRING("slli"),
	[Mnemonic_srli]  = MS_STRING("srli"),
	[Mnemonic_srai]  = MS_STRING("srai"),
	[Mnemonic_add]   = MS_STRING("add"),
	[Mnemonic_sub]   = MS_STRING("sub"),
	[Mnemonic_sll]   = MS_STRING("sll"),
	[Mnemonic_slt]   = MS_STRING("slt"),
	[Mnemonic_sltu]  = MS_STRING("sltu"),
	[Mnemonic_xor]   = MS_STRING("xor"),
	[Mnemonic_srl]   = MS_STRING("srl"),
	[Mnemonic_sra]   = MS_STRING("sra"),
	[Mnemonic_or]    = MS_STRING("or"),
	[Mnemonic_and]   = MS_STRING("and"),
	[Mnemonic_lb]    = MS_STRING("lb"),
	[Mnemonic_lh]    = MS_STRING("lh"),
	[Mnemonic_lw]    = MS_STRING("lw"),
	[Mnemonic_lbu]   = MS_STRING("lbu"),
	[Mnemonic_lhu]   = MS_STRING("lhu"),
	[Mnemonic_sb]    = MS_STRING("sb"),
	[Mnemonic_sh]    = MS_STRING("sh"),
	[Mnemonic_sw]    = MS_STRING("sw"),
	[Mnemonic_jal]   = MS_STRING("jal"),
	[Mnemonic_jalr]  = MS_STRING("jalr"),
	[Mnemonic_beq]   = MS_STRING("beq"),
	[Mnemonic_bne]   = MS_STRING("bne"),
	[Mnemonic_blt]   = MS_STRING("blt"),
	[Mnemonic_bge]   = MS_STRING("bge"),
	[Mnemonic_bltu]  = MS_STRING("bltu"),
	[Mnemonic_bgeu]  = MS_STRING("bgeu"),
	[Mnemonic_call]  = MS_STRING("call"),
	[Mnemonic_li]    = MS_STRING("li"),
};

typedef u8 Reg;

struct { String string; Reg reg; } Reg_Strings[] = {
	{ MS_STRING("x0"),   0 },
	{ MS_STRING("x1"),   1 },
	{ MS_STRING("x2"),   2 },
	{ MS_STRING("x3"),   3 },
	{ MS_STRING("x4"),   4 },
	{ MS_STRING("x5"),   5 },
	{ MS_STRING("x6"),   6 },
	{ MS_STRING("x7"),   7 },
	{ MS_STRING("x8"),   8 },
	{ MS_STRING("x9"),   9 },
	{ MS_STRING("x10"), 10 },
	{ MS_STRING("x11"), 11 },
	{ MS_STRING("x12"), 12 },
	{ MS_STRING("x13"), 13 },
	{ MS_STRING("x14"), 14 },
	{ MS_STRING("x15"), 15 },
	{ MS_STRING("x16"), 16 },
	{ MS_STRING("x17"), 17 },
	{ MS_STRING("x18"), 18 },
	{ MS_STRING("x19"), 19 },
	{ MS_STRING("x20"), 20 },
	{ MS_STRING("x21"), 21 },
	{ MS_STRING("x22"), 22 },
	{ MS_STRING("x23"), 23 },
	{ MS_STRING("x24"), 24 },
	{ MS_STRING("x25"), 25 },
	{ MS_STRING("x26"), 26 },
	{ MS_STRING("x27"), 27 },
	{ MS_STRING("x28"), 28 },
	{ MS_STRING("x29"), 29 },
	{ MS_STRING("x30"), 30 },
	{ MS_STRING("x31"), 31 },

	// aliases
	{ MS_STRING("zero"),   0 },
	{ MS_STRING("ra"),     1 },
	{ MS_STRING("sp"),     2 },
	{ MS_STRING("gp"),     3 },
	{ MS_STRING("tp"),     4 },
	{ MS_STRING("t0"),     5 },
	{ MS_STRING("t1"),     6 },
	{ MS_STRING("t2"),     7 },
	{ MS_STRING("s0"),     8 },
	{ MS_STRING("fp"),     8 },
	{ MS_STRING("s1"),     9 },
	{ MS_STRING("a0"),    10 },
	{ MS_STRING("a1"),    11 },
	{ MS_STRING("a2"),    12 },
	{ MS_STRING("a3"),    13 },
	{ MS_STRING("a4"),    14 },
	{ MS_STRING("a5"),    15 },
	{ MS_STRING("a6"),    16 },
	{ MS_STRING("a7"),    17 },
	{ MS_STRING("s2"),    18 },
	{ MS_STRING("s3"),    19 },
	{ MS_STRING("s4"),    20 },
	{ MS_STRING("s5"),    21 },
	{ MS_STRING("s6"),    22 },
	{ MS_STRING("s7"),    23 },
	{ MS_STRING("s8"),    24 },
	{ MS_STRING("s9"),    25 },
	{ MS_STRING("s10"),   26 },
	{ MS_STRING("s11"),   27 },
	{ MS_STRING("t3"),    28 },
	{ MS_STRING("t4"),    29 },
	{ MS_STRING("t5"),    30 },
	{ MS_STRING("t6"),    31 },
};

typedef enum Token_Kind
{
	Token_Invalid = 0,
	Token_EOF,

	Token_Mnemonic,
	Token_Reg,
	Token_Ident,
	Token_Int,
	Token_String,
	Token_OpenParen,
	Token_CloseParen,
	Token_Colon,
	Token_Dot,
	Token_Comma,
} Token_Kind;

typedef struct Token
{
	Token_Kind kind;
	u32 line;
	u32 len;
	u8* line_pos;
	u8* pos;
	union
	{
		Mnemonic mnemonic;
		Reg reg;
		String ident;
		struct
		{
			s64 integer;
			u8 integer_len;
		};
		String string;
	};
} Token;

typedef struct Lexer
{
	u8* content;
	u8* cursor;
	u8* line_cursor;
	u32 line;
	Token token;
} Lexer;

Lexer
Lexer_Init(u8* content)
{
	Lexer lexer = (Lexer){
		.content     = content,
		.cursor      = content,
		.line_cursor = content,
		.line        = 1,
	};

	Token Lexer_NextToken(Lexer* lexer);
	Lexer_NextToken(&lexer);
	return lexer;
}

Token
Lexer_NextToken(Lexer* lexer)
{
	Token token = { .kind = Token_Invalid };

	for (;;)
	{
		if ((u8)(*lexer->cursor-1) < (u8)0x20)
		{
			if (*lexer->cursor == '\n')
			{
				lexer->line_cursor = lexer->cursor + 1;
				++lexer->line;
			}
			++lexer->cursor;
		}
		else if (*lexer->cursor == '#')
		{
			while (*lexer->cursor != 0 && *lexer->cursor != '\n') ++lexer->cursor;
		}
		else break;
	}

	u8* start_of_token = lexer->cursor;

	u8 c = *lexer->cursor;
	lexer->cursor += (c != 0);

	switch (c)
	{
		case 0: token.kind = Token_EOF; break;

		case '(': token.kind = Token_OpenParen;  break;
		case ')': token.kind = Token_CloseParen; break;
		case ':': token.kind = Token_Colon;      break;
		case '.': token.kind = Token_Dot;        break;
		case ',': token.kind = Token_Comma;      break;

		default:
		{
			if (Char_IsAlpha(c) || c == '_')
			{
				while (Char_IsAlpha(*lexer->cursor) || Char_IsDigit(*lexer->cursor) || *lexer->cursor == '_') ++lexer->cursor;

				String ident = {
					.data = start_of_token,
					.len = (u32)(lexer->cursor - start_of_token)
				};

				token.kind  = Token_Ident;
				token.ident = ident;

				for (umm i = 0; i < ARRAY_LEN(Mnemonic_Strings); ++i)
				{
					if (String_MatchCaseInsensitiveUnchecked(ident, Mnemonic_Strings[i]))
					{
						token.kind     = Token_Mnemonic;
						token.mnemonic = i;
						break;
					}
				}

				for (umm i = 0; i < ARRAY_LEN(Reg_Strings); ++i)
				{
					if (String_MatchCaseInsensitive(ident, Reg_Strings[i].string))
					{
						token.kind  = Token_Reg;
						token.reg   = Reg_Strings[i].reg;
						break;
					}
				}
			}
			else if (Char_IsDigit(c) || c == '-' || c == '+')
			{
				bool encountered_errors = false;

				s64 value       = c&0xF;	
				bool is_neg     = (c == '-');
				umm digit_count = 1;

				if (c == '-' || c == '+')
				{
					for (;;)
					{
						while (*lexer->cursor != 0 && *lexer->cursor != '\n' && *lexer->cursor <= 0x20) ++lexer->cursor;

						if (*lexer->cursor == '-' || *lexer->cursor == '+')
						{
							is_neg = (*lexer->cursor == '-' ? !is_neg : is_neg);
							++lexer->cursor;
							continue;
						}
						else break;
					}

					if (!Char_IsDigit(*lexer->cursor))
					{
						//// ERROR
						Error(lexer->line, lexer->line_cursor, start_of_token, lexer->cursor - start_of_token, "Missing digits after sign prefix");
						encountered_errors = true;
					}
					else
					{
						value = *lexer->cursor&0xF;
						++lexer->cursor;
					}
				}

				if (!encountered_errors)
				{
					s64 base = 10;
					if (value == 0)
					{
						if      (Char_UncheckedToUpper(*lexer->cursor) == 'X') base = 16;
						else if (Char_UncheckedToUpper(*lexer->cursor) == 'B') base = 2;
						
						if (base != 10)
						{
							++lexer->cursor;
							digit_count = 0;
						}
					}

					bool has_overflown = false;
					while (!encountered_errors)
					{
						s64 digit;
						if      (Char_IsDigit(*lexer->cursor))                                     digit = *lexer->cursor&0xF;
						else if ((u8)(Char_UncheckedToUpper(*lexer->cursor)-'A') <= (u8)('F'-'A')) digit = (*lexer->cursor&0x7) + 9;
						else break;

						if (digit >= base)
						{
							//// ERROR
							Error(lexer->line, lexer->line_cursor, lexer->cursor, 1, "Digit is out of bounds for base %u literal", base);
							encountered_errors = true;
						}
						else
						{
							value = value*base + digit;
							has_overflown = (has_overflown || value > U32_MAX);
							digit_count += 1;
							++lexer->cursor;
						}
					}

					if (!encountered_errors)
					{
						if (digit_count == 0)
						{
							//// ERROR
							Error(lexer->line, lexer->line_cursor, start_of_token, lexer->cursor - start_of_token, "Missing digits after base prefix");
							encountered_errors = true;
						}
						else if (has_overflown)
						{
							//// ERROR
							Error(lexer->line, lexer->line_cursor, start_of_token, lexer->cursor - start_of_token, "Numeric literal does not fit in 32 bits");
							encountered_errors = true;
						}
						else
						{
							token.kind        = Token_Int;
							token.integer     = (is_neg ? -value : value);
							token.integer_len = (!is_neg && base == 16 ? (digit_count <= 2 ? 1 : digit_count <= 4 ? 2 : 4) : 4);
						}
					}
				}
			}
			else if (c == '"')
			{
				while (*lexer->cursor != 0 && *lexer->cursor != '\n' && *lexer->cursor != '"') ++lexer->cursor;

				if (*lexer->cursor != '"')
				{
					//// ERROR
					Error(lexer->line, lexer->line_cursor, start_of_token, lexer->cursor - start_of_token, "Unterminated string literal");
				}
				else
				{
					token.kind   = Token_String;
					token.string = (String){
						.data = start_of_token + 1,
						.len  = (u32)(lexer->cursor - (start_of_token+1)),
					};
					++lexer->cursor;
				}
			}
			else
			{
				//// ERROR
				Error(lexer->line, lexer->line_cursor, lexer->cursor-1, 1, "Unknown symbol");
			}
		} break;
	}

	token.line     = lexer->line;
	token.len      = (u32)(lexer->cursor - start_of_token);
	token.line_pos = lexer->line_cursor;
	token.pos      = start_of_token;

	lexer->token = token;
	return lexer->token;
}

bool
Lexer_IsToken(Lexer* lexer, Token_Kind kind)
{
	return (lexer->token.kind == kind);
}

bool
Expect(Lexer* lexer, Token_Kind kind, void* data)
{
	bool result = Lexer_IsToken(lexer, kind);

	if (!result)
	{
		//// ERROR
		Token t = lexer->token;
		
		char* t_str = 0;
		switch (kind)
		{
			case Token_Mnemonic:   t_str = "instruction mnemonic"; break;
			case Token_Reg:        t_str = "register name";        break;
			case Token_Ident:      t_str = "identifier";           break;
			case Token_Int:        t_str = "integer";              break;
			case Token_OpenParen:  t_str = "open parenthesis";     break;
			case Token_CloseParen: t_str = "close parenthesis";    break;
			case Token_Colon:      t_str = "colon";                break;
			case Token_Dot:        t_str = "dot";                  break;
			case Token_Comma:      t_str = "comma";                break;
		}

		Error(t.line, t.line_pos, t.pos, t.len, "Expected %s but got this shit instead", t_str);
	}
	else
	{
		switch (lexer->token.kind)
		{
			case Token_Mnemonic: *(Mnemonic*)data = lexer->token.mnemonic; break;
			case Token_Reg:      *(Reg*)data      = lexer->token.reg;      break;
			case Token_Int:      *(s64*)data      = lexer->token.integer;  break;
			case Token_Ident:    *(String*)data   = lexer->token.ident;    break;
		}

		Lexer_NextToken(lexer);
	}

	return result;
}

bool
IsImmInRange(Mnemonic mnemonic, s64 imm)
{
	bool is_shift = (mnemonic == Mnemonic_slli || mnemonic == Mnemonic_srli || mnemonic == Mnemonic_srai);
	bool is_20bit = (mnemonic == Mnemonic_lui || mnemonic == Mnemonic_auipc || mnemonic == Mnemonic_jal);
	bool is_32bit = (mnemonic == Mnemonic_call || mnemonic == Mnemonic_li);

	bool result;
	if      (is_shift) result = (imm >= 0 && imm < 32);
	else if (is_20bit) result = (imm >= -(1LL << 19) && imm < (1LL << 19));
	else if (is_32bit) result = ((s32)imm == imm);
	else               result = (imm >= -(1LL << 11) && imm < (1LL << 11));

	return result;
}

bool
ExpectAddress(Lexer* lexer, Mnemonic mnemonic, s64* offset, Reg* reg)
{
	bool encountered_errors = false;

	*offset = 0;
	if (Lexer_IsToken(lexer, Token_Int))
	{
		if (!IsImmInRange(mnemonic, lexer->token.integer))
		{
			//// ERROR
			Token t = lexer->token;
			Error(t.line, t.line_pos, t.pos, t.len, "Offset is out of range");
			encountered_errors = true;
		}
		else
		{
			*offset = lexer->token.integer;
			Lexer_NextToken(lexer);
		}
	}
	else if (Lexer_IsToken(lexer, Token_Ident))
	{
		//// ERROR
		Token t = lexer->token;
		Error(t.line, t.line_pos, t.pos, t.len, "Cannot use label value here");
		encountered_errors = true;
	}

	return (!encountered_errors && Expect(lexer, Token_OpenParen, 0) && Expect(lexer, Token_Reg, reg) && Expect(lexer, Token_CloseParen, 0));
}

#define MAX_LABELS 1024
#define MAX_FIXUPS 8129

typedef struct Label
{
	String name;
	s32 pos;
} Label;

typedef struct Fixup
{
	u16 label_idx;
	u16 pos;
	Mnemonic mnemonic;
} Fixup;

bool
AddLabel(String name, s32 pos, Label label_stack[MAX_LABELS], umm* label_cursor, u16* label_idx)
{
	bool encountered_errors = false;

	u16 i = 0;
	for (; i < *label_cursor; ++i)
	{
		if (String_Match(name, label_stack[i].name)) break;
	}

	if (i < *label_cursor)
	{
		if (pos != -1)
		{
			if (label_stack[i].pos == -1) label_stack[i].pos = pos;
			else
			{
				//// ERROR
				fprintf(stderr, "Duplicate label %.*s\n", (int)name.len, name.data);
				encountered_errors = true;
			}
		}

		*label_idx = i;
	}
	else
	{
		if (*label_cursor >= MAX_LABELS)
		{
			fprintf(stderr, "Too many labels\n");
			encountered_errors = true;
		}
		else
		{
			label_stack[*label_cursor] = (Label){
				.name = name,
				.pos  = pos,
			};

			*label_idx = (u16)*label_cursor;
			*label_cursor += 1;
		}
	}

	return !encountered_errors;
}

bool
AddFixup(Mnemonic mnemonic, u16 pos, String name, Fixup fixup_stack[MAX_FIXUPS], umm* fixup_cursor, Label label_stack[MAX_LABELS], umm* label_cursor)
{
	bool encountered_errors = false;

	u16 label_idx;
	if (!AddLabel(name, -1, label_stack, label_cursor, &label_idx)) encountered_errors = true;
	else
	{
		if (*fixup_cursor >= MAX_FIXUPS)
		{
			//// ERROR
			fprintf(stderr, "Too many label fixups\n");
			encountered_errors = true;
		}
		else
		{
			Fixup* f = &fixup_stack[*fixup_cursor];
			*fixup_cursor += 1;

			*f = (Fixup){
				.label_idx = label_idx,
				.pos       = pos,
				.mnemonic  = mnemonic,
			};
		}
	}

	return !encountered_errors;
}

bool
ExpectIntOrLabel(Lexer* lexer, Mnemonic mnemonic, u16 pos, s64* offset, Fixup fixup_stack[MAX_FIXUPS], umm* fixup_cursor, Label label_stack[MAX_LABELS], umm* label_cursor)
{
	bool encountered_errors = false;

	Token t = lexer->token;
	if (t.kind == Token_Int)
	{
		if (!IsImmInRange(mnemonic, t.integer))
		{
			//// ERROR
			Error(t.line, t.line_pos, t.pos, t.len, "Integer is out of bounds");
			encountered_errors = true;
		}
		else
		{
			*offset = t.integer;
			Lexer_NextToken(lexer);
		}
	}
	else if (t.kind == Token_Ident)
	{
		if (!AddFixup(mnemonic, pos, t.ident, fixup_stack, fixup_cursor, label_stack, label_cursor)) encountered_errors = true;
		else
		{
			*offset = 0;
			Lexer_NextToken(lexer);
		}
	}
	else
	{
		//// ERROR
		Error(t.line, t.line_pos, t.pos, t.len, "Expected either an integer or a label but got this shit instead");
		encountered_errors = true;
	}

	return !encountered_errors;
}

s32
SignExtend(u32 word, u8 len)
{
	return (s32)(word << (32 - len)) >> (32 - len);
}

u32
PackRType(Reg rd, u8 funct3, Reg rs1, Reg rs2, u8 funct7)
{
	return 0x33 | ((u32)rd << 7) | ((u32)funct3 << 12) | ((u32)rs1 << 15) | ((u32)rs2 << 20) | ((u32)funct7 << 25);
}

void
UnpackRType(u32 word, Reg* rd, u8* funct3, Reg* rs1, Reg* rs2, u8* funct7)
{
	*rd     = (u8)((word >>  7) & 0x1F);
	*funct3 = (u8)((word >> 12) & 0x07);
	*rs1    = (u8)((word >> 15) & 0x1F);
	*rs2    = (u8)((word >> 20) & 0x1F);
	*funct7 = (u8)((word >> 25) & 0x7F);
}

u32
PackIType(u8 opcode, Reg rd, u8 funct3, Reg rs1, s64 imm)
{
	return opcode | ((u32)rd << 7) | ((u32)funct3 << 12) | ((u32)rs1 << 15) | ((u32)imm << 20);
}

void
UnpackIType(u32 word, Reg* rd, u8* funct3, Reg* rs1, s64* imm)
{
	*rd     = (u8)((word >>  7) & 0x1F);
	*funct3 = (u8)((word >> 12) & 0x07);
	*rs1    = (u8)((word >> 15) & 0x1F);
	*imm    = (s32)word >> 20;
}

u32
PackUType(u8 opcode, Reg rd, s64 imm)
{
	return opcode | ((u32)rd << 7) | ((u32)imm << 12);
}

void
UnpackUType(u32 word, Reg* rd, s64* imm)
{
	*rd     = (u8)((word >> 7) & 0x1F);
	*imm    = word >> 12;
}

u32
PackSType(u8 opcode, u8 imm_lo, u8 funct3, Reg rs1, Reg rs2, u8 imm_hi)
{
	return opcode | ((u32)imm_lo << 7) | ((u32)funct3 << 12) | ((u32)rs1 << 15) | ((u32)rs2 << 20) | ((u32)imm_hi << 25);
}

void
UnpackSType(u32 word, u8* imm_lo, u8* funct3, Reg* rs1, Reg* rs2, u8* imm_hi)
{
	*imm_lo = (u8)((word >>  7) & 0x1F);
	*funct3 = (u8)((word >> 12) & 0x07);
	*rs1    = (u8)((word >> 15) & 0x1F);
	*rs2    = (u8)((word >> 20) & 0x1F);
	*imm_hi = (u8)(word >> 25);
}

u32
PackJType(Reg rd, s64 imm)
{
	return 0x6F | ((u32)rd << 7) | ((u32)((imm >> 12) & 0xFF) << 12) | ((u32)((imm >> 11) & 0x1) << 20) | ((u32)((imm >> 1) & 0x3FF) << 21) | ((u32)((imm >> 20) & 0x1) << 31);
}

void
UnpackJType(u32 word, Reg* rd, s64* imm)
{
	*rd  = (u8)((word >>  7) & 0x1F);
	*imm = (((word >> 12) & 0xFF) << 12) | (((word >> 20) & 0x1) << 11) | (((word >> 21) & 0x3FF) << 1) | (((word >> 31) & 0x1) << 20);
}

u32
PackBType(u8 funct3, Reg rs1, Reg rs2, s64 imm)
{
	return 0x63 | ((u32)((imm >> 11) & 0x1) << 7) | ((u32)((imm >> 1) & 0xF) << 8) | ((u32)funct3 << 12) | ((u32)rs1 << 15) | ((u32)rs2 << 20) | ((u32)((imm >> 5) & 0x3F) << 25) | ((u32)((imm >> 12) & 0x1) << 31);
}

void
UnpackBType(u32 word, u8* funct3, Reg* rs1, Reg* rs2, s64* imm)
{
	*funct3 = (u8)((word >> 12) & 0x07);
	*rs1    = (u8)((word >> 15) & 0x1F);
	*rs2    = (u8)((word >> 20) & 0x1F);
	*imm    = (((word >> 7) & 0x1) << 11) | (((word >> 8) & 0xF) << 1) | (((word >> 25) & 0x3F) << 5) | (((word >> 31) & 0x1) << 12);
}

#define PUSH_DATA(D, L) do {                                                    \
		if (memory_cursor > U16_MAX - (L)) fprintf(stderr, "Program is too large"); \
		else                                                                        \
		{                                                                           \
			Copy(Memory + memory_cursor, (D), (L));                                   \
			memory_cursor += (L);                                                     \
		}                                                                           \
	} while (0)

#define PUSH_INSTRUCTION(I) PUSH_DATA(&(u32){I}, sizeof(u32))

static u8 Memory[U16_MAX] = {0};

void
PF_print_char(u32 regs[32])
{
	printf("%c", (u8)regs[10]);
}

void
PF_divmod(u32 regs[32])
{
	s32 a = regs[10];
	s32 b = regs[11];
	regs[10] = a / b;
	regs[11] = a % b;
}

void
PF_mul(u32 regs[32])
{
	s32 a = regs[10];
	s32 b = regs[11];
	regs[10] = a * b;
}

typedef struct Predefined_Func
{
	String name;
	void (*func)(u32 regs[32]);
} Predefined_Func;

#define PREDEFINED_FUNC_BASE 0x10000

Predefined_Func PredefinedFuncs[] = {
	{ MS_STRING("print_char"), PF_print_char },
	{ MS_STRING("divmod"),     PF_divmod     },
	{ MS_STRING("mul"),        PF_mul        },
};

bool
Assemble(u8* code, u8* input, u16* main_addr)
{
	bool encountered_errors = false;

	Lexer _lexer = Lexer_Init(code);
	Lexer* lexer = &_lexer;

	umm memory_cursor = 0;

	Label label_stack[MAX_LABELS];
	umm label_cursor = 0;

	Fixup fixup_stack[MAX_FIXUPS];
	umm fixup_cursor = 0;

	encountered_errors = !AddLabel(STRING("input"), -1, label_stack, &label_cursor, &(u16){0});

	for (umm i = 0; !encountered_errors && i < ARRAY_LEN(PredefinedFuncs); ++i)
	{
		encountered_errors = !AddLabel(PredefinedFuncs[i].name, (u32)(PREDEFINED_FUNC_BASE + i*2), label_stack, &label_cursor, &(u16){0});
	}

	while (!encountered_errors && !Lexer_IsToken(lexer, Token_EOF))
	{
		Token token = lexer->token;
		Lexer_NextToken(lexer);

		if (token.kind == Token_Ident)
		{
			if (String_Match(token.ident, STRING("rep8")))
			{
				s64 rep_count;
				s64 value;
				if (!Expect(lexer, Token_Int, &rep_count) || !Expect(lexer, Token_Int, &value)) encountered_errors = true;
				else
				{
					for (s64 i = 0; i < rep_count; ++i) PUSH_DATA(&(u8){value & 0xFF}, 1);
				}
			}
			else if (String_Match(token.ident, STRING("rep32")))
			{
				s64 rep_count;
				s64 value;
				if (!Expect(lexer, Token_Int, &rep_count) || !Expect(lexer, Token_Int, &value)) encountered_errors = true;
				else
				{
					for (s64 i = 0; i < rep_count; ++i) PUSH_DATA(&(u32){(u32)value}, sizeof(u32));
				}
			}
			else if (!Expect(lexer, Token_Colon, 0)) encountered_errors = true;
			else
			{
				encountered_errors = !AddLabel(token.ident, (s32)memory_cursor, label_stack, &label_cursor, &(u16){0});
			}
		}
		else if (token.kind == Token_Int)
		{
			PUSH_DATA(&token.integer, token.integer_len);
		}
		else if (token.kind == Token_String)
		{
			PUSH_DATA(token.string.data, token.string.len);
		}
		else if (token.kind == Token_Mnemonic)
		{
			switch (token.mnemonic)
			{
				case Mnemonic_call:
				{
					String ident;
					if (!Expect(lexer, Token_Ident, &ident)) encountered_errors = true;
					else
					{
						if (!AddFixup(Mnemonic_call, (u16)memory_cursor, ident, fixup_stack, &fixup_cursor, label_stack, &label_cursor)) encountered_errors = true;
						else
						{
							PUSH_INSTRUCTION(PackUType(0x37, 5, 0));       // lui t0, func >> 12
							PUSH_INSTRUCTION(PackIType(0x67, 1, 0, 5, 0)); // jalr ra, t0, func & 0xFFF
						}
					}
				} break;

				case Mnemonic_li:
				{
					Reg rd;
					s64 imm;
					if (!Expect(lexer, Token_Reg, &rd) || !Expect(lexer, Token_Comma, 0) || !ExpectIntOrLabel(lexer, Mnemonic_li, (u16)memory_cursor, &imm, fixup_stack, &fixup_cursor, label_stack, &label_cursor)) encountered_errors = true;
					else
					{
						if ((u32)(imm & 0xFFF) < 0x800)
						{
							PUSH_INSTRUCTION(PackUType(0x37, rd, ((u32)imm >> 12)));        // lui t0, (val >> 12)
							PUSH_INSTRUCTION(PackIType(0x13, rd, 6, rd, (u32)imm & 0xFFF)); // ori rs1, rs1, val & 0xFFF
						}
						else
						{
							PUSH_INSTRUCTION(PackUType(0x37, rd, ~((u32)imm >> 12)));       // lui t0, ~(val >> 12)
							PUSH_INSTRUCTION(PackIType(0x13, rd, 4, rd, (u32)imm & 0xFFF)); // xori rs1, rs1, val & 0xFFF
						}
					}
				} break;

				case Mnemonic_addi:
				case Mnemonic_slti:
				case Mnemonic_sltiu:
				case Mnemonic_xori:
				case Mnemonic_ori:
				case Mnemonic_andi:
				case Mnemonic_slli:
				case Mnemonic_srli:
				case Mnemonic_srai:
				{
					Reg rd;
					Reg rs1;
					s64 imm;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && Expect(lexer, Token_Reg, &rs1) && Expect(lexer, Token_Comma, 0) && ExpectIntOrLabel(lexer, token.mnemonic, (u16)memory_cursor, &imm, fixup_stack, &fixup_cursor, label_stack, &label_cursor))
					{
						u8 funct3;
						switch (token.mnemonic)
						{
							case Mnemonic_addi:  funct3 = 0; break;
							case Mnemonic_slti:  funct3 = 2; break;
							case Mnemonic_sltiu: funct3 = 3; break;
							case Mnemonic_xori:  funct3 = 4; break;
							case Mnemonic_ori:   funct3 = 6; break;
							case Mnemonic_andi:  funct3 = 7; break;
							case Mnemonic_slli:  funct3 = 1; break;
							case Mnemonic_srli:  funct3 = 5; break;
							case Mnemonic_srai:  funct3 = 5; break;
						}

						if (funct3 == 1 || funct3 == 5) imm &= 0x1F;

						if (token.mnemonic == Mnemonic_srai) imm |= (1 << 10);

						PUSH_INSTRUCTION(PackIType(0x13, rd, funct3, rs1, imm));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_lui:
				case Mnemonic_auipc:
				{
					Reg rd;
					s64 imm;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && ExpectIntOrLabel(lexer, token.mnemonic, (u16)memory_cursor, &imm, fixup_stack, &fixup_cursor, label_stack, &label_cursor))
					{
						PUSH_INSTRUCTION(PackUType((token.mnemonic == Mnemonic_lui ? 0x37 : 0x17), rd, imm));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_add:
				case Mnemonic_sub:
				case Mnemonic_slt:
				case Mnemonic_sltu:
				case Mnemonic_xor:
				case Mnemonic_or:
				case Mnemonic_and:
				case Mnemonic_sll:
				case Mnemonic_srl:
				case Mnemonic_sra:
				{
					Reg rd;
					Reg rs1;
					Reg rs2;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && Expect(lexer, Token_Reg, &rs1) && Expect(lexer, Token_Comma, 0) && Expect(lexer, Token_Reg, &rs2))
					{
						u8 funct7 = (token.mnemonic == Mnemonic_sub || token.mnemonic == Mnemonic_sra ? 1 << 6 : 0);
						u8 funct3;
						switch (token.mnemonic)
						{
							case Mnemonic_add:  funct3 = 0; break;
							case Mnemonic_sub:  funct3 = 0; break;
							case Mnemonic_slt:  funct3 = 2; break;
							case Mnemonic_sltu: funct3 = 3; break;
							case Mnemonic_xor:  funct3 = 4; break;
							case Mnemonic_or:   funct3 = 6; break;
							case Mnemonic_and:  funct3 = 7; break;
							case Mnemonic_sll:  funct3 = 1; break;
							case Mnemonic_srl:  funct3 = 5; break;
							case Mnemonic_sra:  funct3 = 5; break;
						}


						PUSH_INSTRUCTION(PackRType(rd, funct3, rs1, rs2, funct7));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_lb:
				case Mnemonic_lh:
				case Mnemonic_lw:
				case Mnemonic_lbu:
				case Mnemonic_lhu:
				{
					Reg rd;
					s64 offset;
					Reg rs1;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && ExpectAddress(lexer, token.mnemonic, &offset, &rs1))
					{
						u8 funct3;
						switch (token.mnemonic)
						{
							case Mnemonic_lb:  funct3 = 0; break;
							case Mnemonic_lh:  funct3 = 1; break;
							case Mnemonic_lw:  funct3 = 2; break;
							case Mnemonic_lbu: funct3 = 4; break;
							case Mnemonic_lhu: funct3 = 5; break;
						}

						PUSH_INSTRUCTION(PackIType(0x03, rd, funct3, rs1, offset));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_sb:
				case Mnemonic_sh:
				case Mnemonic_sw:
				{
					Reg rs2;
					s64 offset;
					Reg rs1;
					if (Expect(lexer, Token_Reg, &rs2) && Expect(lexer, Token_Comma, 0) && ExpectAddress(lexer, token.mnemonic, &offset, &rs1))
					{
						u8 funct3;
						switch (token.mnemonic)
						{
							case Mnemonic_sb: funct3 = 0; break;
							case Mnemonic_sh: funct3 = 1; break;
							case Mnemonic_sw: funct3 = 2; break;
						}

						PUSH_INSTRUCTION(PackSType(0x23, (u8)(offset & 0x1F), funct3, rs1, rs2, (u32)offset >> 5));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_jal:
				{
					Reg rd;
					s64 offset;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && ExpectIntOrLabel(lexer, Mnemonic_jal, (u16)memory_cursor, &offset, fixup_stack, &fixup_cursor, label_stack, &label_cursor))
					{
						PUSH_INSTRUCTION(PackJType(rd, offset));
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_jalr:
				{
					Reg rd;
					Reg rs1;
					if (Expect(lexer, Token_Reg, &rd) && Expect(lexer, Token_Comma, 0) && Expect(lexer, Token_Reg, &rs1) && Expect(lexer, Token_Comma, 0))
					{
						Token t = lexer->token;
						s64 offset;
						if (!Expect(lexer, Token_Int, &offset)) encountered_errors = true;
						else
						{
							if (!IsImmInRange(Mnemonic_jalr, offset))
							{
								//// ERROR
								Error(t.line, t.line_pos, t.pos, t.len, "Offset is out of bounds");
								encountered_errors = true;
							}
							else
							{
								PUSH_INSTRUCTION(PackIType(0x67, rd, 0, rs1, offset));
							}
						}
					}
					else encountered_errors = true;
				} break;

				case Mnemonic_beq:
				case Mnemonic_bne:
				case Mnemonic_blt:
				case Mnemonic_bge:
				case Mnemonic_bltu:
				case Mnemonic_bgeu:
				{
					Reg rs1;
					Reg rs2;
					s64 offset;
					if (Expect(lexer, Token_Reg, &rs1) && Expect(lexer, Token_Comma, 0) && Expect(lexer, Token_Reg, &rs2) && Expect(lexer, Token_Comma, 0) && ExpectIntOrLabel(lexer, token.mnemonic, (u16)memory_cursor, &offset, fixup_stack, &fixup_cursor, label_stack, &label_cursor))
					{
						u8 funct3;
						switch (token.mnemonic)
						{
							case Mnemonic_beq:  funct3 = 0; break;
							case Mnemonic_bne:  funct3 = 1; break;
							case Mnemonic_blt:  funct3 = 4; break;
							case Mnemonic_bge:  funct3 = 5; break;
							case Mnemonic_bltu: funct3 = 6; break;
							case Mnemonic_bgeu: funct3 = 7; break;
						}

						PUSH_INSTRUCTION(PackBType(funct3, rs1, rs2, offset));
					}
					else encountered_errors = true;
				} break;

				default:
				{
					ASSERT(!"Illegal Mnemonic");
				} break;
			}
		}
		else
		{
			//// ERROR
			Error(token.line, token.line_pos, token.pos, token.len, "Unexpected token");
			encountered_errors = true;
		}
	}

	if (!encountered_errors)
	{
		encountered_errors = !AddLabel(STRING("input"), (u16)memory_cursor, label_stack, &label_cursor, &(u16){0});
		umm input_len = 0;
		for (u8* scan = input; *scan != 0; ++scan) ++input_len;
		PUSH_DATA(input, input_len);
		PUSH_DATA(&(u32){0}, 1);

		for (umm i = 0; !encountered_errors && i < fixup_cursor; ++i)
		{
			Fixup fixup = fixup_stack[i];

			s32 pos = label_stack[fixup.label_idx].pos;
			if (pos == -1)
			{
				//// ERROR
				String name = label_stack[fixup.label_idx].name;
				fprintf(stderr, "Undefined label '%.*s' used\n", (int)name.len, name.data);
				encountered_errors = true;
			}
			else
			{
				u32* word = (u32*)&Memory[fixup.pos];
				u8 opcode = *word & 0x7F;
				s64 imm   = label_stack[fixup.label_idx].pos;

				if (!IsImmInRange(fixup.mnemonic, imm))
				{
					//// ERROR
					fprintf(stderr, "Some instructions use out of range labels as immediates\n");
					encountered_errors = true;
				}
				else
				{
					switch (fixup.mnemonic)
					{
						case Mnemonic_call:
						{
							Reg rd;
							UnpackUType(*word, &rd, &(s64){0});
							*word = PackUType(0x37, rd, imm >> 12);

							u8 funct3;
							Reg rs1;
							UnpackIType(*(word+1), &rd, &funct3, &rs1, &(s64){0});
							*(word+1) = PackIType(0x67, rd, funct3, rs1, imm & 0xFFF);
						} break;

						case Mnemonic_li:
						{
							if ((u32)(imm & 0xFFF) < 0x800)
							{
								Reg rd;
								UnpackUType(*word, &rd, &(s64){0});
								*word = PackUType(0x37, rd, ((u32)imm >> 12));
							}
							else
							{
								Reg rd;
								UnpackUType(*word, &rd, &(s64){0});
								*word = PackUType(0x37, rd, ~((u32)imm >> 12));
							}

							Reg rd;
							u8 funct3;
							Reg rs1;
							UnpackIType(*(word+1), &rd, &funct3, &rs1, &(s64){0});
							*(word+1) = PackIType(0x13, rd, funct3, rs1, imm & 0xFFF);
						} break;


						case Mnemonic_lui:
						case Mnemonic_auipc:
						{
							Reg rd;
							UnpackUType(*word, &rd, &(s64){0});
							*word = PackUType(opcode, rd, imm);
						} break;

						case Mnemonic_addi:
						case Mnemonic_slti:
						case Mnemonic_sltiu:
						case Mnemonic_xori:
						case Mnemonic_ori:
						case Mnemonic_andi:
						case Mnemonic_slli:
						case Mnemonic_srli:
						case Mnemonic_srai:
						{
							Reg rd;
							u8 funct3;
							Reg rs1;
							UnpackIType(*word, &rd, &funct3, &rs1, &(s64){0});
							*word = PackIType(opcode, rd, funct3, rs1, imm);
						} break;

						case Mnemonic_jal:
						{
							Reg rd;
							UnpackJType(*word, &rd, &(s64){0});
							*word = PackJType(rd, (s32)((u32)imm - (u32)fixup.pos) >> 1);
						} break;

						case Mnemonic_beq:
						case Mnemonic_bne:
						case Mnemonic_blt:
						case Mnemonic_bge:
						case Mnemonic_bltu:
						case Mnemonic_bgeu:
						{
							u8 funct3;
							Reg rs1;
							Reg rs2;
							UnpackBType(*word, &funct3, &rs1, &rs2, &(s64){0});
							*word = PackBType(funct3, rs1, rs2, (s32)((u32)imm - (u32)fixup.pos) >> 1);
						} break;
						
						default:
						{
							ASSERT(!"Illegal mnemonic for fixup");
						} break;
					}
				}
			}
		}
	}

	if (!encountered_errors)
	{
		u16 main_idx;
		if (!AddLabel(STRING("main"), -1, label_stack, &label_cursor, &main_idx)) encountered_errors = true;
		else
		{
			if (label_stack[main_idx].pos == -1)
			{
				//// ERROR
				fprintf(stderr, "Missing main function\n");
				encountered_errors = true;
			}
			else *main_addr = label_stack[main_idx].pos;
		}
	}

	return !encountered_errors;
}

u32
ALUOp(u8 funct3, u8 funct7, u32 a, u32 b)
{
	u32 result;

	switch (funct3)
	{
		case 0: result = a + (((funct7 >> 6) & 1) != 0 ? ~b + 1: b);        break;
		case 1: result = a << b;                                            break;
		case 2: result = ((s32)a < (s32)b);                                 break;
		case 3: result = (a < b);                                           break;
		case 4: result = a ^ b;                                             break;
		case 5: result = (((funct7 >> 6) & 1) == 0 ? (s32)a >> b : a >> b); break;
		case 6: result = a | b;                                             break;
		case 7: result = a & b;                                             break;
	}

	return result;
}

int
main(int argc, char** argv)
{
	int SetConsoleMode(void*,u32);
	int GetConsoleMode(void*,u32*);
	void* GetStdHandle(u32);
	void* console = GetStdHandle((u32)-11);
	u32 console_mode;
	GetConsoleMode(console, &console_mode);
	SetConsoleMode(console, console_mode | 4);

	if (argc != 3) fprintf(stderr, "Invalid Arguments. Expected: riscy asm_file problem_input\n");
	else
	{
		u8* code  = LoadFile(argv[1]);
		u8* input = LoadFile(argv[2]);

		if      (code  == 0) fprintf(stderr, "Failed to load asm file\n");
		else if (input == 0) fprintf(stderr, "Failed to load problem input\n");
		else
		{
			u16 main_addr;
			if (Assemble(code, input, &main_addr))
			{
				u32 halt_addr = 0xEB1CEB1C;

				u32 regs[32] = {0};
				regs[1] = halt_addr;  // ra
				regs[2] = 0xFFFF;     // sp
				regs[3] = 0;          // gp
				regs[4] = 0;          // tp
				regs[8] = 0xFFFF;     // fp

				u32 pc = main_addr;

				while (pc != halt_addr)
				{
					if (pc >= PREDEFINED_FUNC_BASE)
					{
						ASSERT((pc - PREDEFINED_FUNC_BASE)/2 < ARRAY_LEN(PredefinedFuncs));
						PredefinedFuncs[(pc - PREDEFINED_FUNC_BASE)/2].func(regs);
						pc = regs[1];
					}
					else
					{
						u32 word = *(u32*)&Memory[pc];
						u8 opcode = word & 0x7F;
					
						pc += 4;

						switch (opcode)
						{
							case 0x33: // RType
							{
								Reg rd;
								u8 funct3;
								Reg rs1;
								Reg rs2;
								u8 funct7;
								UnpackRType(word, &rd, &funct3, &rs1, &rs2, &funct7);
								regs[rd] = ALUOp(funct3, funct7, regs[rs1], regs[rs2]);
							} break;

							case 0x6F: // jal
							{
								Reg rd;
								s64 imm;
								UnpackJType(word, &rd, &imm);
								regs[rd] = pc;
								pc       = (pc-4) + (SignExtend((u32)imm, 20) << 1);
							} break;

							case 0x63: // BType
							{
								u8 funct3;
								Reg rs1;
								Reg rs2;
								s64 imm;
								UnpackBType(word, &funct3, &rs1, &rs2, &imm);
								
								bool should_branch;
								switch (funct3)
								{
									case 0: should_branch = (regs[rs1] == regs[rs2]);           break;
									case 1: should_branch = (regs[rs1] != regs[rs2]);           break;
									case 4: should_branch = ((s32)regs[rs1] < (s32)regs[rs2]);  break;
									case 5: should_branch = ((s32)regs[rs1] >= (s32)regs[rs2]); break;
									case 6: should_branch = (regs[rs1] < regs[rs2]);            break;
									case 7: should_branch = (regs[rs1] >= regs[rs2]);           break;
								}

								if (should_branch) pc = (pc-4) + (SignExtend((u32)imm, 12) << 1);
							} break;

							case 0x13: // immediate arith
							{
								Reg rd;
								u8 funct3;
								Reg rs1;
								s64 imm;
								UnpackIType(word, &rd, &funct3, &rs1, &imm);
								imm = SignExtend((u32)imm, 12);
								regs[rd] = ALUOp(funct3, 0, regs[rs1], (u32)imm);
							} break;

							case 0x03: // load
							{
								Reg rd;
								u8 funct3;
								Reg rs1;
								s64 offset;
								UnpackIType(word, &rd, &funct3, &rs1, &offset);

								umm addr = regs[rs1] + SignExtend((u32)offset, 12);
								if      (funct3 == 0) regs[rd] = (u32)SignExtend(Memory[addr]         & 0x0000007F,  7);
								else if (funct3 == 1) regs[rd] = (u32)SignExtend(*(u16*)&Memory[addr] & 0x00007FFF, 15);
								else if (funct3 == 2) regs[rd] = (u32)SignExtend(*(u32*)&Memory[addr] & 0x7FFFFFFF, 31);
								else if (funct3 == 4) regs[rd] = Memory[addr]         & 0x00FF;
								else                  regs[rd] = *(u16*)&Memory[addr] & 0xFFFF;
							} break;

							case 0x23: // store
							{
								u8 imm_lo;
								u8 funct3;
								Reg rs1;
								Reg rs2;
								u8 imm_hi;
								UnpackSType(word, &imm_lo, &funct3, &rs1, &rs2, &imm_hi);

								u32 offset = imm_lo | ((u32)imm_hi << 5);

								if      (funct3 == 0) Memory[regs[rs1] + SignExtend(offset, 12)] = (u8)(regs[rs2] & 0x00FF);
								else if (funct3 == 1) *(u16*)&Memory[regs[rs1] + SignExtend(offset, 12)] = (u16)(regs[rs2] & 0xFFFF);
								else                  *(u32*)&Memory[regs[rs1] + SignExtend(offset, 12)] = regs[rs2];

								;
							} break;

							case 0x67: // jalr
							{
								Reg rd;
								u8 funct3;
								Reg rs1;
								s64 imm;
								UnpackIType(word, &rd, &funct3, &rs1, &imm);
								regs[rd] = pc;
								pc       = (regs[rs1] + SignExtend((u32)imm, 12)) & ~(u32)1;
							} break;

							case 0x37: // lui
							{
								Reg rd;
								s64 imm;
								UnpackUType(word, &rd, &imm);
								regs[rd] = (u32)imm << 12;
							} break;

							case 0x17: // auipc
							{
								Reg rd;
								s64 imm;
								UnpackUType(word, &rd, &imm);
								regs[rd] = (pc-4) + ((u32)imm << 12);
							} break;

							default:
							{
								//// ERROR
								ASSERT(!"Illegal opcode");
							} break;
						}

						regs[0] = 0; // NOTE: instead of gating writes to x0, just overwrite them at the end of the cycle
					}
				}
			}
		}
	}

	return 0;
}
