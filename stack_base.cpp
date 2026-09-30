#include <bits/stdc++.h>
using namespace std;


const int N = 200005;   // <-- máx. operaciones

struct Node {
    long long val; 
    long long agr;  // agregado de este nodo hacia abajo (mínimo en este ejemplo)
    int tam;  
    int prev;  
} nodo[N];

int cnt = 0; 
int root[N]; 

int push(int v, long long x) {
    long long agr = (v == -1) ? x : min(x, nodo[v].agr);  
     // <-- CAMBIA min por: max / x + nodo[v].agr / x ^ nodo[v].agr / __gcd(...)
    
    int tam = (v == -1) ? 1 : nodo[v].tam + 1;
    nodo[cnt] = {x, agr, tam, v};
    return cnt++;
}

int pop(int v) { return v == -1 ? -1 : nodo[v].prev; } 

long long query(int v) {
    // Implementación de la consulta
    if (v == -1) return LLONG_MAX;    // stack vacío -> valor neutro (min: LLONG_MAX | max: LLONG_MIN | suma/xor: 0)
    return nodo[v].agr;

}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    root[0] = -1; 
    for (int i = 1; i <= q; i++) {
        string tipo;
        cin >> tipo;
        if (tipo == "push") { 
            int t; long long x;
            cin >> t >> x;
            root[i] = push(root[t], x);
        } else if (tipo == "pop") {  
            int t;
            cin >> t;
            root[i] = pop(root[t]);
        } else {   
            int t;
            cin >> t;
            cout << query(root[t]) << '\n';
            root[i] = root[t];
        }
    }
    return 0;
}
