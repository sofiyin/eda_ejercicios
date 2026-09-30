#include <bits/stdc++.h>
using namespace std;

/*
    SEGMENT TREE PERSISTENTE — plantilla base

    update() no modifica nada: copia solo el camino raíz->hoja y reusa el resto.
    root[i] = raíz de la versión i.
    El nodo 0 es el "vacío": hijos = 0 y val = 0. Una versión vacía es root = 0
    y nunca hay que chequear nulos (cualquier rama sin usar cae en el 0).

    Dos usos típicos (cambia solo el main):
     (A) VIAJE EN EL TIEMPO: arreglo + operaciones. query(root[t], ...) responde
         "cómo estaba el rango justo después de la operación t".
     (B) FRECUENCIAS POR VALOR: el árbol se indexa por VALOR (comprimido) y
         root[i] guarda los valores de a[1..i]. Para las posiciones [l, r] se
         comparan las versiones root[l-1] y root[r] (lo que hay en r menos lo
         que había en l-1), igual que en el trie de bits.
*/

const int N = 200005;                       // <-- máx. elementos / operaciones
const int LOG = 18;                         // techo(log2(N))
const int NODES = 2 * N + N * (LOG + 2) + 5;

// ================= LO QUE CAMBIAS SEGÚN EL PROBLEMA =================
const long long NEUTRO = 0;   // suma/xor/conteo: 0 | min: LLONG_MAX | max: LLONG_MIN | gcd: 0
long long combinar(long long a, long long b) { return a + b; }        // min(a,b), max(a,b), __gcd(a,b), a^b ...
long long aplicar(long long viejo, long long x) { return viejo + x; } // "sumar x" (frecuencia: x = 1). Para ASIGNAR: return x;
// ====================================================================

struct Node { int izq, der; long long val; } nodo[NODES];
int cntNodes = 1;      // el 0 está reservado como nodo vacío
int root[N + 5];       // root[i] = raíz de la versión i

int nuevoNodo(int izq, int der, long long val) {
    nodo[cntNodes] = {izq, der, val};
    return cntNodes++;
}

// Solo para (A): construye la versión 0 desde el arreglo a[1..n]
int build(int l, int r, vector<long long>& a) {
    if (l == r) return nuevoNodo(0, 0, a[l]);
    int m = (l + r) / 2;
    int i = build(l, m, a), d = build(m + 1, r, a);
    return nuevoNodo(i, d, combinar(nodo[i].val, nodo[d].val));
}

// Devuelve la NUEVA raíz con la posición pos modificada (viejo queda intacto)
int update(int viejo, int l, int r, int pos, long long x) {
    if (l == r) return nuevoNodo(0, 0, aplicar(nodo[viejo].val, x));
    int m = (l + r) / 2;
    int i = nodo[viejo].izq, d = nodo[viejo].der;
    if (pos <= m) i = update(i, l, m, pos, x);     // solo se recrea el camino de pos,
    else          d = update(d, m + 1, r, pos, x); // el otro hijo se comparte
    return nuevoNodo(i, d, combinar(nodo[i].val, nodo[d].val));
}

// ---------- (A) query sobre UNA versión: agregado de [ql, qr] ----------
long long query(int u, int l, int r, int ql, int qr) {
    if (qr < l || r < ql) return NEUTRO;           // no se cruzan
    if (ql <= l && r <= qr) return nodo[u].val;    // totalmente dentro
    int m = (l + r) / 2;
    return combinar(query(nodo[u].izq, l, m, ql, qr),
                    query(nodo[u].der, m + 1, r, ql, qr));
}

// ---------- (B) entre dos versiones (a = root[l-1], b = root[r]); solo con suma/conteo ----------
// Conteo de valores comprimidos en [ql, qr] dentro de las posiciones (l..r)
long long queryDif(int a, int b, int l, int r, int ql, int qr) {
    if (qr < l || r < ql) return 0;
    if (ql <= l && r <= qr) return nodo[b].val - nodo[a].val;   // frecuencia(r) - frecuencia(l-1)
    int m = (l + r) / 2;
    return queryDif(nodo[a].izq, nodo[b].izq, l, m, ql, qr)
         + queryDif(nodo[a].der, nodo[b].der, m + 1, r, ql, qr);
}

// k-ésimo menor valor (comprimido) entre las posiciones (l..r). k = 1 -> el menor
int kesimo(int a, int b, int l, int r, int k) {
    if (l == r) return l;
    int m = (l + r) / 2;
    long long izq = nodo[nodo[b].izq].val - nodo[nodo[a].izq].val;  // cuántos van por la izquierda
    if (k <= izq) return kesimo(nodo[a].izq, nodo[b].izq, l, m, k);
    return kesimo(nodo[a].der, nodo[b].der, m + 1, r, k - izq);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    //  k-ésimo menor en a[l..r]
    int n, q;
    cin >> n >> q;
    vector<int> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];

    vector<int> vals(a.begin() + 1, a.end());   // compresión de coordenadas
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    int V = vals.size();
    auto comp = [&](int x) { return int(lower_bound(vals.begin(), vals.end(), x) - vals.begin()); };

    root[0] = 0;                                                  // versión 0 = vacío
    for (int i = 1; i <= n; i++)
        root[i] = update(root[i - 1], 0, V - 1, comp(a[i]), 1);   // +1 a la frecuencia de a[i]

    while (q--) {
        int l, r, k;
        cin >> l >> r >> k;
        cout << vals[kesimo(root[l - 1], root[r], 0, V - 1, k)] << '\n';
        // conteo de valores en [lo, hi] dentro de l..r:
        //   queryDif(root[l-1], root[r], 0, V-1, comp(lo), comp(hi))   (lo/hi deben existir en vals)
    }

}
