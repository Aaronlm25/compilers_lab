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
    int n = (int)dfa.subsets.size();
    int m = (int)dfa.alphabet.size();

    std::set<int> f, no_f;
    for (int q = 0; q < n; q++)
    {
        (dfa.accept[q] ? f : no_f).insert(q);
    }

    
}
