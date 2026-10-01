#ifndef SCANNER_H
#define SCANNER_H

typedef enum ScannerToken {
	TOK_EOF = 0,
	TOK_ERROR = 256,

	/* tipos */
	TOK_KW_INT,
	TOK_KW_FLOAT,
	TOK_KW_DOUBLE,
	TOK_KW_CHAR,
	TOK_KW_BOOL,
	TOK_KW_VOID,

	/* control de flujo */
	TOK_KW_IF,
	TOK_KW_ELSE,
	TOK_KW_WHILE,
	TOK_KW_FOR,
	TOK_KW_FOREACH,

	/* declaraciones */
	TOK_KW_VAR,
	TOK_KW_CONST,
	TOK_KW_FUNC,
	TOK_KW_RETURN,
	TOK_KW_BREAK,
	TOK_KW_CONTINUE,

	/* funciones nativas */
	TOK_KW_PRINT,
	TOK_KW_INPUT,
	TOK_KW_EXIT,
	TOK_KW_ROLL,
	TOK_KW_SEED,

	TOK_IDENTIFIER,
	TOK_INT_LITERAL,
	TOK_FLOAT_LITERAL,
	TOK_STRING_LITERAL,
	TOK_CHAR_LITERAL,
	TOK_BOOL_LITERAL,

	TOK_INC,
	TOK_DEC,
	TOK_PRE_INC,
	TOK_POST_INC,
	TOK_PRE_DEC,
	TOK_POST_DEC,
	TOK_PLUS_ASSIGN,
	TOK_MINUS_ASSIGN,
	TOK_MUL_ASSIGN,
	TOK_DIV_ASSIGN,
	TOK_MOD_ASSIGN,
	TOK_ASSIGN,

	TOK_EQ,
	TOK_NEQ,
	TOK_LT,
	TOK_LE,
	TOK_GT,
	TOK_GE,

	TOK_AND,
	TOK_OR,
	TOK_NOT,

	TOK_PLUS,
	TOK_MINUS,
	TOK_MUL,
	TOK_DIV,
	TOK_MOD,

	/* operadores a nivel de bits */
	TOK_SHL,
	TOK_SHR,
	TOK_BIT_AND,
	TOK_BIT_OR,
	TOK_BIT_XOR,
	TOK_BIT_NOT,

	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_COMMA,
	TOK_SEMICOLON
} ScannerToken;

const char *scanner_token_name(int token);

extern int scanner_last_token;

#endif