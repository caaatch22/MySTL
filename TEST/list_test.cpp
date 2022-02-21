#include "../MySTL/list.h"
#include <iostream>
#include <initializer_list>
#include <utility>


using namespace MYSTL;
using std::cout;
using std::endl;


int main() {


    list<int> ilist1, ilist2, ilist3;
    list<int> tmp(ilist3);
    for (int i = 0; i < 10; i ++)
        ilist1.push_back(i * 4);
    
    tmp = ilist1;
    list<int> a(tmp.begin(), tmp.end());
    cout << a.size() << endl;
    
    for(auto i:a)
        cout << i << endl;

    a = {3, 4, 5};
    cout << a.size() << endl;
    list<double> b{2.3, 3.3, 2};
    b.assign({3, 3.3, 44, 4.44, 44, 3.3});

    for(auto i:b)
        cout << i << endl;

    return 0;
}
