#include<bits/stdc++.h>
using namespace std;
using ll=long long;
const int MAXQ=256;
const int MUERTO=-1;
using Conjunto=bitset<MAXQ>;

struct NFA{
	int n=0;
	int q0=0;
	string sigma;
	array<int,128> idx{};
	vector<vector<vector<int>>> delta;
	vector<vector<int>> eps;
	Conjunto F;

	void init(int nn,const string& sig){
		n=nn;
		sigma=sig;
		idx.fill(-1);
		for(size_t i=0;i<sigma.size();i++){
			idx[(unsigned char)sigma[i]]=i;
		}
		delta.assign(n,vector<vector<int>>(sigma.size()));
		eps.assign(n,{});
	}

	void add(int s,char c,int t){
		if(c=='.'){
			eps[s].push_back(t);
		}else{
			delta[s][idx[(unsigned char)c]].push_back(t);
		}
	}
};

// Algoritmo 1: Move
Conjunto Move(const NFA& N,const Conjunto& T,int a){
	Conjunto R;
	for(size_t s=T._Find_first();s<T.size();s=T._Find_next(s)){
		for(int u:N.delta[s][a]){
			R.set(u);
		}
	}
	return R;
}

// Algoritmo 2: epsilon-Closure
Conjunto eClosure(const NFA& N,const Conjunto& T){
	vector<int> pila;
	for(size_t s=T._Find_first();s<T.size();s=T._Find_next(s)){
		pila.push_back((int)s);
	}
	Conjunto C=T;
	while(!pila.empty()){
		int t=pila.back();
		pila.pop_back();
		for(int u:N.eps[t]){
			if(!C.test(u)){
				C.set(u);
				pila.push_back(u);
			}
		}
	}
	return C;
}

struct DFA{
	string sigma;
	vector<Conjunto> subconj;
	vector<vector<int>> delta;
	int s0=0;
	vector<char> esFinal;
};

// Algoritmo 3: Subconjuntos
DFA subconjuntos(const NFA& N){
	DFA D;
	D.sigma=N.sigma;
	int m=(int)N.sigma.size();
	unordered_map<Conjunto,int> id;
	queue<int> cola;

	auto registrar=[&](const Conjunto& S){
		auto it=id.find(S);
		if(it!=id.end()){
			return it->second;
		}
		int nuevo=(int)D.subconj.size();
		id.emplace(S,nuevo);
		D.subconj.push_back(S);
		D.delta.emplace_back(m,MUERTO);
		cola.push(nuevo);
		return nuevo;
	};

	Conjunto inicio;
	inicio.set(N.q0);
	D.s0=registrar(eClosure(N,inicio));

	while(!cola.empty()){
		int U=cola.front();
		cola.pop();
		for(int a=0;a<m;a++){
			Conjunto V=eClosure(N,Move(N,D.subconj[U],a));
			if(V.none()){
				continue;
			}
			int destino=registrar(V);
			D.delta[U][a]=destino;
		}
	}

	D.esFinal.assign(D.subconj.size(),0);
	for(size_t S=0;S<D.subconj.size();S++){
		D.esFinal[S]=(D.subconj[S]&N.F).any();
	}
	return D;
}

bool acepta(const DFA& D,const string& w){
	int act=D.s0;
	for(char c:w){
		size_t a=D.sigma.find(c);
		if(a==string::npos){
			return false;
		}
		act=D.delta[act][a];
		if(act==MUERTO){
			return false;
		}
	}
	return D.esFinal[act];
}

void imprimir(const DFA& D){
	cout<<"alfabeto: "<<D.sigma<<"\n";
	cout<<"estados: "<<D.subconj.size()<<"\n";
	cout<<"inicial: "<<D.s0<<"\n";
	for(size_t i=0;i<D.subconj.size();i++){
		cout<<i<<(D.esFinal[i]?"* {":" {");
		const Conjunto& S=D.subconj[i];
		bool primero=true;
		for(size_t b=S._Find_first();b<S.size();b=S._Find_next(b)){
			if(!primero){
				cout<<",";
			}
			cout<<b;
			primero=false;
		}
		cout<<"}";
		for(size_t a=0;a<D.sigma.size();a++){
			cout<<" "<<D.sigma[a]<<"->";
			if(D.delta[i][a]==MUERTO){
				cout<<"-";
			}else{
				cout<<D.delta[i][a];
			}
		}
		cout<<"\n";
	}
}

int main(){
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int n,m,q0,k;
	if(!(cin>>n>>m)){
		return 0;
	}
	string sigma;
	cin>>sigma>>q0>>k;
	if(n<=0||n>MAXQ){
		cerr<<"n fuera de rango: debe estar entre 1 y "<<MAXQ<<"\n";
		return 1;
	}
	if(q0<0||q0>=n){
		cerr<<"estado inicial invalido: "<<q0<<"\n";
		return 1;
	}

	NFA N;
	N.init(n,sigma);
	N.q0=q0;
	for(int i=0;i<k;i++){
		int f;
		cin>>f;
		N.F.set(f);
	}
	for(int i=0;i<m;i++){
		int s,t;
		char c;
		cin>>s>>c>>t;
		N.add(s,c,t);
	}

	DFA D=subconjuntos(N);
	imprimir(D);

	int p;
	if(cin>>p){
		for(int i=0;i<p;i++){
			string w;
			cin>>w;
			if(w=="-"){
				w.clear();
			}
			cout<<"\""<<w<<"\": "<<(acepta(D,w)?"aceptada":"rechazada")<<"\n";
		}
	}
	return 0;
}
