#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;


void bubbleSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1])
                swap(a[j], a[j + 1]);
}

void insertionSort(vector<int>& a) {
    int n = a.size();
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

void merge(vector<int>& a, int l, int m, int r) {
    vector<int> L(a.begin() + l, a.begin() + m + 1);
    vector<int> R(a.begin() + m + 1, a.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < L.size() && j < R.size())
        a[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];

    while (i < L.size()) a[k++] = L[i++];
    while (j < R.size()) a[k++] = R[j++];
}

void mergeSortRec(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSortRec(a, l, m);
    mergeSortRec(a, m + 1, r);
    merge(a, l, m, r);
}

void mergeSort(vector<int>& a) {
    mergeSortRec(a, 0, a.size() - 1);
}

int partition(vector<int>& a, int low, int high) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (a[j] <= pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[high]);
    return i + 1;
}

void quickSortRec(vector<int>& a, int low, int high) {
    if (low < high) {
        int pi = partition(a, low, high);
        quickSortRec(a, low, pi - 1);
        quickSortRec(a, pi + 1, high);
    }
}

void quickSort(vector<int>& a) {
    quickSortRec(a, 0, a.size() - 1);
}

vector<int> generateRandom(int n) {
    static mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

template<typename Func>
long long timeSort(Func f, vector<int> data) {
    auto start = chrono::high_resolution_clock::now();
    f(data);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

int main() {
    ofstream out("sorting_results.csv");
    out << "Size,Bubble,Insertion,Merge,Quick\n";

    vector<int> sizes = { 5, 10, 20, 30, 50, 100, 500, 1000, 5000, 10000 };
    int runs = 5;

    for (int n : sizes) {
        long long tb = 0, ti = 0, tm = 0, tq = 0;

        for (int i = 0; i < runs; i++) {
            auto data = generateRandom(n);
            tb += timeSort(bubbleSort, data);
            ti += timeSort(insertionSort, data);
            tm += timeSort(mergeSort, data);
            tq += timeSort(quickSort, data);
        }

        tb /= runs; ti /= runs; tm /= runs; tq /= runs;

        cout << "n=" << n
            << " Bubble=" << tb
            << " Insertion=" << ti
            << " Merge=" << tm
            << " Quick=" << tq << endl;

        out << n << "," << tb << "," << ti << "," << tm << "," << tq << "\n";
    }

    out.close();
    cout << "Results saved to sorting_results.csv\n";
    return 0;
}