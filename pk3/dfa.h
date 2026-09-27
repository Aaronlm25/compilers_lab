#ifndef DFA_H
#define DFA_H

#include <set>
#include <string>
#include <vector>

#include "nfa.h"

/* Estado "muerto": no existe transicion definida. */
#define DFA_DEAD (-1)

/*
 * DFA obtenido por construccion de subconjuntos a partir de un NFA de
 * Thompson.
 *
 * Cada estado del DFA es un subconjunto de estados del NFA (se guarda en
 * `subsets` para poder imprimirlo y depurarlo); `delta` es una tabla densa
 * indexada por [estado][posicion del simbolo en `alphabet`].
 */
struct DFA
{
    std::string alphabet;
    std::vector<std::set<int>> subsets;
    std::vector<std::vector<int>> delta;
    int start;
    std::vector<char> accept;
};

/* Construye el DFA equivalente al NFA de Thompson mediante construccion de subconjuntos. */
DFA nfa_to_dfa(const nfa &n);

/* Imprime la tabla de transiciones del DFA bajo el titulo dado. */
void print_dfa(const DFA &dfa, const char *titulo);

/* Simula `input` sobre el DFA; true si termina en un estado de aceptacion. */
bool test_string(const DFA &dfa, const std::string &input);

/* Corre un lote de pruebas de aceptacion/rechazo y reporta el resultado. */
void run_test_suite(const DFA &dfa, const std::vector<std::string> &accept_tests,
                     const std::vector<std::string> &reject_tests);

/*
 * Practica 3: aplica el algoritmo de refinamiento de particiones de Hopcroft
 * sobre `dfa` (ya sin estados inalcanzables, por construccion) y devuelve el
 * DFA minimizado equivalente.
 *
 * Implementar en minimizacion.cpp.
 */
DFA minimize_dfa(const DFA &dfa);

#endif /* DFA_H */
