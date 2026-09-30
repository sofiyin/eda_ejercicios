#include <bits/stdc++.h>
using namespace std;



const int MAXN = 200005;   // <-- AJUSTA: tamaño máximo del arreglo inicial
const int MAXQ = 200005;   // <-- AJUSTA: máximo número de operaciones
const int LEVELS = 18;     // techo(log2(MAXN)) — con MAXN=2*10^5 basta 18
const int NODES = 2 * MAXN + MAXQ * (LEVELS + 2) + 5;

struct Node {
    int izq, der;     // hijos (índices en `nodo`); 0 = no tiene (hoja)
    long long val;    // agregado de este subárbol: suma
} nodo[NODES];         // pool: TODAS las versiones viven aquí

int cnt = 0;              // próximo índice libre del pool
int root[MAXQ + 1];       // root[t] = raíz de la versión tras la operación t

int newLeaf(long long v) {
    nodo[cnt] = {0, 0, v};
    return cnt++;
}

// izq, der : hijos ya existentes que este nodo va a enlazar
int newInternal(int izq, int der) {
    nodo[cnt] = {izq, der, nodo[izq].val + nodo[der].val};   // combinación: suma
    return cnt++;
}

int build(int l, int r, vector<long long>& a) {
    if (l == r) return newLeaf(a[l]);
    int m = (l + r) / 2;
    int izq = build(l, m, a);
    int der = build(m + 1, r, a);
    return newInternal(izq, der);
}

// viejo : raíz del subárbol de la versión anterior (queda intacto)
// devuelve la NUEVA raíz de este subárbol, con a[pos] = val
int update(int viejo, int l, int r, int pos, long long val) {
    if (l == r) return newLeaf(val);   // <-- si es "sumar" en vez de "asignar": newLeaf(nodo[viejo].val + val)
    int m = (l + r) / 2;
    if (pos <= m)   // se clona el camino izquierdo, el derecho se comparte
        return newInternal(update(nodo[viejo].izq, l, m, pos, val), nodo[viejo].der);
    else            // se comparte el izquierdo, se clona el derecho
        return newInternal(nodo[viejo].izq, update(nodo[viejo].der, m + 1, r, pos, val));
}

// Los 3 casos de todo segment tree:
//   1) [l,r] no toca [ql,qr]        -> aporta el NEUTRO
//   2) [l,r] dentro de [ql,qr]      -> aporta nodo[u].val ya calculado
//   3) cruce parcial                -> baja por ambos hijos y combina
long long query(int u, int l, int r, int ql, int qr) {
    if (qr < l || r < ql) return 0;            // caso 1: neutro
    if (ql <= l && r <= qr) return nodo[u].val;        // caso 2: se usa directo
    int m = (l + r) / 2;
    long long izq = query(nodo[u].izq, l, m, ql, qr);         // resultado de la mitad izquierda
    long long der = query(nodo[u].der, m + 1, r, ql, qr);     // resultado de la mitad derecha
    return izq + der;   // misma combinación que en newInternal
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    vector<long long> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];

    root[0] = build(1, n, a);   // versión 0 = arreglo inicial

    for (int i = 1; i <= q; i++) {
        string tipo;
        cin >> tipo;

        if (tipo == "update") {
            int t, pos; long long val;
            cin >> t >> pos >> val;
            root[i] = update(root[t], 1, n, pos, val);
        } else { // "query"
            int t, l, r; //t es la versión, l y r son los indices del rango
            cin >> t >> l >> r;
            cout << query(root[t], 1, n, l, r) << '\n';
            root[i] = root[t];   // no genera versión nueva
        }
        //mi v 0 es mi version arreglo og, luego l y r son los indices
    }
    return 0;
}
