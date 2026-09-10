#include "regex.h"
#include "nfa.h"
#include "grafico_nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// Imprime el arreglo de tokens ya en notacion postfija, un caracter por token, sin separadores, usado por el modo -r
void print_postfix(regex r)
{
    for (int i = 0; i < r.size; i++)
    {
        printf("%c", r.items[i].value);
    }
    printf("\n");
}

// Construye el NFA una sola vez y lo reutiliza para evaluar cada linea restante de stdin; usado por el modo -t
void test_strings_stdin(const char *regex_str)
{
    regex r = parse_regex(regex_str);
    nfa n = regex_to_nfa(r);

    char buf[1024];
    while (fgets(buf, sizeof(buf), stdin))
    {
        buf[strcspn(buf, "\r\n")] = '\0';
        int result = match_nfa(n, buf, strlen(buf));
        printf("%d", result ? 1 : 0);
    }
    printf("\n");

    free_nfa(&n);
}

int serialize_nfa_from_regex(const char *regex_str, const char *output_path)
{
    regex r = parse_regex(regex_str);
    nfa n = regex_to_nfa(r);

    bool ok = save_nfa(&n, output_path);
    free_nfa(&n);

    if (!ok)
    {
        fprintf(stderr, "Error: No se pudo serializar el NFA en '%s'.\n", output_path);
        return 1;
    }

    return 0;
}

// Construye el NFA, calcula su distribucion grafica con iniciaGrafico y escribe el SVG resultante con escribeNfa usado por -g
int draw_nfa_from_regex(const char *regex_str, const char *ou
int draw_nfa_from_regex(const char *regex_str, const char *output_path)
{
    regex r = parse_regex(regex_str);
    nfa n = regex_to_nfa(r);
    grafico g;

    if (iniciaGrafico(&g, n) != 0)
    {
        fprintf(stderr, "Error: No se pudo calcular la distribucion del automata.\n");
        free_nfa(&n);
        return 1;
    }

    int fallo = escribeNfa(&g, output_path, regex_str);

    liberaGrafico(&g);
    free_nfa(&n);

    if (fallo)
    {
        fprintf(stderr, "Error: No se pudo escribir el SVG en '%s'.\n", output_path);
        return 1;
    }

    return 0;
}

// Parsea las opciones de linea de comandos para elegir un modo de operacion, luego lee la expresion regular desde la primera linea de stdin y ejecuta el modo correspondiente
int main(int argc, char *argv[])
{
    int opt;
    char regex_str[1024];
    char *output_file = NULL;
    int mode = 0;

    while ((opt = getopt(argc, argv, "rto:g:")) != -1)
    {
        if (opt != 'r' && opt != 't' && opt != 'o' && opt != 'g')
        {
            fprintf(stderr, "Usage: %s -r | -t | -o <archivo.nfa> | -g <archivo.svg>\n", argv[0]);
            return 1;
        }

        if (mode != 0)
        {
            fprintf(stderr, "Error: Solo puedes usar una opcion de modo entre -r, -t, -o o -g.\n");
            return 1;
        }

        if (opt == 'r')
        {
            mode = 'r';
        }
        else if (opt == 't')
        {
            mode = 't';
        }
        else if (opt == 'o')
        {
            mode = 'o';
            output_file = optarg;
        }
        else
        {
            mode = 'g';
            output_file = optarg;
        }
    }

    if (mode == 0)
    {
        fprintf(stderr, "Usage: %s -r | -t | -o <archivo.nfa> | -g <archivo.svg>\n", argv[0]);
        return 1;
    }

    if (!fgets(regex_str, sizeof(regex_str), stdin))
    {
        return 1;
    }
    regex_str[strcspn(regex_str, "\r\n")] = '\0';

    if (mode == 'r')
    {
        print_postfix(parse_regex(regex_str));
        return 0;
    }

    if (mode == 't')
    {
        test_strings_stdin(regex_str);
        return 0;
    }

    if (mode == 'g')
    {
        return draw_nfa_from_regex(regex_str, output_file);
    }

    return serialize_nfa_from_regex(regex_str, output_file);
}
