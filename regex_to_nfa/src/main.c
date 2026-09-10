#include "regex.h"
#include "nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* imprime los tokens de la expresion ya convertida a postfijo */
void print_postfix(regex r)
{
    for (int i = 0; i < r.size; i++)
    {
        printf("%c", r.items[i].value);
    }
    printf("\n");
}

/* valida cada linea que llega por la entrada estandar contra la expresion */
void test_strings_stdin(const char *regex_str)
{
    /* el automata se construye una sola vez y se reusa para todas las cadenas */
    regex r = parse_regex(regex_str);
    nfa n = regex_to_nfa(r);

    char buf[1024];
    while (fgets(buf, sizeof(buf), stdin))
    {
        /* quita el salto de linea que deja fgets */
        buf[strcspn(buf, "\r\n")] = '\0';

        int result = match_nfa(n, buf, strlen(buf));
        printf("%d", result ? 1 : 0);
    }
    printf("\n");

    free_nfa(&n);
}

/* lee la expresion regular de la primera linea y despacha el modo pedido */
int main(int argc, char *argv[])
{
    int opt;
    char regex_str[1024];
    int mode = 0;

    while ((opt = getopt(argc, argv, "rt")) != -1)
    {
        if (opt != 'r' && opt != 't')
        {
            fprintf(stderr, "Usage: %s -r | -t\n", argv[0]);
            return 1;
        }

        /* solo se admite un modo a la vez */
        if (mode != 0)
        {
            fprintf(stderr, "Error: Solo puedes usar una opcion de modo entre -r o -t.\n");
            return 1;
        }

        if (opt == 'r')
        {
            mode = 'r';
        }
        else
        {
            mode = 't';
        }
    }

    if (mode == 0)
    {
        fprintf(stderr, "Usage: %s -r | -t\n", argv[0]);
        return 1;
    }

    /* la expresion regular siempre viene en la primera linea */
    if (!fgets(regex_str, sizeof(regex_str), stdin))
    {
        return 1;
    }
    regex_str[strcspn(regex_str, "\r\n")] = '\0';

    /* modo r solo convierte a postfijo */
    if (mode == 'r')
    {
        print_postfix(parse_regex(regex_str));
        return 0;
    }

    /* modo t ademas construye el automata y valida las cadenas */
    test_strings_stdin(regex_str);

    return 0;
}