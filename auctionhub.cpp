#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace std::chrono;

struct Bid {
    string bidderId;
    double amount;       // bid price
    long long timestamp; // Unix ms
    int auctionId;
};

// ---------- Utilities ----------
void printBids(const vector<Bid>& v, size_t limit = 10) {
    for (size_t i = 0; i < min(v.size(), limit); ++i)
        cout << "[" << v[i].bidderId << "] $" << v[i].amount
             << " @ " << v[i].timestamp << "\n";
    if (v.size() > limit) cout << "... (" << v.size() << " total)\n";
}

bool loadBids(const string& path, vector<Bid>& out) {
    ifstream f(path);
    if (!f) return false;
    Bid b;
    while (f >> b.bidderId >> b.amount >> b.timestamp >> b.auctionId)
        out.push_back(b);
    return true;
}

// Thứ tự tăng dần: theo amount, nếu bằng nhau thì theo timestamp
bool lessAsc(const Bid& a, const Bid& b) {
    if (a.amount != b.amount) return a.amount < b.amount;
    return a.timestamp < b.timestamp;
}

bool isSorted(const vector<Bid>& v) {
    for (size_t i = 1; i < v.size(); ++i)
        if (lessAsc(v[i], v[i - 1])) return false;
    return true;
}

// ---------- Task A: Insertion Sort ----------
// Lấy key = v[i], dịch các phần tử lớn hơn sang phải, chèn key vào chỗ trống.
// Dùng int j để j có thể xuống -1 mà không bị underflow.
void insertionSort(vector<Bid>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        Bid key = v[i];
        int j = (int)i - 1;
        while (j >= 0 && lessAsc(key, v[j])) {
            v[j + 1] = v[j];
            --j;
        }
        v[j + 1] = key;
    }
}

// ---------- Task B: Selection Sort (giảm dần, in-place) ----------
// Mỗi lượt tìm CHỈ SỐ của phần tử lớn nhất trong đoạn chưa sắp xếp rồi swap 1 lần.
void selectionSort(vector<Bid>& v) {
    int n = (int)v.size();
    for (int i = 0; i < n - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < n; ++j)
            if (lessAsc(v[best], v[j])) best = j; // v[j] lớn hơn -> chọn
        if (best != i) swap(v[i], v[best]);
    }
}

// ---------- Task C: Interchange Sort ----------
// Cố định i, gặp v[j] nhỏ hơn v[i] thì swap ngay (khác Selection Sort).
long long g_swapCount = 0;

void interchangeSort(vector<Bid>& v) {
    g_swapCount = 0;
    int n = (int)v.size();
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (lessAsc(v[j], v[i])) {
                swap(v[i], v[j]);
                ++g_swapCount;
            }
}

// ---------- Task D: Bubble Sort early stop ----------
// stable chỉ được quyết định ở LƯỢT ĐẦU; swapped chỉ dùng để thoát sớm.
bool bubbleSortEarlyStop(vector<Bid>& v) {
    int n = (int)v.size();
    bool stable = true;
    for (int pass = 0; pass < n - 1; ++pass) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - pass; ++j)
            if (lessAsc(v[j + 1], v[j])) {
                swap(v[j], v[j + 1]);
                swapped = true;
            }
        if (pass == 0 && swapped) stable = false; // lượt đầu có swap -> UNSTABLE
        if (!swapped) break;                      // early stop
    }
    cout << (stable ? "STABLE" : "UNSTABLE") << "\n";
    return stable;
}

// ---------- Task E: Quick Sort (median-of-three + Lomuto) ----------
void quickSort(vector<Bid>& v, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2; // tránh tràn số so với (lo+hi)/2

    // Sắp xếp 3 phần tử lo, mid, hi -> trung vị nằm ở mid
    if (lessAsc(v[mid], v[lo]))  swap(v[mid], v[lo]);
    if (lessAsc(v[hi],  v[lo]))  swap(v[hi],  v[lo]);
    if (lessAsc(v[hi],  v[mid])) swap(v[hi],  v[mid]);

    swap(v[mid], v[hi]); // đưa pivot (trung vị) về cuối cho Lomuto
    Bid pivot = v[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j)
        if (lessAsc(v[j], pivot)) swap(v[++i], v[j]);
    swap(v[i + 1], v[hi]);
    int p = i + 1;

    quickSort(v, lo, p - 1);
    quickSort(v, p + 1, hi);
}

void quickSortWrapper(vector<Bid>& v) {
    if (v.empty()) return; // tránh v.size()-1 bị underflow
    quickSort(v, 0, (int)v.size() - 1);
}

