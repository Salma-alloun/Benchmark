#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

// ---------------- UTILITAIRE ----------------
long long nowMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

template<typename F>
long long measure(F f) {
    long long start = nowMs();
    f();
    return nowMs() - start;
}

// ---------------- BST SIMPLE ----------------
struct BST {
    struct Node {
        int key;
        Node* left;
        Node* right;
        Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };
    Node* root = nullptr;

    Node* insertNode(Node* node,int key){
        if(!node) return new Node(key);
        if(key < node->key) node->left = insertNode(node->left,key);
        else if(key > node->key) node->right = insertNode(node->right,key);
        return node;
    }

    Node* minValueNode(Node* node){
        while(node->left) node=node->left;
        return node;
    }

    Node* deleteNode(Node* node,int key){
        if(!node) return nullptr;
        if(key<node->key) node->left = deleteNode(node->left,key);
        else if(key>node->key) node->right = deleteNode(node->right,key);
        else{
            if(!node->left){ Node* r=node->right; delete node; return r; }
            else if(!node->right){ Node* l=node->left; delete node; return l; }
            Node* m = minValueNode(node->right);
            node->key = m->key;
            node->right = deleteNode(node->right,m->key);
        }
        return node;
    }

    bool search(Node* node,int key){
        if(!node) return false;
        if(key==node->key) return true;
        if(key<node->key) return search(node->left,key);
        return search(node->right,key);
    }

    void insert(int key){ root=insertNode(root,key); }
    void erase(int key){ root=deleteNode(root,key); }
    bool contains(int key){ return search(root,key); }
};

// ---------------- RED-BLACK TREE SIMPLE ----------------
// Simplifié pour benchmark (no balancing for simplicity, just to compare)
enum Color {RED, BLACK};
struct RBNode {
    int key;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    RBNode(int k): key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};
struct RBTree {
    RBNode* root = nullptr;

    RBNode* insertNode(RBNode* root,RBNode* node){
        if(!root) return node;
        if(node->key < root->key){
            root->left = insertNode(root->left,node);
            root->left->parent = root;
        } else if(node->key > root->key){
            root->right = insertNode(root->right,node);
            root->right->parent = root;
        }
        return root;
    }

    void insert(int key){
        RBNode* node = new RBNode(key);
        root = insertNode(root,node);
        node->color = BLACK; // simplifié
    }

    bool search(RBNode* node,int key){
        if(!node) return false;
        if(node->key==key) return true;
        if(key<node->key) return search(node->left,key);
        return search(node->right,key);
    }

    bool contains(int key){ return search(root,key); }

    RBNode* minValueNode(RBNode* node){
        while(node->left) node=node->left;
        return node;
    }

    RBNode* deleteNode(RBNode* node,int key){
        if(!node) return nullptr;
        if(key<node->key) node->left = deleteNode(node->left,key);
        else if(key>node->key) node->right = deleteNode(node->right,key);
        else{
            if(!node->left){ RBNode* r=node->right; delete node; return r; }
            else if(!node->right){ RBNode* l=node->left; delete node; return l; }
            RBNode* m = minValueNode(node->right);
            node->key = m->key;
            node->right = deleteNode(node->right,m->key);
        }
        return node;
    }

    void erase(int key){ root = deleteNode(root,key); }
};

// ---------------- STRUCTURE POUR RESULTATS ----------------
struct BenchmarkResult {
    string name;
    string complexity;
    double insertion;
    double search;
    double deletion;
};

// ---------------- MAIN ----------------
int main(){
    const int N = 4000000;
    vector<int> data(N);
    iota(data.begin(),data.end(),0);
    shuffle(data.begin(),data.end(),mt19937(0));

    vector<BenchmarkResult> results;

    cout << "\n=========== BENCHMARK 4,000,000 ELEMENTS ===========\n";

    // unordered_set
    {
        unordered_set<int> us;
        double ins = measure([&](){ for(int v:data) us.insert(v); })/1000.0;
        double sea = measure([&](){ for(int v:data) us.find(v); })/1000.0;
        double del = measure([&](){ for(int v:data) us.erase(v); })/1000.0;

        cout << "\n[unordered_set]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        BenchmarkResult r;
        r.name="unordered_set";
        r.complexity="O(1) amorti";
        r.insertion=ins; r.search=sea; r.deletion=del;
        results.push_back(r);
    }

    // set
    {
        set<int> s;
        double ins = measure([&](){ for(int v:data) s.insert(v); })/1000.0;
        double sea = measure([&](){ for(int v:data) s.find(v); })/1000.0;
        double del = measure([&](){ for(int v:data) s.erase(v); })/1000.0;

        cout << "\n[set STL]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        BenchmarkResult r;
        r.name="set";
        r.complexity="O(log n)";
        r.insertion=ins; r.search=sea; r.deletion=del;
        results.push_back(r);
    }

    // map
    {
        map<int,int> m;
        double ins = measure([&](){ for(int v:data) m[v]=v; })/1000.0;
        double sea = measure([&](){ for(int v:data) m.find(v); })/1000.0;
        double del = measure([&](){ for(int v:data) m.erase(v); })/1000.0;

        cout << "\n[map STL]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        BenchmarkResult r;
        r.name="map";
        r.complexity="O(log n)";
        r.insertion=ins; r.search=sea; r.deletion=del;
        results.push_back(r);
    }

    // BST simple
    {
        BST bst;
        double ins = measure([&](){ for(int v:data) bst.insert(v); })/1000.0;
        double sea = measure([&](){ for(int v:data) bst.contains(v); })/1000.0;
        double del = measure([&](){ for(int v:data) bst.erase(v); })/1000.0;

        cout << "\n[BST simple]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        BenchmarkResult r;
        r.name="BST simple";
        r.complexity="O(n) worst-case";
        r.insertion=ins; r.search=sea; r.deletion=del;
        results.push_back(r);
    }

    // Red-Black Tree maison
    {
        RBTree rbt;
        double ins = measure([&](){ for(int v:data) rbt.insert(v); })/1000.0;
        double sea = measure([&](){ for(int v:data) rbt.contains(v); })/1000.0;
        double del = measure([&](){ for(int v:data) rbt.erase(v); })/1000.0;

        cout << "\n[Red-Black Tree simple]\nInsertion: " << ins << " s, Recherche: " << sea << " s, Suppression: " << del << " s\n";

        BenchmarkResult r;
        r.name="Red-Black Tree";
        r.complexity="O(log n) approximatif";
        r.insertion=ins; r.search=sea; r.deletion=del;
        results.push_back(r);
    }

    // fichier texte
    ofstream fout("benchmark_results.txt");
    fout << "========== BENCHMARK 4,000,000 ELEMENTS ==========\n\n";
    for(auto &r:results){
        fout << r.name << " (" << r.complexity << ")\n";
        fout << "Insertion: " << r.insertion << " s\n";
        fout << "Recherche: " << r.search << " s\n";
        fout << "Suppression: " << r.deletion << " s\n";
        fout << "----------------------------------------\n";
    }
    fout.close();

    cout << "\nBenchmark terminé. Résultats enregistrés dans 'benchmark_results.txt'.\n";

    return 0;
}
