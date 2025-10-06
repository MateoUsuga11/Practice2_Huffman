// src/main.cpp
// Implementación de Huffman + Canonical Huffman
// Compilar: g++ -std=c++17 -O2 -Wall -Wextra src/main.cpp -o huffman

#include <bits/stdc++.h>
using namespace std;
using ull = unsigned long long;

struct Node {
    char ch; // carácter (válido sólo para hojas)
    uint64_t freq;
    Node* left;
    Node* right;
    unsigned char min_char; // mínimo carácter ASCII en el subárbol (para desempates)

    Node(char c, uint64_t f) : ch(c), freq(f), left(nullptr), right(nullptr), min_char((unsigned char)c) {}
    Node(Node* l, Node* r) : ch(0), freq(l->freq + r->freq), left(l), right(r) {
        min_char = min(l->min_char, r->min_char);
    }
    bool is_leaf() const { return left == nullptr && right == nullptr; }
};

// Comparador para priority_queue (min-heap)
// Queremos el nodo de menor freq; si empate, el de menor min_char.
struct NodeComp {
    bool operator()(const Node* a, const Node* b) const {
        if (a->freq != b->freq) return a->freq > b->freq; // menor freq -> mayor prioridad
        return a->min_char > b->min_char; // menor ASCII -> mayor prioridad
    }
};

// Helper para mostrar caracteres legibles
string show_char(char c) {
    if (c == ' ') return "' ' (space)";
    // ASCII 32..126 son imprimibles, otros no los esperamos según el enunciado
    return string(1, c);
}

// Construye códigos (no-canónicos) y longitudes desde el árbol (DFS)
void build_codes(Node* node, const string &cur, unordered_map<char,string>& codes, unordered_map<char,int>& lengths) {
    if (!node) return;
    if (node->is_leaf()) {
        string code = cur;
        if (code.empty()) code = "0"; // caso: único símbolo -> longitud 1
        codes[node->ch] = code;
        lengths[node->ch] = (int)code.size();
        return;
    }
    if (node->left) build_codes(node->left, cur + "0", codes, lengths);
    if (node->right) build_codes(node->right, cur + "1", codes, lengths);
}

// Convierte entero a string binaria (MSB...LSB) sin padding
string toBinary(ull v) {
    if (v == 0) return "0";
    string s;
    while (v) {
        s.push_back((v & 1) ? '1' : '0');
        v >>= 1;
    }
    reverse(s.begin(), s.end());
    return s;
}

// Convierte entero a bitstring de longitud 'len' (con ceros a la izquierda)
string toBinaryPadded(ull v, int len) {
    string s = toBinary(v);
    if ((int)s.size() < len) s = string(len - s.size(), '0') + s;
    return s;
}

