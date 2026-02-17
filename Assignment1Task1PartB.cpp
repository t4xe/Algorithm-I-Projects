#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

struct CardA {
    std::string maskedNumber;
};

struct CardB {
    std::string suffixNumber;
    std::string expText;
    std::string cvvText;
    std::string pinText;
    std::string netText;

    int expIndex = 0;
    int pinValue = 0;
};

static std::vector<std::string> splitByComma(const std::string& line) {
    std::vector<std::string> cols;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) cols.push_back(item);
    return cols;
}

static int monthToInt(const std::string& mmm) {
    static const std::unordered_map<std::string, int> m = {
        {"Jan",1},{"Feb",2},{"Mar",3},{"Apr",4},{"May",5},{"Jun",6},
        {"Jul",7},{"Aug",8},{"Sep",9},{"Oct",10},{"Nov",11},{"Dec",12}
    };
    auto it = m.find(mmm);
    if (it == m.end()) return 0;
    return it->second;
}

static bool parseMMMYY(const std::string& s, int& year, int& month) {
    if (s.size() < 6) return false;
    if (s[3] != '-') return false;

    std::string mmm = s.substr(0, 3);
    month = monthToInt(mmm);
    if (month == 0) return false;

    char c0 = s[4], c1 = s[5];
    if (c0 < '0' || c0 > '9' || c1 < '0' || c1 > '9') return false;

    int yy = (c0 - '0') * 10 + (c1 - '0');
    year = 2000 + yy;
    return true;
}

static std::vector<CardA> loadDumpA(const fs::path& p) {
    std::ifstream in(p);
    if (!in) throw std::runtime_error("Cannot open: " + p.string());

    std::vector<CardA> rows;
    std::string line;

    if (!std::getline(in, line)) return rows;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto cols = splitByComma(line);
        if (cols.empty()) continue;

        CardA r;
        r.maskedNumber = cols[0];
        rows.push_back(std::move(r));
    }
    return rows;
}

static std::vector<CardB> loadDumpB(const fs::path& p) {
    std::ifstream in(p);
    if (!in) throw std::runtime_error("Cannot open: " + p.string());

    std::vector<CardB> rows;
    std::string line;

    if (!std::getline(in, line)) return rows;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto cols = splitByComma(line);
        if (cols.size() < 5) continue;

        CardB r;
        r.suffixNumber = cols[0];
        r.expText = cols[1];
        r.cvvText = cols[2];
        r.pinText = cols[3];
        r.netText = cols[4];

        int y = 0, m = 0;
        if (parseMMMYY(r.expText, y, m)) r.expIndex = y * 12 + m;
        else r.expIndex = 0;

        try { r.pinValue = std::stoi(r.pinText); }
        catch (...) { r.pinValue = 0; }

        rows.push_back(std::move(r));
    }
    return rows;
}

static std::string mergeCardNumber(const std::string& a, const std::string& b) {
    std::string last4 = "????";
    if (b.size() >= 4) last4 = b.substr(b.size() - 4);

    if (a.size() >= 4 && a.rfind("****") == a.size() - 4) {
        return a.substr(0, a.size() - 4) + last4;
    }

    auto pos = a.find_last_of('-');
    if (pos != std::string::npos) {
        return a.substr(0, pos + 1) + last4;
    }

    return a + last4;
}

static bool cmpByExpThenPin(const CardB& x, const CardB& y) {
    if (x.expIndex != y.expIndex) return x.expIndex < y.expIndex;
    return x.pinValue < y.pinValue;
}

static std::vector<CardB> stableCountByPin(const std::vector<CardB>& v) {
    const int K = 10000;
    std::vector<int> cnt(K, 0);

    for (const auto& x : v) {
        int p = x.pinValue;
        if (p < 0) p = 0;
        if (p >= K) p = K - 1;
        cnt[p]++;
    }

    for (int i = 1; i < K; i++) cnt[i] += cnt[i - 1];

    std::vector<CardB> out(v.size());

    for (int i = (int)v.size() - 1; i >= 0; i--) {
        int p = v[i].pinValue;
        if (p < 0) p = 0;
        if (p >= K) p = K - 1;
        int idx = --cnt[p];
        out[idx] = v[i];
    }

    return out;
}

