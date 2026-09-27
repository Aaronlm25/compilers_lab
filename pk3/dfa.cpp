#include "dfa.h"

#include <iostream>
#include <map>
#include <queue>

/* Cerradura epsilon de un conjunto de estados del NFA de Thompson. */
static std::set<int> cerradura_epsilon(const nfa &n, const std::set<int> &t)
{
    std::vector<int> pila(t.begin(), t.end());
    std::set<int> c = t;

    while (!pila.empty())
    {
        int s = pila.back();
        pila.pop_back();

        if (n.states[s].symbol == EPSILON)
        {
            int destinos[2] = {n.states[s].out1, n.states[s].out2};
            for (int u : destinos)
            {
                if (u != NO_STATE && c.insert(u).second)
                {
                    pila.push_back(u);
                }
            }
        }
    }

    return c;
}

/* Estados alcanzables desde `t` consumiendo el simbolo `c`. */
static std::set<int> mover(const nfa &n, const std::set<int> &t, char c)
{
    std::set<int> r;
    for (int s : t)
    {
        if (n.states[s].symbol == c)
        {
            r.insert(n.states[s].out1);
        }
    }
    return r;
}

DFA nfa_to_dfa(const nfa &n)
{
    DFA d;
    d.start = 0;

    std::set<char> alfabeto;
    for (int i = 0; i < n.count; i++)
    {
        if (n.states[i].symbol != EPSILON)
        {
            alfabeto.insert(n.states[i].symbol);
        }
    }
    d.alphabet.assign(alfabeto.begin(), alfabeto.end());
    int m = (int)d.alphabet.size();

    std::map<std::set<int>, int> id;
    std::queue<int> cola;

    auto registrar = [&](const std::set<int> &s)
    {
        auto it = id.find(s);
        if (it != id.end())
        {
            return it->second;
        }
        int nuevo = (int)d.subsets.size();
        id.emplace(s, nuevo);
        d.subsets.push_back(s);
        d.delta.emplace_back(m, DFA_DEAD);
        cola.push(nuevo);
        return nuevo;
    };

    d.start = registrar(cerradura_epsilon(n, {n.start}));

    while (!cola.empty())
    {
        int u = cola.front();
        cola.pop();

        for (int a = 0; a < m; a++)
        {
            std::set<int> v = cerradura_epsilon(n, mover(n, d.subsets[u], d.alphabet[a]));
            if (v.empty())
            {
                continue;
            }
            d.delta[u][a] = registrar(v);
        }
    }

    d.accept.assign(d.subsets.size(), 0);
    for (size_t i = 0; i < d.subsets.size(); i++)
    {
        d.accept[i] = d.subsets[i].count(n.accept) ? 1 : 0;
    }

    return d;
}

void print_dfa(const DFA &dfa, const char *titulo)
{
    std::cout << "--- " << titulo << " ---\n";
    std::cout << "Estado Inicial: " << dfa.start << "\n";
    std::cout << "Estados de Aceptacion: ";
    for (size_t i = 0; i < dfa.accept.size(); i++)
    {
        if (dfa.accept[i])
        {
            std::cout << i << " ";
        }
    }
    std::cout << "\nTransiciones:\n";

    for (size_t i = 0; i < dfa.delta.size(); i++)
    {
        for (size_t a = 0; a < dfa.alphabet.size(); a++)
        {
            if (dfa.delta[i][a] != DFA_DEAD)
            {
                std::cout << "  d(" << i << ", '" << dfa.alphabet[a] << "') -> "
                           << dfa.delta[i][a] << "\n";
            }
        }
    }
}

bool test_string(const DFA &dfa, const std::string &input)
{
    if (dfa.subsets.empty())
    {
        return false;
    }

    int actual = dfa.start;
    for (char c : input)
    {
        size_t a = dfa.alphabet.find(c);
        if (a == std::string::npos)
        {
            return false;
        }
        actual = dfa.delta[actual][a];
        if (actual == DFA_DEAD)
        {
            return false;
        }
    }
    return dfa.accept[actual];
}

void run_test_suite(const DFA &dfa, const std::vector<std::string> &accept_tests,
                     const std::vector<std::string> &reject_tests)
{
    int passed = 0;
    int total = (int)(accept_tests.size() + reject_tests.size());

    std::cout << "\n[Corriendo Casos de Aceptacion]\n";
    for (const auto &s : accept_tests)
    {
        bool res = test_string(dfa, s);
        std::cout << "Cadena \"" << s << "\": " << (res ? "PASS" : "FAIL") << "\n";
        passed += res;
    }

    std::cout << "\n[Corriendo Casos de Rechazo]\n";
    for (const auto &s : reject_tests)
    {
        bool res = !test_string(dfa, s);
        std::cout << "Cadena \"" << s << "\": " << (res ? "PASS" : "FAIL") << "\n";
        passed += res;
    }

    std::cout << "\nResultado: " << passed << "/" << total << " pruebas superadas.\n";
}