// Visualización textual del árbol (preorder con paréntesis)
string tree_preorder(Node* node) {
    if (!node) return string();
    if (node->is_leaf()) {
        stringstream ss;
        ss << "'" << (node->ch == ' ' ? ' ' : node->ch) << "':" << node->freq;
        return ss.str();
    }
    string left = tree_preorder(node->left);
    string right = tree_preorder(node->right);
    stringstream ss;
    ss << "(" << node->freq << " " << left << " " << right << ")";
    return ss.str();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string input;
    cout << "Enter a line of text (ASCII 32-126). It must have at least 30 characters:\n";
    if (!std::getline(cin, input)) {
        cerr << "No input received.\n";
        return 1;
    }
    if ((int)input.size() < 30) {
        cerr << "ERROR: input must have at least 30 characters. (you entered " << input.size() << ")\n";
        return 1;
    }

    // 1) Frecuencias
    unordered_map<char, uint64_t> freq;
    for (unsigned char c : input) freq[(char)c]++;

    // Mostrar tabla de frecuencias (ordenada por símbolo)
    map<unsigned char, uint64_t> freq_sorted;
    for (auto &p : freq) freq_sorted[(unsigned char)p.first] = p.second;

    cout << "\n=== Frequency table ===\n";
    for (auto &p : freq_sorted) {
        cout << show_char((char)p.first) << "\t: " << p.second << "\n";
    }

    // 2) Construir árbol de Huffman
    priority_queue<Node*, vector<Node*>, NodeComp> pq;
    for (auto &p : freq) {
        Node* leaf = new Node(p.first, p.second);
        pq.push(leaf);
    }

    if (pq.empty()) {
        cerr << "ERROR: empty input.\n";
        return 1;
    }

    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        Node* parent = new Node(a, b);
        pq.push(parent);
    }
    Node* root = pq.top();

    // 3) Obtener códigos no-canónicos y longitudes
    unordered_map<char,string> noncanonical;
    unordered_map<char,int> lengths;
    build_codes(root, "", noncanonical, lengths);

    // 4) Preparar lista (symbol, length) y construir códigos canónicos
    vector<pair<char,int>> syms;
    for (auto &p : lengths) syms.push_back({p.first, p.second});
    sort(syms.begin(), syms.end(), [](const pair<char,int>& a, const pair<char,int>& b){
        if (a.second != b.second) return a.second < b.second; // length asc
        return (unsigned char)a.first < (unsigned char)b.first; // symbol asc (ASCII)
    });

    int max_len = 0;
    for (auto &p : syms) if (p.second > max_len) max_len = p.second;
    if (max_len == 0) max_len = 1;

    vector<int> bl_count(max_len + 1, 0); // bl_count[len] = count
    for (auto &p : syms) bl_count[p.second]++;

    // next_code[len] algorithm
    vector<ull> next_code(max_len + 1, 0);
    ull code = 0;
    for (int bits = 1; bits <= max_len; ++bits) {
        code = (code + (ull)bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }

    unordered_map<char,string> canonical;
    for (auto &p : syms) {
        char ch = p.first;
        int len = p.second;
        ull assigned = next_code[len]++;
        string bits = toBinaryPadded(assigned, len);
        canonical[ch] = bits;
    }

    // 5) Mostrar longitudes, códigos no-canónicos y canónicos (orden por ASCII)
    cout << "\n=== Codes and lengths per symbol ===\n";
    cout << "Symbol\tFreq\tLen\tHuffman(non-canonical)\tCanonical\n";
    vector<unsigned char> symbols_ordered;
    for (auto &p : freq_sorted) symbols_ordered.push_back(p.first);
    sort(symbols_ordered.begin(), symbols_ordered.end());
    for (auto sc : symbols_ordered) {
        char c = (char)sc;
        cout << show_char(c) << "\t" << freq[(char)c] << "\t";
        cout << lengths[c] << "\t";
        cout << (noncanonical.count(c) ? noncanonical[c] : "?") << "\t\t";
        cout << (canonical.count(c) ? canonical[c] : "?") << "\n";
    }

    // 6) Visualización textual del árbol
    cout << "\n=== Huffman Tree (textual preorder) ===\n";
    cout << tree_preorder(root) << "\n";

    // 7) Codificar la entrada con códigos canónicos
    string bitstream;
    bitstream.reserve(input.size() * 4);
    uint64_t compressed_bits = 0;
    for (unsigned char uc : input) {
        char c = (char)uc;
        auto it = canonical.find(c);
        if (it == canonical.end()) {
            cerr << "ERROR: symbol without canonical code (impossible)\n";
            return 1;
        }
        bitstream += it->second;
        compressed_bits += it->second.size();
    }

    uint64_t original_bits = (uint64_t)input.size() * 8ULL;
    double ratio = original_bits ? (double)compressed_bits / (double)original_bits : 0.0;
    double reduction = (1.0 - ratio) * 100.0;

    // 8) Mostrar primeras 128 bits y tamaño total. También hex (agrupando bytes).
    cout << "\n=== Compression ===\n";
    cout << "Original (bits) : " << original_bits << " (" << input.size() << " chars x 8)\n";
    cout << "Compressed (bits): " << compressed_bits << "\n";
    cout << fixed << setprecision(6);
    cout << "Ratio: " << ratio << "    Reduction: " << reduction << "%\n";

    size_t firstN = min<size_t>(128, bitstream.size());
    cout << "\nFirst " << firstN << " bits (or fewer if the stream is shorter):\n";
    if (bitstream.empty()) cout << "(empty)\n"; else cout << bitstream.substr(0, firstN) << "\n";

    // Hex representation (pad al final con ceros hasta múltiplo de 8)
    string padded = bitstream;
    while (padded.size() % 8 != 0) padded.push_back('0');
    stringstream hexss;
    for (size_t i = 0; i < padded.size(); i += 8) {
        string byte = padded.substr(i, 8);
        unsigned int val = 0;
        for (char b : byte) {
            val = (val << 1) | (b == '1' ? 1U : 0U);
        }
        hexss << uppercase << hex << setw(2) << setfill('0') << (val & 0xFF);
        if (i + 8 < padded.size()) hexss << " ";
    }
    cout << "\nStream (hex, MSB-first bytes, last byte padded with zeros):\n";
    cout << hexss.str() << "\n";

    cout << "\n(Only the first " << min<size_t>(128, bitstream.size()) << " bits are shown above; total size in bits: " << compressed_bits << ")\n";

    // Liberar nodos (opcional)
    // Nota: para simplicidad no se hace borrado recursivo exhaustivo aquí (corto-lived program).

    return 0;
}