static std::vector<CardB> stableCountByExp(const std::vector<CardB>& v) {
    if (v.empty()) return v;

    int mn = v[0].expIndex, mx = v[0].expIndex;
    for (const auto& x : v) {
        if (x.expIndex < mn) mn = x.expIndex;
        if (x.expIndex > mx) mx = x.expIndex;
    }

    int range = mx - mn + 1;
    if (range <= 0) return v;

    std::vector<int> cnt((size_t)range, 0);

    for (const auto& x : v) cnt[x.expIndex - mn]++;

    for (int i = 1; i < range; i++) cnt[i] += cnt[i - 1];

    std::vector<CardB> out(v.size());

    for (int i = (int)v.size() - 1; i >= 0; i--) {
        int e = v[i].expIndex - mn;
        int idx = --cnt[e];
        out[idx] = v[i];
    }

    return out;
}

static std::vector<CardB> linearSortExpPin(const std::vector<CardB>& v) {
    auto s1 = stableCountByPin(v);
    auto s2 = stableCountByExp(s1);
    return s2;
}

static void saveMatched(const fs::path& outPath,
    const std::vector<CardA>& a,
    const std::vector<CardB>& b) {
    std::ofstream out(outPath);
    if (!out) throw std::runtime_error("Cannot write: " + outPath.string());

    out << "Credit Card Number,Expiry Date,Verification Code,PIN,Issueing Network\n";
    size_t n = std::min(a.size(), b.size());

    for (size_t i = 0; i < n; i++) {
        std::string full = mergeCardNumber(a[i].maskedNumber, b[i].suffixNumber);
        out << full << ","
            << b[i].expText << ","
            << b[i].cvvText << ","
            << b[i].pinText << ","
            << b[i].netText << "\n";
    }
}

static void saveTimes(const fs::path& outPath, double tSort, double tLinear) {
    std::ofstream out(outPath);
    if (!out) throw std::runtime_error("Cannot write: " + outPath.string());

    out << "Algorithm,Time_ms\n";
    out << "nlogn," << tSort << "\n";
    out << "Bucket," << tLinear << "\n";
}

int main() {
    try {
        // hey professor, please put your absolute path here, this is mine:
        fs::path pA = "C:/Users/Eren/source/repos/AlgorithmAssignment1PartBSolution/AlgorithmAssignment1PartB/carddump1.csv";
        fs::path pB = "C:/Users/Eren/source/repos/AlgorithmAssignment1PartBSolution/AlgorithmAssignment1PartB/carddump2.csv";

        auto dumpA = loadDumpA(pA);
        auto dumpB = loadDumpB(pB);

        std::vector<CardB> vSort = dumpB;
        std::vector<CardB> vLinear = dumpB;

        auto t1 = std::chrono::high_resolution_clock::now();
        std::sort(vSort.begin(), vSort.end(), cmpByExpThenPin);
        auto t2 = std::chrono::high_resolution_clock::now();
        double msSort = std::chrono::duration<double, std::milli>(t2 - t1).count();

        auto t3 = std::chrono::high_resolution_clock::now();
        vLinear = linearSortExpPin(vLinear);
        auto t4 = std::chrono::high_resolution_clock::now();
        double msLinear = std::chrono::duration<double, std::milli>(t4 - t3).count();

        fs::path dir = pA.parent_path();
        if (dir.empty()) dir = ".";

        fs::path out1 = dir / "matched_l.csv";
        fs::path out2 = dir / "matched_b.csv";
        fs::path outT = dir / "times.csv";

        saveMatched(out1, dumpA, vSort);
        saveMatched(out2, dumpA, vLinear);
        saveTimes(outT, msSort, msLinear);

        std::cout << "Times saved to: " << outT.string() << "\n";
        std::cout << "Matching results are saved. " << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

