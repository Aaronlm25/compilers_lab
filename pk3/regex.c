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
    case TOKEN_PLUS:
    case TOKEN_QUESTION:
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
    case '+':
        return TOKEN_PLUS;
    case '?':
        return TOKEN_QUESTION;
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
 */
static regex add_explicit_concat(const char *regex_str)
{
    regex out;
    out.size = 0;

    if (regex_str == NULL)
    {
        return out;
    }

    for (int i = 0; regex_str[i] != '\0' && out.size < MAX_REGEX_LEN * 2; i++)
    {
        token_type type = classify(regex_str[i]);

        if (out.size > 0)
        {
            token_type previous = out.items[out.size - 1].type;

            int previous_closes = previous == TOKEN_SYMBOL ||
                                  previous == TOKEN_RPAREN ||
                                  previous == TOKEN_STAR ||
                                  previous == TOKEN_PLUS ||
                                  previous == TOKEN_QUESTION;

            int current_opens = type == TOKEN_SYMBOL || type == TOKEN_LPAREN;

            if (previous_closes && current_opens && out.size + 1 < MAX_REGEX_LEN * 2)
            {
                out.items[out.size].value = CONCAT_CHAR;
                out.items[out.size].type = TOKEN_CONCAT;
                out.size++;
            }
        }

        out.items[out.size].value = regex_str[i];
        out.items[out.size].type = type;
        out.size++;
    }

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
 */
static regex shunting_yard(regex infix)
{
    regex out;
    regex_item stack[MAX_REGEX_LEN * 2];
    int top = 0;

    out.size = 0;

    for (int i = 0; i < infix.size; i++)
    {
        regex_item item = infix.items[i];

        if (item.type == TOKEN_SYMBOL)
        {
            out.items[out.size] = item;
            out.size++;
        }
        else if (item.type == TOKEN_LPAREN)
        {
            stack[top] = item;
            top++;
        }
        else if (item.type == TOKEN_RPAREN)
        {
            while (top > 0 && stack[top - 1].type != TOKEN_LPAREN)
            {
                top--;
                out.items[out.size] = stack[top];
                out.size++;
            }

            if (top > 0)
            {
                top--;
            }
        }
        else
        {
            while (top > 0 &&
                   stack[top - 1].type != TOKEN_LPAREN &&
                   precedence(stack[top - 1].type) >= precedence(item.type))
            {
                top--;
                out.items[out.size] = stack[top];
                out.size++;
            }

            stack[top] = item;
            top++;
        }
    }

    while (top > 0)
    {
        top--;

        if (stack[top].type != TOKEN_LPAREN)
        {
            out.items[out.size] = stack[top];
            out.size++;
        }
    }

    return out;
}

regex parse_regex(const char *regex_str)
{
    regex explicit_concat = add_explicit_concat(regex_str);
    return shunting_yard(explicit_concat);
}