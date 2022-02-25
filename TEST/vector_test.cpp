#include "../MySTL/vector.h"
#include <iostream>
#include <initializer_list>
#include <utility>


using namespace MYSTL;
using std::cout;
using std::endl;
using std::ostream;

template <typename T>
void printVec(vector<T> vec) {
    for(auto i: vec) {
        cout << i << endl;
    }
}

int main() {

    vector<int> vec2{2, 4, 5};

    vector<int> vec3(10, 3);
    vector<int> vec4(std::move(vec2));
    //printVec(vec3);
    //const vector<int> con_vec1 = vec3;
    //const vector<int> con_vec2(10, 3);    

    // cout << "vec2:" << endl;
    // printVec(vec2);

    // cout << "vec1: " << endl;
    // printVec(vec1);
    cout << "vec4:" << endl;
    printVec(vec4);

    vec4.emplace_back(4);

    vector<int> vec1;
    vec1.push_back(3);
    for(int i = 0; i < 10; i ++)
        vec1.push_back(i * 3);
    cout << "vec1" << endl;
    printVec(vec1);

    cout << "vec4" << endl;
    printVec(vec4);

    cout << "vec1.size() = " << vec1.size() << endl;
    cout << "vec1.capacaty() = " << vec1.capacity() << endl;
    vec1.shrink_to_fit();
    cout << "vec1.size() = " << vec1.size() << endl;
    cout << "vec1.capacaty() = " << vec1.capacity() << endl;
    vec4.assign(vec1.begin(), vec1.end());
    cout << "vec4" << endl;
    printVec(vec4);
    cout << "vec4.size() = " << vec4.size() << endl;
    cout << "vec4.capacaty() = " << vec4.capacity() << endl;
    vec4.push_back(3);
    cout << vec4.capacity() << endl;


    /*********************const vector test*****************************/
    const vector<int> cvec1 = {3, 4, 5, 6};


    return 0;
}
