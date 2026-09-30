#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

template <class T, class Cmp>
struct Heap {                                            // heap binario genérico
    vector<T> a;                                         // arreglo que representa el árbol
    Cmp cmp;                                             // cmp(x,y)=true si x sale antes que y
    bool empty() const { return a.empty(); }
    const T& top() const { return a[0]; }                // el de mayor prioridad está en la raíz
    void push(const T& x) {
        a.push_back(x);                                  // se inserta al final
        int i = (int)a.size() - 1;
        while (i > 0) {                                  // sift up: sube mientras sea mejor que su padre
            int p = (i - 1) / 2;                         // índice del padre
            if (cmp(a[i], a[p])) { swap(a[i], a[p]); i = p; }
            else break;
        }
    }
    void pop() {
        a[0] = a.back();                                 // el último pasa a la raíz
        a.pop_back();
        int n = (int)a.size(), i = 0;
        while (true) {                                   // sift down: baja hacia el mejor hijo
            int best = i, l = 2 * i + 1, r = 2 * i + 2;  // hijos izquierdo y derecho
            if (l < n && cmp(a[l], a[best])) best = l;
            if (r < n && cmp(a[r], a[best])) best = r;
            if (best == i) break;                        // ya está en su lugar
            swap(a[i], a[best]);
            i = best;
        }
    }
};

const int MAXB = 100000 + 20;                            // bits posibles (2^1e5 más acarreos)
const int MAXNODES = 12000000;                           // nodos del árbol persistente
const int MOD1 = 1000000007, MOD2 = 998244353;           // MOD1 da la respuesta, MOD2 es 2do hash

int Lc[MAXNODES], Rc[MAXNODES], Cnt[MAXNODES], H1[MAXNODES], H2[MAXNODES]; // hijos, #unos, hashes
int tot = 0;                                             // nodo 0 = subárbol de puros ceros
int pw1[MAXB + 1], pw2[MAXB + 1];                        // pw1[i]=2^i mod, pw2[i]=base^i mod

int newNode(int from) {                                  // copia un nodo (persistencia)
    ++tot;
    Lc[tot] = Lc[from]; Rc[tot] = Rc[from];
    Cnt[tot] = Cnt[from]; H1[tot] = H1[from]; H2[tot] = H2[from];
    return tot;
}
void pull(int o) {                                       // recalcula el nodo desde sus hijos
    Cnt[o] = Cnt[Lc[o]] + Cnt[Rc[o]];
    H1[o] = (H1[Lc[o]] + H1[Rc[o]]) % MOD1;              // suma de 2^i de los bits en 1
    H2[o] = (H2[Lc[o]] + H2[Rc[o]]) % MOD2;
}
int findZero(int o, int l, int r, int x) {               // primer bit en 0 con posición >= x
    if (r < x || Cnt[o] == r - l + 1) return -1;         // fuera de rango o todo en unos
    if (l == r) return l;
    int m = (l + r) / 2;
    int res = findZero(Lc[o], l, m, x);                  // primero busca en la mitad baja
    if (res != -1) return res;
    return findZero(Rc[o], m + 1, r, x);
}
int clearRange(int o, int l, int r, int ql, int qr) {    // pone en 0 los bits [ql, qr]
    if (o == 0 || qr < l || r < ql) return o;            // ya es cero o no se toca
    if (ql <= l && r <= qr) return 0;                    // rango completo -> nodo cero
    int n = newNode(o), m = (l + r) / 2;
    Lc[n] = clearRange(Lc[o], l, m, ql, qr);
    Rc[n] = clearRange(Rc[o], m + 1, r, ql, qr);
    pull(n);
    return n;
}
int setBit(int o, int l, int r, int p) {                 // pone en 1 el bit p
    int n = newNode(o);
    if (l == r) { Cnt[n] = 1; H1[n] = pw1[l]; H2[n] = pw2[l]; return n; }
    int m = (l + r) / 2;
    if (p <= m) Lc[n] = setBit(Lc[o], l, m, p);
    else        Rc[n] = setBit(Rc[o], m + 1, r, p);
    pull(n);
    return n;
}
int addPow(int root, int x) {                            // nuevo número = root + 2^x
    int p = findZero(root, 0, MAXB, x);                  // hasta dónde llega el acarreo
    if (p > x) root = clearRange(root, 0, MAXB, x, p - 1); // los unos del acarreo pasan a 0
    return setBit(root, 0, MAXB, p);                     // el bit p pasa a 1
}
int cmpNum(int a, int b, int l = 0, int r = MAXB) {      // <0 si a<b, 0 si iguales, >0 si a>b
    if (H1[a] == H1[b] && H2[a] == H2[b]) return 0;      // mismos hashes -> iguales
    if (l == r) return Cnt[a] - Cnt[b];                  // bit donde difieren
    int m = (l + r) / 2;
    if (H1[Rc[a]] != H1[Rc[b]] || H2[Rc[a]] != H2[Rc[b]])
        return cmpNum(Rc[a], Rc[b], m + 1, r);           // difieren en bits altos -> ir a la derecha
    return cmpNum(Lc[a], Lc[b], l, m);                   // si no, en los bajos
}

