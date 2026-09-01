#include "regex.h"

#include <string.h>

/*
 * Devuelve la precedencia de un operador.
 * Mayor numero indica mayor precedencia.
 *
 *   *  ->  3   (cerradura de Kleene)
 *   .  ->  2   (concatenacion)
 *   |  ->  1   (union)
 */
static int precedence(token_type type)
{
    switch (type)
    {
    case TOKEN_STAR:
        return 3;
    case TOKEN_CONCAT:
        return 2;
    case TOKEN_UNION:
        return 1;
    default:
        return 0;
    }
}

/* Clasifica un caracter de la expresion regular. */
static token_type classify(char c)
{
    switch (c)
    {
    case '|':
        return TOKEN_UNION;
    case '*':
        return TOKEN_STAR;
    case '(':
        return TOKEN_LPAREN;
    case ')':
        return TOKEN_RPAREN;
    default:
        return TOKEN_SYMBOL;
    }
}

/*
 * Paso 1: inserta los operadores de concatenacion explicitos.
 *
 * Se inserta un CONCAT entre dos tokens consecutivos cuando el primero
 * puede cerrar una subexpresion (simbolo, ')' o '*') y el segundo puede
 * abrir una nueva (simbolo o '(').
 *
 * TODO: implementar.
 */
static regex add_explicit_concat(const char *regex_str)
{
    regex out;
    out.size = 0;

    (void)regex_str;
    (void)classify;

    return out;
}

/*
 * Paso 2: algoritmo de Shunting-Yard.
 *
 * Recorre los tokens en orden infijo y produce la salida en postfijo,
 * apoyandose en una pila de operadores:
 *
 *   - un simbolo pasa directo a la salida
 *   - un '(' se apila
 *   - un ')' desapila hasta encontrar el '(' correspondiente
 *   - un operador desapila mientras la cima tenga precedencia mayor o igual
 *   - al final se vacia la pila hacia la salida
 *
 * TODO: implementar.
 */
static regex shunting_yard(regex infix)
{
    regex out;
    out.size = 0;

    (void)infix;
    (void)precedence;

    return out;
}

regex parse_regex(const char *regex_str)
{
    regex explicit_concat = add_explicit_concat(regex_str);
    return shunting_yard(explicit_concat);
}