#include "nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Capacidad inicial del arreglo de estados*/
#define INITIAL_CAPACITY 32


/* Representa un sub-grafo de un automata finito NFA 
*
* Lo utilizamos para construir el NFA a partir de la expresion regular obtenida del regex.
*/
typedef struct
{
    int start;
    int accept;
} fragment;


/**
 * Crea un nuevo estado en el NFA y devuelve su indice.
 * Si no hay memoria suficiente, devuelve NO_STATE.
 *  
 * @param n Puntero al NFA donde se agregara el estado.
 * @param symbol Simbolo que consume la transicion, o EPSILON.
 * @param out1 Primer destino del estado, o NO_STATE.
 * @param out2 Segundo destino del estado (solo para transiciones epsilon), o NO_STATE.
 * 
 * @return Indice del nuevo estado en el arreglo de estados del NFA, o NO_STATE si no hay memoria suficiente.
 */
static int new_state(nfa *n, char symbol, int out1, int out2)
{
    if (n->count == n->capacity)
    {
        int nueva = (n->capacity == 0) ? INITIAL_CAPACITY : n->capacity * 2;
        state *tmp = realloc(n->states, nueva * sizeof(state));
        if (tmp == NULL)
        {
            return NO_STATE;
        }
        n->states = tmp;
        n->capacity = nueva;
    }

    n->states[n->count].symbol = symbol;
    n->states[n->count].out1 = out1;
    n->states[n->count].out2 = out2;

    return n->count++;
}


/**
 * Convierte una expresión regular en un Autómata NFA.
 * Esta función toma una expresión regular previamente procesada
 * y construye su NFA equivalente utilizando el Algoritmo de Construcción de Thompson. 
 * Utiliza una pila interna de estructuras para ir enlazando los estados 
 * del autómata de forma modular.
 * 
 * @param r Estructura que contiene el arreglo de tokens de la expresión regular en postfijo.
 * @return nfa El autómata finito construido, amenos que no haya suficiente memoria, en ese caso manda 
 *         un NFA con estados 'NO_STATE'.
 */
nfa regex_to_nfa(regex r)
{
    nfa n;
    n.states = NULL;
    n.count = 0;
    n.capacity = 0;
    n.start = NO_STATE;
    n.accept = NO_STATE;

    fragment pila[MAX_REGEX_LEN * 2];
    int tope = 0;

    for (int i = 0; i < r.size; i++)
    {
        switch (r.items[i].type)
        {
        case TOKEN_SYMBOL:
        {
            int f = new_state(&n, EPSILON, NO_STATE, NO_STATE);
            int s = new_state(&n, r.items[i].value, f, NO_STATE);
            // f es el estado de aceptacion (epsilon, sin salidas) y s el estado que consume el caracter con out1 = f
            if (f == NO_STATE || s == NO_STATE)
            {
                free_nfa(&n);
                return n;
            }
            pila[tope].start = s;
            pila[tope].accept = f;
            tope++;
            break;
        }

        case TOKEN_CONCAT:
        {
            if (tope < 2)
            {
                free_nfa(&n);
                return n;
            }
            fragment b = pila[--tope];
            fragment a = pila[--tope];

            n.states[a.accept].symbol = EPSILON;
            n.states[a.accept].out1 = b.start;

            pila[tope].start = a.start;
            pila[tope].accept = b.accept;
            tope++;
            break;
        }

        case TOKEN_UNION:
        {
            if (tope < 2)
            {
                free_nfa(&n);
                return n;
            }
            fragment b = pila[--tope];
            fragment a = pila[--tope];
            
            // fin une las dos ramas y ini bifurca hacia el inicio de cada fragmento
            int fin = new_state(&n, EPSILON, NO_STATE, NO_STATE);
            int ini = new_state(&n, EPSILON, a.start, b.start);
            if(ini == NO_STATE || fin == NO_STATE)
            {
                free_nfa(&n);
                return n;
            }

            n.states[a.accept].symbol = EPSILON;
            n.states[a.accept].out1 = fin;
            n.states[b.accept].symbol = EPSILON;
            n.states[b.accept].out1 = fin;

            pila[tope].start = ini;
            pila[tope].accept = fin;
            tope++;

            break;
        }

        case TOKEN_STAR:
        {
            if (tope < 1)
            {
                free_nfa(&n);
                return n;
            }
            fragment a = pila[--tope];

            int fin = new_state(&n, EPSILON, NO_STATE, NO_STATE);
            int ini = new_state(&n, EPSILON, a.start, fin);
            if (fin == NO_STATE || ini == NO_STATE)
            {
                free_nfa(&n);
                return n;
            }
            // ini permite entrar al fragmento o saltarlo directo a fin (cero repeticiones) la aceptacion de a vuelve a a.start (repetir) o avanza a fin (salir)
            n.states[a.accept].symbol = EPSILON;
            n.states[a.accept].out1 = a.start;
            n.states[a.accept].out2 = fin;

            pila[tope].start = ini;
            pila[tope].accept = fin;
            tope++;
            break;
        }

        case TOKEN_PLUS:
        {
            if (tope < 1)
            {
                free_nfa(&n);
                return n;
            }
            fragment a = pila[--tope];

            int fin = new_state(&n, EPSILON, NO_STATE, NO_STATE);
            int ini = new_state(&n, EPSILON, a.start, NO_STATE);
            if (fin == NO_STATE || ini == NO_STATE)
            {
                free_nfa(&n);
                return n;
            }

            n.states[a.accept].symbol = EPSILON;
            n.states[a.accept].out1 = a.start;
            n.states[a.accept].out2 = fin;

            pila[tope].start = ini;
            pila[tope].accept = fin;
            tope++;
            break;
        }

        case TOKEN_QUESTION:
        {
            if (tope < 1)
            {
                free_nfa(&n);
                return n;
            }
            fragment a = pila[--tope];

            int fin = new_state(&n, EPSILON, NO_STATE, NO_STATE);
            int ini = new_state(&n, EPSILON, a.start, fin);
            if (fin == NO_STATE || ini == NO_STATE)
            {
                free_nfa(&n);
                return n;
            }

            n.states[a.accept].symbol = EPSILON;
            n.states[a.accept].out1 = fin;

            pila[tope].start = ini;
            pila[tope].accept = fin;
            tope++;
            break;
        }
 
        case TOKEN_LPAREN:
        case TOKEN_RPAREN:
        default:
            free_nfa(&n);
            return n;
        }
    }

    if (tope != 1)
    {
        free_nfa(&n);
        return n;
    }

    n.start = pila[0].start;
    n.accept = pila[0].accept;

    return n;
}