struct DistCmp {                                         // comparador del heap: menor distancia primero
    bool operator()(const pair<int,int>& a, const pair<int,int>& b) const {
        return cmpNum(a.second, b.second) < 0;           // .second = raíz del número distancia
    }
};

void dijkstra(int n, int s, const vector<vector<pair<int,int>>>& adj,
              vector<int>& dist, vector<int>& par) {
    dist.assign(n + 1, -1);                              // -1 = infinito
    par.assign(n + 1, -1);                               // padre para reconstruir el camino
    vector<bool> done(n + 1, false);                     // vértices ya finalizados
    Heap<pair<int,int>, DistCmp> h;
    dist[s] = 0;                                         // nodo 0 = número 0
    h.push({s, 0});
    while (!h.empty()) {
        int u = h.top().first; h.pop();                  // vértice con menor distancia
        if (done[u]) continue;                           // entrada vieja, se ignora
        done[u] = true;
        for (auto [v, x] : adj[u]) {
            if (done[v]) continue;
            int nd = addPow(dist[u], x);                 // dist[u] + 2^x sin modificar dist[u]
            if (dist[v] == -1 || cmpNum(nd, dist[v]) < 0) { // relajación
                dist[v] = nd;
                par[v] = u;
                h.push({v, nd});
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    int base2 = rng() % (MOD2 - 1000) + 500;             // base aleatoria para el 2do hash
    pw1[0] = pw2[0] = 1;
    for (int i = 1; i <= MAXB; i++) {                    // precálculo de potencias
        pw1[i] = (ll)pw1[i - 1] * 2 % MOD1;
        pw2[i] = (ll)pw2[i - 1] * base2 % MOD2;
    }

    int n, m;
    cin >> n >> m;
    vector<vector<pair<int,int>>> adj(n + 1);            // adj[u] = {(v, exponente x)}
    for (int i = 0; i < m; i++) {
        int u, v, x;
        cin >> u >> v >> x;
        adj[u].push_back({v, x});                        // grafo no dirigido
        adj[v].push_back({u, x});
    }
    int s, t;
    cin >> s >> t;

    vector<int> dist, par;
    dijkstra(n, s, adj, dist, par);

    if (dist[t] == -1) {                                 // t no alcanzable
        cout << -1 << "\n";
        return 0;
    }
    vector<int> path;
    for (int v = t; v != -1; v = par[v]) path.push_back(v); // se recorre de t hacia s
    reverse(path.begin(), path.end());

    cout << H1[dist[t]] << "\n" << path.size() << "\n";  // H1 = distancia mod 1e9+7
    for (int v : path) cout << v << " ";
    cout << "\n";
    return 0;
}