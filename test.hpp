#ifndef ATCODERC_TEST_H
#define ATCODERC_TEST_H
#include <set>
using namespace std;

inline void func_test() {
    set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(1);
    cout << s.size() << endl;
}
#endif //ATCODERC_TEST_H