// ---------- Driver ----------
static void check(const string& name, bool ok, int& fails) {
    cout << (ok ? "[PASS] " : "[FAIL] ") << name << "\n";
    if (!ok) ++fails;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv; // chua dung tham so dong lenh
    string smallPath = "test_data/small.txt";
    string largePath = "test_data/large.txt";
    int fails = 0;

    vector<Bid> small;
    if (!loadBids(smallPath, small)) { cout << "Khong mo duoc " << smallPath << "\n"; return 1; }

    cout << "=== Task A: Insertion Sort ===\n";
    vector<Bid> a = small; insertionSort(a); printBids(a);
    check("A: ket qua dung (alice, carol, bob, dave)",
          a.size() == 4 && a[0].bidderId == "alice" && a[1].bidderId == "carol" &&
          a[2].bidderId == "bob" && a[3].bidderId == "dave", fails);

    cout << "\n=== Task B: Selection Sort (Top 3) ===\n";
    vector<Bid> b = small; selectionSort(b); printBids(b, 3);
    check("B: Top 3 = dave, bob, carol",
          b.size() >= 3 && b[0].bidderId == "dave" && b[1].bidderId == "bob" &&
          b[2].bidderId == "carol", fails);

    cout << "\n=== Task C: Interchange Sort ===\n";
    vector<Bid> c = small; interchangeSort(c);
    cout << "Total swaps = " << g_swapCount << "\n";
    check("C: sorted tang dan", isSorted(c), fails);

    cout << "\n=== Task D: Bubble Sort early stop ===\n";
    vector<Bid> sortedIn = a;                                  // da sap xep
    vector<Bid> revIn(a.rbegin(), a.rend());                   // dao nguoc
    check("D1: sorted -> true", bubbleSortEarlyStop(sortedIn) == true, fails);
    check("D2: reversed -> false", bubbleSortEarlyStop(revIn) == false, fails);
    check("D2: reversed sau khi sort thi sorted", isSorted(revIn), fails);

    cout << "\n=== Edge cases ===\n";
    {
        vector<Bid> e;
        insertionSort(e); selectionSort(e); interchangeSort(e); quickSortWrapper(e);
        check("rong: khong crash", true, fails);
        vector<Bid> one = {{"solo", 5.0, 1, 1}};
        insertionSort(one); selectionSort(one); interchangeSort(one); quickSortWrapper(one);
        check("1 phan tu: khong crash", one.size() == 1, fails);

        // tie-break theo timestamp: cung amount, timestamp khac nhau
        vector<Bid> t = {{"x", 10.0, 30, 1}, {"y", 10.0, 10, 1}, {"z", 10.0, 20, 1}};
        vector<Bid> t1 = t, t2 = t, t3 = t, t4 = t;
        insertionSort(t1); interchangeSort(t2); quickSortWrapper(t3); bubbleSortEarlyStop(t4);
        auto okTie = [](const vector<Bid>& r) {
            return r[0].bidderId == "y" && r[1].bidderId == "z" && r[2].bidderId == "x";
        };
        check("tie-break insertion", okTie(t1), fails);
        check("tie-break interchange", okTie(t2), fails);
        check("tie-break quick", okTie(t3), fails);
        check("tie-break bubble", okTie(t4), fails);
    }

    cout << "\n=== Task E: Quick Sort 100k ===\n";
    vector<Bid> big;
    if (!loadBids(largePath, big)) { cout << "Khong mo duoc " << largePath << "\n"; return 1; }
    cout << "Loaded " << big.size() << " bids\n";
    auto t0 = steady_clock::now();
    quickSortWrapper(big);
    auto t1 = steady_clock::now();
    long long ms = duration_cast<milliseconds>(t1 - t0).count();
    cout << "QuickSort runtime: " << ms << " ms\n";
    check("E: 100k sorted dung", big.size() == 100000 && isSorted(big), fails);

    // Bonus: quick sort tren input da sap xep / dao nguoc (median-of-three)
    {
        vector<Bid> asc = big;
        vector<Bid> desc(big.rbegin(), big.rend());
        auto s0 = steady_clock::now(); quickSortWrapper(asc);  auto s1 = steady_clock::now();
        auto d0 = steady_clock::now(); quickSortWrapper(desc); auto d1 = steady_clock::now();
        cout << "QuickSort tren input da sap xep: " << duration_cast<milliseconds>(s1 - s0).count() << " ms\n";
        cout << "QuickSort tren input dao nguoc:  " << duration_cast<milliseconds>(d1 - d0).count() << " ms\n";
        check("E: sorted/reversed input OK", isSorted(asc) && isSorted(desc), fails);
    }

    cout << "\n" << (fails == 0 ? "TAT CA TEST DEU PASS" : "CO TEST FAIL") << "\n";
    return fails == 0 ? 0 : 1;
}
