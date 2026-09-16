#include <bits/stdc++.h>
using namespace std;
 
using ll = long long;
#define all(x) begin(x), end(x)
 
constexpr int MAXQ = 256;            
using Conjunto = bitset<MAXQ>;
constexpr int MUERTO = -1;

/* ----- NFA ----- */

struct NFA {
    int n = 0, q0 = 0;
    string sigma;                            // alfabeto sin epsilon
    array<int, 128> idx{};                   // símbolo -> posición en sigma
    vector<vector<vector<int>>> delta;       // delta[estado][símbolo] -> destinos
    vector<vector<int>> eps;                 // delta[estado][epsilon]
    Conjunto F;
 
    void init(int n_, const string& sig) {
        n = n_;
        sigma = sig;
        idx.fill(-1);
        for (int i = 0; i < (int)sigma.size(); ++i) idx[(unsigned char)sigma[i]] = i;
        delta.assign(n, vector<vector<int>>(sigma.size()));
        eps.assign(n, {});
    }
    void add(int s, char c, int t) {         // c == '.' => transición epsilon
        if (c == '.') eps[s].push_back(t);
        else          delta[s][idx[(unsigned char)c]].push_back(t);
    }
    void compactar() {                       // duplicados fuera, listas ordenadas
        auto limpiar = [](vector<int>& v) {
            sort(all(v));
            v.erase(unique(all(v)), v.end());
        };
        for (auto& fila : delta) for (auto& v : fila) limpiar(v);
        for (auto& v : eps) limpiar(v);
    }
};
 
/* ---- Algoritmo 1: Move ---- */
 
Conjunto Move(const NFA& N, const Conjunto& T, int a) {
    Conjunto R;                                              // R <- vacío
    for (size_t s = T._Find_first(); s < T.size(); s = T._Find_next(s))
        for (int u : N.delta[s][a]) R.set(u);                // R <- R U delta(s,a)
    return R;
}

/* ---- Algoritmo 2: epsilon-Closure ---- */
 
Conjunto eClosure(const NFA& N, const Conjunto& T) {
    vector<int> pila;
    for (size_t s = T._Find_first(); s < T.size(); s = T._Find_next(s))
        pila.push_back((int)s);
 
    Conjunto C = T;                          
    while (!pila.empty()) {
        int t = pila.back();
        pila.pop_back();
        for (int u : N.eps[t])
            if (!C.test(u)) {                // el bitset hace de "visitado"
                C.set(u);
                pila.push_back(u);
            }
    }
    return C;
}
 
/* ----- AFD ----- */
 
struct DFA {
    string sigma;
    vector<Conjunto> subconj;                // subconj[i] = subconjunto de Q_N
    vector<vector<int>> delta;               // delta[i][a], MUERTO si no existe
    int s0 = 0;
    vector<char> esFinal;
};
