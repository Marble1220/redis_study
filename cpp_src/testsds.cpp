#include "sds.h"
#include <iostream>

using namespace std;


int main(){
    // sdshdr *ptr = new sdshdr("addf");
    // cout << *ptr << '\n' << ptr->sdslen() << '\n' << ptr->sdsavail() << endl;
    // ptr->sdsclear();
    // cout << *ptr << '\n' << ptr->sdslen() << '\n' << ptr->sdsavail() << endl;
    // ptr->sdsRemoveFreeSpace();
    // cout << *ptr << '\n' << ptr->sdslen() << '\n' << ptr->sdsavail() << endl;
    // ptr->sdsMakeRoomFor(10);
    // cout << *ptr << '\n' << ptr->sdslen() << '\n' << ptr->sdsavail() << endl;
    // sdshdr *ptr2 = new sdshdr("test2");
    // sdshdr *ptr3 = new sdshdr(*ptr2);
    // sdshdr test4 = *ptr3;
    // cout << *ptr2 << ' ' << *ptr3 << " " << test4 << endl;
    // cout << (void*)ptr2->get_buf() << endl;
    // cout << (void*)ptr3->get_buf() << endl;
    // cout << (void*)test4.get_buf();
    // delete ptr;
    // delete ptr2;
    // delete ptr3;
    // cout << (void*)ptr3->get_buf() << endl;
    // cout << ptr3->get_buf() << endl;

    sdshdr *test1 = new sdshdr("test1");
    sdshdr *test2 = new sdshdr("test2");
    sdshdr *test3 = new sdshdr("test3");
    cout << (void*)(test3->get_buf()) << endl;
    cout << (void*)(test2->get_buf()) << endl;
    cout << test3->get_buf() << ' ' << test3->sdslen() << ' ' << test3->sdsavail() << endl;
    *test3 += *test1;
    cout << test3->get_buf() << ' ' << test3->sdslen() << ' ' << test3->sdsavail() << endl;
    *test3 += "this temp c string";
    cout << test3->get_buf() << ' ' << test3->sdslen() << ' ' << test3->sdsavail() << endl;

    *test3 += *test3;
    cout << test3->get_buf() << ' ' << test3->sdslen() << ' ' << test3->sdsavail() << endl;

    *test3 += *test3;
    cout << test3->get_buf() << ' ' << test3->sdslen() << ' ' << test3->sdsavail() << endl;
    
    cout << '\n' << endl;


    sdshdr* test4 = new sdshdr("test4");
    sdshdr* test5 = new sdshdr("test5");

    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;    
    cout << (void*)(test4->get_buf()) << endl;
    *test4 += *test5;
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;
    cout << (void*)(test4->get_buf()) << endl;
    *test4 += *test5;
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;
    cout << (void*)(test4->get_buf()) << endl;

    cout << '\n' << endl;

    test4 = new sdshdr("test4");
    test5 = new sdshdr("test5");
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;    
    cout << (void*)(test4->get_buf()) << endl;
    // test4 = testadd(test4, test5);
    
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;
    cout << (void*)(test4->get_buf()) << endl;
    test4 = testadd(test4, test5);
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;
    test4 = *test5 + "addd";

    
    

    cout << (void*)(test4->get_buf()) << endl;
    cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;

    long long i = 21233;
    *test4 = i;
    // cout << test4->get_buf() << ' ' << test4->sdslen() << ' '<< test4->sdsavail() << endl;
    test4->sds_info();
    auto test6 = new sdshdr(12345);
    test6->sds_info();
    sdshdr test7;
    test7 = *test6;
    test7.sds_info();

    auto test_array = new sdshdr[10];
    for (int i = 0; i < 10; i++){
        test_array[i] = test7;
    }

    for (int i = 0; i < 10; i++){
        test_array[i].sds_info();
    }

    auto test8 = new sdshdr("1");
    auto test9 = new sdshdr("1");

    if (*test8 == *test9){
        cout << "OK" << endl;
    }
    if (sdsmatch(test8, test9)){
        cout << "  OK   " << endl;
    }
    
    *test9 += "lll";
    if (!(*test8 == *test9)){
        cout << "OK" << endl;
    }

    int cot = 0;
    auto test10 = sdssplitlen("ababababababababababababababab",strlen("ababababababababababababababab"), "b", strlen("b"), &cot);
    cout << cot << endl;
    for (int i = 0; i < cot; i++){
        cout << test10[i] << endl;
    }

    cout << test9->length() << endl;
    delete test9;
    cout << test9->struct_type << endl;


}