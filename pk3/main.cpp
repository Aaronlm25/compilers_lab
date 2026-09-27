#include "dfa.h"
#include "nfa.h"

extern "C"
{
#include "regex.h"
}

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

/*
 * Formato de entrada esperado por stdin:
 *
 *   <expresion regular>
 *   [<n_aceptadas>
 *    <n_aceptadas lineas, "-" para la cadena vacia>
 *    <n_rechazadas>
 *    <n_rechazadas lineas, "-" para la cadena vacia>]
 *
 * El bloque de pruebas es opcional: si no se da, el programa solo
 * construye e imprime las tablas de transicion.
 */

/* Lee un bloque de `cantidad` cadenas de prueba desde stdin. */
static std::vector<std::string> leer_cadenas(int cantidad)
{
    std::vector<std::string> cadenas;
    std::string linea;
    for (int i = 0; i < cantidad && std::getline(std::cin, linea); i++)
    {
        if (linea == "-")
        {
            linea.clear();
        }
        cadenas.push_back(linea);
    }
    return cadenas;
}

int main()
{
    char regex_str[MAX_REGEX_LEN];
    if (!fgets(regex_str, sizeof(regex_str), stdin))
    {
        return 1;
    }
    regex_str[strcspn(regex_str, "\r\n")] = '\0';

    regex r = parse_regex(regex_str);
    nfa n = regex_to_nfa(r);

    DFA original = nfa_to_dfa(n);
    print_dfa(original, "Tabla de Transiciones (DFA Original)");

    DFA minimizado = minimize_dfa(original);
    print_dfa(minimizado, "Tabla de Transiciones (DFA Minimizado)");

    bool cumple = original.subsets.size() >= minimizado.subsets.size();
    std::cout << "\nComprobacion |Q| >= |Q'|: " << original.subsets.size()
               << " >= " << minimizado.subsets.size() << " -> "
               << (cumple ? "OK" : "FALLO") << "\n";
    assert(cumple);

    int num_aceptar = 0;
    if (std::cin >> num_aceptar)
    {
        std::cin.ignore();
        std::vector<std::string> aceptar = leer_cadenas(num_aceptar);

        int num_rechazar = 0;
        std::cin >> num_rechazar;
        std::cin.ignore();
        std::vector<std::string> rechazar = leer_cadenas(num_rechazar);

        run_test_suite(minimizado, aceptar, rechazar);
    }

    free_nfa(&n);
    return 0;
}
