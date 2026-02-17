#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <chrono>

class BST {
private:
    struct Node {
        int key;
        Node* left;
        Node* right;
        Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;

    void insert(Node*& node, int key) {
        if (!node)
            node = new Node(key);
        else if (key < node->key)
            insert(node->left, key);
        else if (key > node->key)
            insert(node->right, key);
        else
            return;
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    Node* find_min(Node* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }

    Node* remove(Node* node, int key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = remove(node->left, key);
            return node;
        }
        else if (key > node->key) {
            node->right = remove(node->right, key);
            return node;
        }
        else {
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            }

            if (!node->left) {
                Node* r = node->right;
                delete node;
                return r;
            }

            if (!node->right) {
                Node* l = node->left;
                delete node;
                return l;
            }

            Node* succ = find_min(node->right);
            node->key = succ->key;
            node->right = remove(node->right, succ->key);
            return node;
        }
    }

public:
    ~BST() { destroy(root); }

    void insert(int key) { insert(root, key); }
    void remove(int key) { root = remove(root, key); }
};

void build_balanced_order_like_cs(
    const std::vector<int>& sorted,
    std::vector<int>& out
) {
    std::vector<int> Ls;
    std::vector<int> Rs;
    Ls.push_back(0);
    Rs.push_back((int)sorted.size() - 1);

    int idx = 0;
    while (idx < (int)Ls.size()) {
        int L = Ls[idx];
        int R = Rs[idx];
        idx++;

        if (L > R) continue;

        int mid = (L + R) / 2;
        out.push_back(sorted[mid]);

        Ls.push_back(L);
        Rs.push_back(mid - 1);

        Ls.push_back(mid + 1);
        Rs.push_back(R);
    }
}

int main() {
    const int n = 1023;

    std::vector<int> keys;
    keys.reserve(n);
    for (int i = 1; i <= n; i++) {
        keys.push_back(i);
    }

    std::mt19937 rng(0);

    for (int i = 0; i < n; i++) {
        int j = (int)(rng() % n);
        std::swap(keys[i], keys[j]);
    }

    using Clock = std::chrono::high_resolution_clock;

    std::ofstream csv("results.csv");
    if (!csv) {
        std::cerr << "Cannot open results.csv for writing!\n";
        return 1;
    }

    csv << "Test,Time_ms\n";
    std::cout << "Test,Time_ms\n";

    auto print_and_csv = [&csv](const std::string& name, double ms) {
        std::cout << name << "," << ms << "\n";
        csv << name << "," << ms << "\n";
        };

    {
        BST tree;
        auto start = Clock::now();
        for (int k : keys) tree.insert(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("BST_random_insert", ms);
    }

    std::vector<int> balanced;
    {
        std::vector<int> sorted = keys;
        std::sort(sorted.begin(), sorted.end());

        balanced.clear();
        balanced.reserve(n);
        build_balanced_order_like_cs(sorted, balanced);

        BST tree;
        auto start = Clock::now();
        for (int k : balanced) tree.insert(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("BST_balanced_insert", ms);
    }

    {
        std::set<int> s;
        auto start = Clock::now();
        for (int k : keys) s.insert(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("SortedSet_insert", ms);
    }

    {
        BST tree;
        for (int k : keys) tree.insert(k);

        auto start = Clock::now();
        for (int k : keys) tree.remove(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("BST_random_remove", ms);
    }

    {
        BST tree;
        for (int k : balanced) tree.insert(k);

        auto start = Clock::now();
        for (int k : keys) tree.remove(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("BST_balanced_remove", ms);
    }

    {
        std::set<int> s;
        for (int k : keys) s.insert(k);

        auto start = Clock::now();
        for (int k : keys) s.erase(k);
        auto end = Clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        print_and_csv("SortedSet_remove", ms);
    }

    csv.close();
    std::cout << "CSV file saved as results.csv\n";
    return 0;
}