/**
 * Agrega un estado al conjunto de estados alcanzables desde el estado `s` en el NFA `n`.
 * Si el estado `s` es un estado de transición epsilon, 
 * se agregan recursivamente los estados alcanzables desde sus destinos.
 * 
 * @param n Puntero al NFA.
 * @param s Indice del estado a agregar al conjunto.
 * @param set Arreglo donde se almacenan los estados alcanzables.
 * @param set_size Puntero al tamaño actual del conjunto de estados.
 * @param visited Arreglo de estados visitados para evitar ciclos.
 * 
 * @return retorna el conjunto de estados alcanzables, se actualiza en `set` y `set_size`.
 */
static void add_state(const nfa *n, int s, int *set, int *set_size, int *visited)
{
    // calcula la cerradura epsilon de s: lo agrega al conjunto y, si es un estado epsilon, se expande recursivamente por out1 y out2
    if (s == NO_STATE || visited[s])
    {
        return;
    }

    visited[s] = 1;

    set[*set_size] = s;
    (*set_size)++;

    if (n->states[s].symbol == EPSILON)
    {
        add_state(n, n->states[s].out1, set, set_size, visited);
        add_state(n, n->states[s].out2, set, set_size, visited);
    }
}


/**
 * Verifica si el texto dado es aceptado por el NFA.
 * 
 * @param n Puntero al NFA.
 * @param text Texto a verificar.
 * @param len Longitud del texto.
 * 
 * @return 1 si el texto es aceptado, 0 en caso contrario.
 */
int match_nfa(nfa n, const char *text, size_t len)
{
    if (n.start == NO_STATE || n.count <= 0)
    {
        return 0;
    }

    int *actual = malloc(n.count * sizeof(int));
    int *siguiente = malloc(n.count * sizeof(int));
    int *visited = malloc(n.count * sizeof(int));

    if (actual == NULL || siguiente == NULL || visited == NULL)
    {
        free(actual);
        free(siguiente);
        free(visited);
        return 0;
    }

    int actual_size = 0;
    memset(visited, 0, n.count * sizeof(int));
    add_state(&n, n.start, actual, &actual_size, visited);

        for (size_t i = 0; i < len; i++)
    {
        int siguiente_size = 0;
        memset(visited, 0, n.count * sizeof(int));

        for (int j = 0; j < actual_size; j++)
        {
            int s = actual[j];
            if (n.states[s].symbol == text[i])
            {
                add_state(&n, n.states[s].out1, siguiente, &siguiente_size, visited);
            }
        }

        int *tmp = actual;
        actual = siguiente;
        siguiente = tmp;
        actual_size = siguiente_size;
    }

        int aceptada = 0;
    for (int j = 0; j < actual_size; j++)
    {
        if (actual[j] == n.accept)
        {
            aceptada = 1;
            break;
        }
    }

    free(actual);
    free(siguiente);
    free(visited);

    return aceptada;
}

/**
 * Libera la memoria asignada para el NFA.
 * @param n Puntero al NFA.
 * @return libera la memoria del NFA y reinicia sus campos.
 */
void free_nfa(nfa *n)
{
    if (n == NULL)
    {
        return;
    }

    free(n->states);

    n->states = NULL;
    n->count = 0;
    n->capacity = 0;
    n->start = NO_STATE;
    n->accept = NO_STATE;
}

/**
 * Guarda el NFA en un archivo binario.
 * @param n Puntero al NFA.
 * @param path Ruta del archivo donde se guardará el NFA.
 * @return true si se guardó correctamente, false en caso contrario.
 */
bool save_nfa(const nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}

/**
 * Carga un NFA desde un archivo binario.
 * @param n Puntero al NFA donde se cargará la información.
 * @param path Ruta del archivo desde donde se cargará el NFA.
 * @return true si se cargó correctamente, false en caso contrario.
 */
bool load_nfa(nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}
