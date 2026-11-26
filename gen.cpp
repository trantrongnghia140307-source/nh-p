#include <fstream>
#include <random>
using namespace std;

int main() {
    mt19937 rng(42);                                   // seed co dinh de tai lap ket qua
    uniform_real_distribution<double> price(1.0, 10000.0);
    uniform_int_distribution<long long> ts(1700000000000LL, 1700086400000LL);
    uniform_int_distribution<int> auc(1, 100);
    ofstream f("test_data/large.txt");
    for (int i = 0; i < 100000; ++i)
        f << "user" << i << " " << price(rng) << " " << ts(rng) << " " << auc(rng) << "\n";
}
