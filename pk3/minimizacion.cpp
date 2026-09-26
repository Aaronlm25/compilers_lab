#include "dfa.h"

/*
 * Practica 3: algoritmo de refinamiento de particiones de Hopcroft.
 *
 * Entrada: `dfa`, ya sin estados inalcanzables (nfa_to_dfa solo genera
 * estados alcanzables por construccion de subconjuntos).
 * Salida: el DFA minimizado equivalente, con el menor numero de estados
 * posible.
 *
 */
DFA minimize_dfa(const DFA &dfa)
{
    DFA dfa_min;
    dfa_min.alphabet = dfa.alphabet;

    // reemplazar este cuerpo por el algoritmo de Hopcroft.

    return dfa_min;
}
