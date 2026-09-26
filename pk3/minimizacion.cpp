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

    std::vector<std::set<int>> P;
    if (!f.empty()) P.push_back(f);
    if (!no_f.empty()) P.push_back(no_f);

    std::vector<std::set<int>> W;
    if (!f.empty() && !no_f.empty())
    {
        W.push_back(f);
        W.push_back(no_f);
    }

    while (!W.empty())
    {
        std::set<int> A = W.back();
        W.pop_back();

        for (int c = 0; c < m; c++)
        {
            std::set<int> X;
            for (int q = 0; q < n; q++)
            {
                if (dfa.delta[q][c] != DFA_DEAD && A.count(dfa.delta[q][c]))
                {
                    X.insert(q);
                }
            }
            if (X.empty()) continue;

            std::vector<std::set<int>> siguienteP;
            for (auto &Y : P)
            {
                std::set<int> Y1, Y2;
                for (int q : Y)
                {
                    (X.count(q) ? Y1 : Y2).insert(q);
                }

                if (Y1.empty() || Y2.empty())
                {
                    siguienteP.push_back(Y);
                    continue;
                }

                siguienteP.push_back(Y1);
                siguienteP.push_back(Y2);

                auto it = std::find(W.begin(), W.end(), Y);
                if (it != W.end())
                {
                    *it = Y1;
                    W.push_back(Y2);
                }
                else
                {
                    W.push_back(Y1.size() <= Y2.size() ? Y1 : Y2);
                }
            }
            P = siguienteP;
        }
    }

    std::vector<int> bloque(n);
    for (size_t i = 0; i < P.size(); i++)
    {
        for (int q : P[i])
        {
            bloque[q] = (int)i;
        }
    }

    DFA dfa_min;
    dfa_min.alphabet = dfa.alphabet;
    dfa_min.subsets = P;
    dfa_min.start = bloque[dfa.start];
    dfa_min.delta.assign(P.size(), std::vector<int>(m, DFA_DEAD));
    dfa_min.accept.assign(P.size(), 0);



}
