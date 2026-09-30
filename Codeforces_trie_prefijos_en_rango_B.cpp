#include <bits/stdc++.h>
using namespace std;

/*
    ================================================================
    TRIE DE STRINGS PERSISTENTE — PREFIJOS EN RANGO [l, r]
    (CF: "Prefijos en Rango")
    ================================================================
    query l r p -> cuántas de s_l..s_r tienen a p como prefijo.
    Igual que trie_entre_versiones.cpp pero con rango [l, r] inclusivo
    en vez de (a, b]: se usa root[l-1] y root[r].

    FORMATO DE ENTRADA:
        n q
        s_1 ... s_n         (la string i crea la versión i)
        l r p   (una línea por query, q líneas)

    Cómo funciona: insertar una string clona solo los nodos de su
    camino (|s|+1) y comparte el resto. root[i] es un trie completo
    con las primeras i strings. El nodo 0 es el "vacío universal"
    (hijos=0, contadores=0): no hay que chequear nulos.

    Cada nodo guarda:  cnt = cuántas strings pasan por él (prefijo)
                       fin = cuántas terminan EXACTAMENTE en él
    ================================================================
*/

const int ALPHA = 26;           // 'a'-'z'
const int MAXSTR = 200000;      // n, q <= 2*10^5
const int MAXTOTALLEN = 500000; // suma de |s_i| <= 5*10^5
const int NODES = MAXTOTALLEN + MAXSTR + 5;

struct Node {
    int hijo[ALPHA];   // hijo[c]: índice del hijo por el carácter c; 0 = no existe
    int cnt;           // strings que comparten el prefijo de este nodo
    int fin;           // strings que terminan exactamente aquí
    bool esFinal;      // true si alguna string termina aquí
} nodo[NODES];

int cntNodes = 1;              // próximo índice libre; el 0 está reservado
int root[MAXSTR + 1];          // root[i] = raíz tras insertar i strings; root[0] = 0

int clone(int old) {
    nodo[cntNodes] = nodo[old];
    return cntNodes++;
}

// viejo : nodo actual de la versión anterior;  s : string;  i : posición actual en s
// devuelve el índice del nuevo nodo que reemplaza a `viejo`
int insertar(int viejo, const string& s, int i = 0) {
    int nuevo = clone(viejo);
    nodo[nuevo].cnt++;
    if (i == (int)s.size()) {
        nodo[nuevo].esFinal = true;
        nodo[nuevo].fin++;
        return nuevo;
    }
    int c = s[i] - 'a';
    nodo[nuevo].hijo[c] = insertar(nodo[viejo].hijo[c], s, i + 1);   // solo se reescribe esta rama
    return nuevo;
}

// vA = root[l-1], vB = root[r]. Se baja por ambos tries a la vez y se resta.
int query(int vA, int vB, const string& p) {
    int u = vB, w = vA;
    for (char ch : p) {
        int c = ch - 'a';
        u = nodo[u].hijo[c];
        w = nodo[w].hijo[c];
        if (u == 0) return 0;      // en vB ni existía ese prefijo: no puede haber diferencia
    }
    return nodo[u].cnt - nodo[w].cnt;   // (con prefijo p hasta r) - (con prefijo p hasta l-1)
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    root[0] = 0;   // versión 0 = trie vacío
    for (int i = 1; i <= n; i++) {
        string s;
        cin >> s;
        root[i] = insertar(root[i - 1], s);
    }

    while (q--) {
        int l, r; string p;
        cin >> l >> r >> p;
        cout << query(root[l - 1], root[r], p) << '\n';
    }
    return 0;
}
