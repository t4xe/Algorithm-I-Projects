# C++ Algorithm & Data Structures Analysis

A collection of university assignments focused on the implementation, benchmarking, and optimization of fundamental algorithms and data structures. This project covers sorting performance, data matching techniques, and the efficiency of Binary Search Trees.

## 📌 Project Overview

This repository contains two primary assignments developed in C++[cite: 3, 5]:
1. **Sorting & Data Matching:** Comparing $O(n^2)$, $O(n \log n)$, and linear-time sorting algorithms using both synthetic and real-world data formats[cite: 3, 4].
2. **Binary Search Trees (BST):** Analyzing the performance impact of insertion order (Random vs. Balanced) and comparing custom BST implementations against STL containers (`std::set`)[cite: 5, 6].

## 📂 Contents

### 1. Sorting Algorithm Comparison (Assignment 1, Task 1A)
**File:** `Assignment1Task1PartA.cpp`
* **Algorithms:** Implements Bubble Sort, Insertion Sort, Merge Sort, and Quick Sort[cite: 3].
* **Functionality:** Generates random datasets of varying sizes (up to 10,000 elements) and measures execution time using the `<chrono>` library[cite: 3].
* **Output:** Generates a `sorting_results.csv` file containing average measurements across multiple trials[cite: 2, 3].

### 2. Linear vs. Log-Linear Sorting (Assignment 1, Task 1B)
**File:** `Assignment1Task1PartB.cpp`
* **Functionality:** Processes two CSV "card dumps" to match masked credit card numbers with their suffixes and metadata[cite: 4].
* **Algorithms:** Compares a log-linear approach ($O(n \log n)$) against a custom linear solution (Counting/Bucket Sort)[cite: 4].
* **Important Note:** Requires the user to provide absolute file paths for `carddump1.csv` and `carddump2.csv` at lines 229 and 230.
* **Output:** Saves matched data into `matched_l.csv` and `matched_b.csv`, along with execution times in `times.csv`[cite: 2, 4].

### 3. BST Performance & Balancing (Assignment 2, Task 1 & 2)
**File:** `Assignment2Task1&2.cpp`
* **Functionality:** Implements a custom Binary Search Tree and calculates timing for various operations[cite: 5].
* **Analysis:** Compares insertion and removal times for keys in random order versus a "best-case" balanced order[cite: 5, 6].
* **Comparison:** Includes benchmarking against the C++ Standard Library `std::set` (typically a Red-Black Tree)[cite: 5, 6].
* **Output:** Results are saved to `results.csv`[cite: 2, 5].

## 🛠️ Setup & Execution

### Prerequisites
* A C++ compiler supporting C++17 or higher (e.g., GCC, Clang, or MSVC).
* (For Part 1B) Input files: `carddump1.csv` and `carddump2.csv`.

### Compilation
Compile the files individually using a standard C++ compiler:
```bash
g++ -std=c++17 Assignment1Task1PartA.cpp -o Task1A
g++ -std=c++17 Assignment1Task1PartB.cpp -o Task1B

g++ -std=c++17 Assignment2Task1&2.cpp -o Task2
