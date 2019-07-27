#include <iostream>

#include <math.h>

#include "intset.h"



using namespace std;

int main(){

    int16_t p[] = {1, 2, 3, 5, 5, 1};
    cout << p << endl;

    ArrayBase *test1 = new ArrayInt<int16_t>(p, 6);

    cout << test1->operator[](1) << endl;

    vector<int16_t> vec{1, 2, 3, 4, 5, 6, 5};
    int64_t i = 4;
    auto ptr = lower_bound(vec.begin(), vec.end(), i);
    cout << ptr - vec.begin() << endl;


    test1->test();
    int pos;
    test1->search(3, &pos);
    cout << pos << endl;
    test1->search(-1, &pos);
    cout << pos << endl;
    test1->search(100, &pos);
    cout << pos << endl;
    test1->search(4, &pos);
    cout << pos << endl;

    test1->add(122);
    test1->test();
    test1->remove(1);
    test1->test();
    test1->add(1);
    test1->test();

    ArrayBase *test2 = new ArrayInt<int64_t>();
    test2 -> test();
    for(int j = -10; j < 10; j++){
        test2->add(j);
    }

    test2->test();
    test2->resize(100);
    test2->test();
    test2->resize(50);
    test2->test();
    test2->resize(10);
    test2->test();
    test2->set(0, 100);
    test2->test();
    cout << endl;
    
    intset* test3 = new intset();
    int success;
    for(int i = 0; i < 20; i++){
        test3->intsetAdd(i, &success);
    }
    test3->test();
    test3->intsetUpgrade(INTSET_ENC_INT32);
    test3->test();
    test3->intsetAdd(1125899906842624, &success);
    test3->test();
    test3->intsetAdd(-1222222222, &success);
    test3->test();
    test3->intsetResize(400);
    test3->test();
    cout << test3->intsetFind(0);
    test3->test();
    test3->intsetSearch(0, &pos);
    test3->test();
    cout << pos << endl;
    test3->intsetSearch(-2, &pos);
    test3->test();
    cout << pos << endl;
    test3->intsetAdd(-5, &success);
    test3->test();
    test3->intsetRemove(123, &success);
    cout << success << endl;
    test3->intsetRemove(10, &success);
    cout << success << endl;
    test3->test();

    base_struct* test4 = test3;

    cout << test4->length() << endl;


    intset test5 = intset();
    intset test6 = intset();
    
    if (intsetmatch(&test5, &test6)){
        cout << "ok" << endl;
    }
    test5.intsetAdd(1, &success);
    test6.intsetAdd(2, &success);

    if (intsetmatch(&test5, &test6)){
        cout << "ok" << endl;
    }

    test6.intsetSet(0, 1);
    if (intsetmatch(&test5, &test6)){
        cout << "ok" << endl;
    }




    
}