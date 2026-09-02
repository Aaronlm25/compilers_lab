#ifndef REGEX_H
#define REGEX_H

/*
 * Simbolo usado para representar la concatenacion explicita.
 * Se define aqui para poder cambiarlo en un solo lugar si el validador
 * espera otro caracter (por ejemplo '&' en lugar de '.').
 */
#define CONCAT_CHAR '.'

/* Longitud maxima de la expresion regular de entrada. */
#define MAX_REGEX_LEN 1024

/*
 * Tipo de cada token de la expresion regular.
 * Guardar el tipo evita tener que volver a comparar caracteres durante
 * la construccion del NFA.
 */
typedef enum
{
    TOKEN_SYMBOL,   /* un caracter del alfabeto: a, b, 0, ... */
    TOKEN_CONCAT,   /* concatenacion explicita                */
    TOKEN_UNION,    /* alternancia: |                         */
    TOKEN_STAR,     /* cerradura de Kleene: *                 */
    TOKEN_PLUS,     /* cerradura positiva: +                  */
    TOKEN_QUESTION, /* opcional: ?                            */
    TOKEN_LPAREN,   /* parentesis izquierdo: (                */
    TOKEN_RPAREN    /* parentesis derecho: )                  */
} token_type;

/* Un token individual de la expresion regular. */
typedef struct
{
    char value;      /* el caracter tal cual aparece en la expresion */
    token_type type; /* su clasificacion                             */
} regex_item;

/*
 * Expresion regular ya procesada: arreglo de tokens en notacion postfija
 * con concatenacion explicita.
 */
typedef struct
{
    regex_item items[MAX_REGEX_LEN * 2]; /* el doble por los CONCAT insertados */
    int size;
} regex;

/*
 * Recibe una expresion regular infija con concatenacion implicita y
 * devuelve su equivalente en notacion postfija con concatenacion explicita.
 *
 * Internamente realiza dos pasos:
 *   1. insercion de los operadores de concatenacion explicitos
 *   2. conversion a postfijo mediante el algoritmo de Shunting-Yard
 */
regex parse_regex(const char *regex_str);

#endif /* REGEX_H */