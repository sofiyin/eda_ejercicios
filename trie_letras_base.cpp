#include <bits/stdc++.h>
using namespace std;

/*
    TRIE PERSISTENTE DE STRINGS (letras) — plantilla base

    La string i-ésima crea la versión i. Cada nodo guarda:
      frec = cuántas strings pasan por él (tienen ese prefijo)
      fin  = cuántas strings terminan EXACTAMENTE ahí
    Para las strings insertadas en las posiciones (a, b] se comparan
    root[a] y root[b]:  frec[b] - frec[a]. Si solo importa "hasta la versión t", usa a = 0.
    El nodo 0 es el vacío (hijos = 0, contadores = 0): no se chequean nulos.
*/

const int ALPHA = 26;  
const char BASE = 'a'; 
const int MAXSTR = 100000; 
const int MAXLEN = 200000; 
const int NODES = MAXLEN + MAXSTR + 5;

struct Node {
    int hijo[ALPHA];  
    int frec;// strings con este prefijo
    int fin; // strings que terminan aquí
} nodo[NODES];

int cntNodes = 1;       // el 0 está reservado como nodo vacío
int root[MAXSTR + 5];   // root[i] = raíz tras insertar i strings; root[0] = 0

int clonar(int v) {
    nodo[cntNodes] = nodo[v];
    nodo[cntNodes].frec++;
    return cntNodes++;
}

// Devuelve la raíz de la versión nueva (viejo queda intacto)
int insertar(int viejo, const string& s) {
    int raiz = clonar(viejo), u = raiz;
    for (char ch : s) {
        int c = ch - BASE;
        int viejoHijo = nodo[viejo].hijo[c];
        nodo[u].hijo[c] = clonar(viejoHijo);   // solo se recrea la rama de s; el resto se comparte
        u = nodo[u].hijo[c];
        viejo = viejoHijo;
    }
    nodo[u].fin++;
    return raiz;
}

/*
    ============================ QUERY (RELLENA) ============================
    a = root[a], b = root[b]   (a = 0 si no hay límite inferior)
    Ejemplo: cuántas strings con prefijo p hay entre las versiones (a, b].
    =========================================================================
*/
int query(int a, int b, const string& p) {
    for (char ch : p) {
        int c = ch - BASE;
        a = nodo[a].hijo[c];
        b = nodo[b].hijo[c];
        if (b == 0) return 0;         // ese prefijo no existe en la versión b
    }
    return nodo[b].frec - nodo[a].frec;
    // Otras preguntas con el mismo esqueleto:
    //   palabra EXACTA (cuántas veces): return nodo[b].fin - nodo[a].fin;
    //   ¿existe la palabra exacta?:     return nodo[b].fin - nodo[a].fin > 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    root[0] = 0; // versión 0 = vacío
    for (int i = 1; i <= n; i++) {
        string s;
        cin >> s;
        root[i] = insertar(root[i - 1], s);
    }
    int q;
    cin >> q;
    while (q--) {
        int a, b; string p;
        cin >> a >> b >> p;  // "hasta la versión t": a = 0, b = t
        cout << query(root[a], root[b], p) << '\n';
    }
    return 0;
}
