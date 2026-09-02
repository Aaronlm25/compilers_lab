#include "nfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 32

typedef struct
{
    int start;
    int accept;
} fragment;

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

static void add_state(const nfa *n, int s, int *set, int *set_size, int *visited)
{
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

bool save_nfa(const nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}

bool load_nfa(nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}