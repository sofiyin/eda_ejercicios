#include <bits/stdc++.h>
using namespace std;


const int MAXN  = 100000;
const int LOG   = 14; 
const int MAXNODE = MAXN * (LOG + 2) + 5;

// child[node][0/1] = hijo del nodo según el bit 0 o 1
int child[MAXNODE][2];
int cant_num[MAXNODE];   // cuántos números "pasan" por este nodo (para la resta de versiones)

int root[MAXN + 1];      // root[i] = raíz de la versión con x[1..i] insertados

int nodes = 0;           // nodes = 0 siempre representa el "nodo vacío" (sentinel)
 // child[0][*] = 0 y cant_num[0] = 0 por estar en un array global

int cloneNode(int old) {
    ++nodes;
    child[nodes][0] = child[old][0];
    child[nodes][1] = child[old][1];
    cant_num[nodes] = cant_num[old];
    return nodes;
}

void resetTrie() {
    nodes = 0;      // el nodo 0 sigue sirviendo de sentinel vacío
    root[0] = 0;    // versión 0 = trie vacío
}

int insert(int oldRoot, int x) {
    int newRoot = cloneNode(oldRoot);
    cant_num[newRoot]++;

    int old    = oldRoot;
    int actual = newRoot;

    for (int b = LOG; b >= 0; b--) {
        int bit = (x >> b) & 1;

        int oldChild = child[old][bit];
        int newChild = cloneNode(oldChild);
        cant_num[newChild]++;

        child[actual][bit] = newChild;

        old    = oldChild;
        actual = newChild;
    }
    return newRoot;
}
// QUERY: max(a XOR v) para v en el rango representado por
// [rootL = root[l-1], rootR = root[r]].
int queryMaxXOR(int rootR, int rootL, int a) {
    int u = rootR;
    int v = rootL;
    int answer = 0;

    for (int b = LOG; b >= 0; b--) {
        int bit    = (a >> b) & 1;
        int wanted = bit ^ 1;   // queremos el bit contrario para maximizar el XOR

        int rightChild = child[u][wanted];
        int leftChild  = child[v][wanted];
        int amount = cant_num[rightChild] - cant_num[leftChild];

        if (amount > 0) {
            answer |= (1 << b);
            u = rightChild;
            v = leftChild;
        } else {
            u = child[u][bit];
            v = child[v][bit];
        }
    }
    return answer;
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;

    while (T--) {
        resetTrie();

        int n, q;
        cin >> n >> q;

        vector<int> x(n + 1);
        for (int i = 1; i <= n; i++) cin >> x[i];

        for (int i = 1; i <= n; i++) {
            root[i] = insert(root[i - 1], x[i]);
        }

        while (q--) {
            int a, l, r;
            cin >> a >> l >> r;
            cout << queryMaxXOR(root[r], root[l - 1], a) << '\n';
        }
    }

    return 0;
}