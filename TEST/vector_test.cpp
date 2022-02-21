#include "../MySTL/vector.h"
#include <iostream>
#include <initializer_list>
#include <utility>


using namespace MYSTL;
using std::cout;
using std::endl;


int main() {

    vector<int> vec;
    vec.push_back(3);
    cout << vec.size() << endl;
    for(auto i: vec) {
        cout << i << endl;
    }
    return 0;
}
