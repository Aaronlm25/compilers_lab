#include<bits/stdc++.h>
using namespace std;
const int MUERTO=-1;

struct NFA{
	int n=0;
	int q0=0;
	int qf=0;
	string sigma;
	vector<vector<pair<char,int>>> delta;

	int nuevo(){
		delta.emplace_back();
		return n++;
	}

	void add(int s,char c,int t){
		delta[s].push_back({c,t});
	}
};

struct DFA{
	int n=0;
	int s0=0;
	string sigma;
	vector<vector<int>> delta;
	vector<char> esFinal;

	int idx(char c) {
		int ans=-1;
		for(int i=0;i<sigma.size();i++){
			if(sigma[i]==c){
				ans=i;
				break;
			}
		}
		return ans;
	}
};


// Plantilla de validación

void print_dfa(const DFA& dfa){
	cout<<"--- Tabla de Transiciones (DFA Original) ---\n";
	cout<<"Alfabeto: "<<dfa.sigma<<"\n";
	cout<<"Numero de estados: "<<dfa.n<<"\n";
	cout<<"Estado Inicial: "<<dfa.s0<<"\n";
	cout<<"Estados de Aceptacion:";
	for(int i=0;i<dfa.n;i++){
		if(dfa.esFinal[i]){
			cout<<" "<<i;
		}
	}
	cout<<"\nTransiciones:\n";
	for(int i=0;i<dfa.n;i++){
		for(size_t a=0;a<dfa.sigma.size();a++){
			cout<<"  d("<<i<<",'"<<dfa.sigma[a]<<"') -> ";
			if(dfa.delta[i][a]==MUERTO){
				cout<<"-\n";
			}else{
				cout<<dfa.delta[i][a]<<"\n";
			}
		}
	}
}

// Tabla de transiciones del DFA minimizado
void print_dfa_min(const DFA& dfa_min){
	cout<<"--- Tabla de Transiciones (DFA Minimizado) ---\n";
	cout<<"Alfabeto: "<<dfa_min.sigma<<"\n";
	cout<<"Numero de estados: "<<dfa_min.n<<"\n";
	cout<<"Estado Inicial: "<<dfa_min.s0<<"\n";
	cout<<"Estados de Aceptacion:";
	for(int i=0;i<dfa_min.n;i++){
		if(dfa_min.esFinal[i]){
			cout<<" "<<i;
		}
	}
	cout<<"\nTransiciones:\n";
	for(int i=0;i<dfa_min.n;i++){
		for(size_t a=0;a<dfa_min.sigma.size();a++){
			cout<<"  d("<<i<<",'"<<dfa_min.sigma[a]<<"') -> ";
			if(dfa_min.delta[i][a]==MUERTO){
				cout<<"-\n";
			}else{
				cout<<dfa_min.delta[i][a]<<"\n";
			}
		}
	}
}


bool test_string(DFA& dfa, string& input){
	int current=dfa.s0;
	for(char c:input){
		int a=dfa.idx(c);
		if(a<0){
			return false;
		}
		current=dfa.delta[current][a];
		if(current==MUERTO){
			return false;
		}
	}
	return dfa.esFinal[current];
}


void run_test_suite(DFA& dfa,vector<string>& accept_tests,vector<string>& reject_tests){
	int passed=0;
	int total=accept_tests.size()+reject_tests.size();

	cout<<"\n[Corriendo Casos de Aceptacion]\n";
	for(string& s:accept_tests){
		bool res=test_string(dfa,s);
		cout<<"Cadena \""<<s<<"\": "<<(res?"PASS":"FAIL")<<"\n";
		passed+=res;
	}

	cout<<"\n[Corriendo Casos de Rechazo]\n";
	for(string& s:reject_tests){
		bool res=!test_string(dfa,s);
		cout<<"Cadena \""<<s<<"\": "<<(res?"PASS":"FAIL")<<"\n";
		passed+=res;
	}

	cout<<"\nResultado: "<<passed<<"/"<<total<<" pruebas superadas.\n";
}

int main(){
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string regex,sigma;
	cout<<"Expresion regular: ";
	cin>>regex;
	cout<<"Alfabeto: ";
	cin>>sigma;

	vector<string> accept_strings={
	};

	vector<string> reject_strings={
	};

	return 0;
}
