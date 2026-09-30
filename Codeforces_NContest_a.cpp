#include <bits/stdc++.h>
using namespace std;

struct PersistentTrie {
    static const int ALPHA = 26;
    vector<array<int, ALPHA>> ch; // hijos de cada nodo
    vector<int> cnt;  // palabras que pasan por el nodo

    PersistentTrie(int maxNodes) {
        ch.reserve(maxNodes);
        cnt.reserve(maxNodes);
        // nodo 0 = nodo nulo (trie vacío)
        array<int, ALPHA> z; z.fill(0);
        ch.push_back(z);
        cnt.push_back(0);
    }

    // crea una copia del nodo 'old' con una palabra más pasando por él
    int cloneNode(int old) {
        ch.push_back(ch[old]);
        cnt.push_back(cnt[old] + 1);
        return (int)ch.size() - 1;
    }

    // devuelve la raíz de una nueva versión = versión 'root' + palabra s
    int insert(int root, const string &s) {
        int newRoot = cloneNode(root);
        int cur = newRoot;
        for (char c : s) {
            int k = c - 'a';
            int nw = cloneNode(ch[cur][k]);
            ch[cur][k] = nw;   // cur es un nodo nuevo, se puede modificar
            cur = nw;
        }
        return newRoot;
    }

    // cantidad de palabras (con repeticiones) de la versión 'root'
    // que tienen a p como prefijo
    int query(int root, const string &p) {
        int cur = root;
        for (char c : p) {
            cur = ch[cur][c - 'a'];
            if (cur == 0) return 0;
        }
        return cnt[cur];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Q;
    cin >> Q;

    // nodos máximos: suma de longitudes + una raíz por inserción + nodo nulo
    PersistentTrie trie(500000 + 200000 + 5);

    vector<int> root(Q + 1, 0); // root[i] = raíz del estado i; root[0] = vacío
    string out;

    for (int i = 1; i <= Q; i++) {
        int type;
        cin >> type;
        if (type == 1) {
            string s;
            cin >> s;
            root[i] = trie.insert(root[i - 1], s);
        } else if (type == 2) {
            int t;
            cin >> t;
            root[i] = root[t];
        } else {
            string p;
            cin >> p;
            root[i] = root[i - 1];
            out += to_string(trie.query(root[i], p));
            out += '\n';
        }
    }

    cout << out;
    return 0;
}