#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
using ll = long long; 
constexpr int MAXQ = 256;                    // tope de estados del AFN
using Conjunto = bitset<MAXQ>;
constexpr int MUERTO = -1;
 
/* ---- AFN ---- */
 
struct NFA {
    int n = 0, q0 = 0;
    string sigma;                            // alfabeto, sin epsilon
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
 
    Conjunto C = T;                          // la clausura contiene a T
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
 
/* ----- Algoritmo 3: Subconjuntos ----- */
 
DFA subconjuntos(const NFA& N) {
    DFA D;
    D.sigma = N.sigma;
    const int m = (int)N.sigma.size();
 
    unordered_map<Conjunto, int> id;         
    queue<int> cola;
 
    auto registrar = [&](const Conjunto& S) {            // id del subconjunto, lo encola si es nuevo
        auto it = id.find(S);
        if (it != id.end()) return it->second;
        int nuevo = (int)D.subconj.size();
        id.emplace(S, nuevo);
        D.subconj.push_back(S);
        D.delta.emplace_back(m, MUERTO);
        cola.push(nuevo);
        return nuevo;
    };
 
    Conjunto inicio;
    inicio.set(N.q0);
    D.s0 = registrar(eClosure(N, inicio));               // s0 <- e-Closure({q0})
 
    while (!cola.empty()) {
        int U = cola.front();
        cola.pop();
        for (int a = 0; a < m; ++a) {
            Conjunto V = eClosure(N, Move(N, D.subconj[U], a));
            if (V.none()) continue;                      // sin estado muerto explícito
            int destino = registrar(V);                  // registrar hace push_back sobre D.delta,
            D.delta[U][a] = destino;                     
        }
    }
 
    D.esFinal.assign(D.subconj.size(), 0);
    for (size_t S = 0; S < D.subconj.size(); ++S)
        D.esFinal[S] = (D.subconj[S] & N.F).any();       // S ∩ F_N != vacío
 
    return D;
}
 
/* ----- salida ----- */
 
bool acepta(const DFA& D, const string& w) {
    int cur = D.s0;
    for (char c : w) {
        size_t a = D.sigma.find(c);
        if (a == string::npos) return false;             // símbolo fuera del alfabeto
        cur = D.delta[cur][a];
        if (cur == MUERTO) return false;
    }
    return D.esFinal[cur];
}
 
void imprimir(const DFA& D) {
    cout << "alfabeto: " << D.sigma << "\n"
         << "estados: " << D.subconj.size() << "\n"
         << "inicial: " << D.s0 << "\n";
 
    for (size_t i = 0; i < D.subconj.size(); ++i) {
        cout << i << (D.esFinal[i] ? "* {" : " {");
        const Conjunto& S = D.subconj[i];
        bool primero = true;
        for (size_t b = S._Find_first(); b < S.size(); b = S._Find_next(b)) {
            if (!primero) cout << ",";
            cout << b;
            primero = false;
        }
        cout << "}";
        for (size_t a = 0; a < D.sigma.size(); ++a) {
            cout << " " << D.sigma[a] << "->";
            if (D.delta[i][a] == MUERTO) cout << "-";
            else                         cout << D.delta[i][a];
        }
        cout << "\n";
    }
}
 
 
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
 
    int n, m, q0, k;
    if (!(cin >> n >> m)) return 0;
    string sigma;
    cin >> sigma >> q0 >> k;
    if (n <= 0 || n > MAXQ) {
        cerr << "n fuera de rango: debe estar entre 1 y " << MAXQ << "\n";
        return 1;
    }
    if (q0 < 0 || q0 >= n) {
        cerr << "estado inicial invalido: " << q0 << "\n";
        return 1;
    }
 
    NFA N;
    N.init(n, sigma);
    N.q0 = q0;
    for (int i = 0, f; i < k; ++i) { cin >> f; N.F.set(f); }
    for (int i = 0, s, t; i < m; ++i) {
        char c;
        cin >> s >> c >> t;
        N.add(s, c, t);
    }
 
    DFA D = subconjuntos(N);
    imprimir(D);
 
    int p;
    if (cin >> p)
        for (int i = 0; i < p; ++i) {
            string w;
            cin >> w;
            if (w == "-") w.clear();                     // cadena vacía
            cout << "\"" << w << "\": " << (acepta(D, w) ? "aceptada" : "rechazada") << "\n";
        }
    return 0;
}
