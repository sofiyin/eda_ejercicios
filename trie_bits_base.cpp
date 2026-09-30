#include <bits/stdc++.h>
using namespace std;

/*
    TRIE PERSISTENTE DE NÚMEROS (bits) — plantilla base

    Cada número se inserta como una cadena de L bits (del más significativo al menos).
    root[i] = trie con a[1..i]. Cada nodo acumula `frec` = cuántos valores
    pasan por él (tienen ese prefijo de bits).

    Para consultar las posiciones [l, r] se bajan a la vez root[l-1] y root[r]:
        frec[nodo en r] - frec[nodo en l-1] > 0   <=>   hay al menos un valor en [l, r] por ese camino
    El nodo 0 es el vacío (hijos = 0, frec = 0): root = 0 es un trie vacío
    y no hay que chequear -1 en ningún lado.
*/

const int L = 30; // <-- bits: valores < 2^L (1e9 -> 30; hasta 65535 -> 16)
const int N = 100005;  // <-- máx. valores a insertar
const int NODES = N * (L + 1) + 5;     // cada insert crea L+1 nodos

int hijo[2][NODES];   // hijo[bit][nodo]
int frec[NODES];      // cuántos valores pasan por el nodo
int cntNodes = 1;     // el 0 está reservado como nodo vacío
int root[N + 5];      // root[i] = raíz tras insertar i valores; root[0] = 0

int clonar(int v) {                    // copia v y cuenta un valor más
    hijo[0][cntNodes] = hijo[0][v];
    hijo[1][cntNodes] = hijo[1][v];
    frec[cntNodes] = frec[v] + 1;
    return cntNodes++;
}

// Devuelve la raíz de la versión nueva (viejo queda intacto)
int insertar(int viejo, int x) {
    int raiz = clonar(viejo), u = raiz;
    for (int i = L - 1; i >= 0; --i) {
        int c = (x >> i) & 1;
        int viejoHijo = hijo[c][viejo];
        hijo[c][u] = clonar(viejoHijo);    // solo se recrea la rama de x; la otra se comparte
        u = hijo[c][u];
        viejo = viejoHijo;
    }
    return raiz;
}

/*
    ============================ QUERY (RELLENA) ============================
    a = root[l-1], b = root[r]   (si solo hay una versión: a = 0)
    Ejemplo: MÁXIMO XOR entre x y algún valor de a[l..r]  (asume que [l,r] no está vacío)
    Idea: el bit más significativo pesa más que todos los demás juntos, así que
    en cada nivel se intenta ir por el bit OPUESTO al de x, si existe un valor por ahí.
    =========================================================================
*/
int query(int a, int b, int x) {
    int res = 0;
    for (int i = L - 1; i >= 0; --i) {
        int c = (x >> i) & 1, d = c ^ 1;                     // d = bit que nos conviene
        if (frec[hijo[d][b]] - frec[hijo[d][a]] > 0) {       // ¿hay algún valor en [l,r] por d?
            res |= (1 << i);
            a = hijo[d][a]; b = hijo[d][b];
        } else {
            a = hijo[c][a]; b = hijo[c][b];
        }
    }
    return res;
    // Otras preguntas con el mismo esqueleto:
    //   MÍNIMO XOR: intenta primero el bit igual (c) y solo si no hay, usa d (y suma ese bit).
    //   Cuántos valores con cierto prefijo de bits: baja por el prefijo y devuelve frec[b] - frec[a].
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    // Si hay varios casos de prueba: reinicia con cntNodes = 1 al empezar cada uno
    root[0] = 0;                                   // versión 0 = vacío
    for (int i = 1; i <= n; i++) {
        int x;
        cin >> x;
        root[i] = insertar(root[i - 1], x);
    }
    while (q--) {
        int x, l, r;
        cin >> x >> l >> r;
        cout << query(root[l - 1], root[r], x) << '\n';
    }
    return 0;
}
