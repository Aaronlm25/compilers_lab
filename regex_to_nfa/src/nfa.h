#ifndef NFA_H
#define NFA_H

#include <stddef.h>
#include <stdbool.h>

#include "regex.h"

/* Valor que indica la ausencia de transicion. */
#define NO_STATE (-1)

/*
 * Marca una transicion epsilon (no consume ningun caracter de la entrada).
 * Se usa el caracter nulo porque no puede aparecer como simbolo valido
 * dentro de una cadena de texto.
 */
#define EPSILON '\0'

/*
 * Un estado del NFA.
 *
 * El algoritmo de Thompson garantiza que cada estado tiene a lo mas dos
 * transiciones salientes, y que se da uno de estos tres casos:
 *
 *   - transicion por un simbolo: symbol != EPSILON, se usa solo out1
 *   - bifurcacion epsilon:       symbol == EPSILON, se usan out1 y out2
 *   - estado de aceptacion:      sin transiciones salientes
 *
 * Esto permite representar el automata como un arreglo de estados
 * enlazados por indice, sin punteros entre nodos.
 */
typedef struct
{
    char symbol; /* simbolo que consume la transicion, o EPSILON */
    int out1;    /* primer destino, o NO_STATE                   */
    int out2;    /* segundo destino (solo epsilon), o NO_STATE   */
} state;

/*
 * El automata finito no determinista completo.
 *
 * El arreglo de estados se reserva dinamicamente y crece conforme el
 * algoritmo de Thompson lo necesita, por lo que debe liberarse con
 * free_nfa al terminar de usarlo.
 */
typedef struct
{
    state *states; /* arreglo dinamico de estados             */
    int count;     /* numero de estados creados               */
    int capacity;  /* capacidad actual del arreglo            */
    int start;     /* estado inicial                          */
    int accept;    /* estado de aceptacion                    */
} nfa;

/*
 * Construye un NFA a partir de una expresion regular en notacion postfija,
 * aplicando el algoritmo de Thompson sobre una pila de fragmentos.
 *
 * La memoria reservada debe liberarse con free_nfa.
 */
nfa regex_to_nfa(regex r);

/*
 * Simula el NFA sobre la cadena dada y devuelve un valor distinto de cero
 * si la cadena pertenece al lenguaje de la expresion regular.
 *
 * La simulacion mantiene el conjunto de estados activos y aplica la
 * cerradura epsilon en cada paso, en lugar de hacer backtracking.
 */
int match_nfa(nfa n, const char *text, size_t len);

/* Libera la memoria del automata y lo deja en un estado seguro. */
void free_nfa(nfa *n);

/*
 * Guarda el automata en un archivo con formato JSON.
 * Devuelve true si la escritura fue exitosa.
 */
bool save_nfa(const nfa *n, const char *path);

/*
 * Carga un automata previamente guardado en formato JSON.
 * Devuelve true si la lectura fue exitosa; en ese caso el automata
 * resultante debe liberarse con free_nfa.
 */
bool load_nfa(nfa *n, const char *path);

#endif /* NFA_H */