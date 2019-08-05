#include "adlist.h"
using std::list;

int main(){
    BaseObject *base1, *base2, *base3;
    adlist::adlistIter begin, end;
    base1 = new StringObject("list2test1");
    base2 = new StringObject("list2test2");
    base3 = new StringObject("list2test3");

    // adlist *list1;
    auto list1 = new adlist();
    
    list1->adlistAddNodeHead(base1);
    list1->adlistAddNodeHead(base2);

    list1->adlistAddNodeTail(base3);
    
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }
    cout << endl;
    adlist::adlistIter two = list1->adlistSearch(base1);
    if(two == list1->adlistGetEnd()) cout << "OK" << endl;
    else cout << *((*two)->get_value()) << endl;
    adlist::adlistIter three = list1->adlistSearch(new StringObject("base3"));
    if(three == list1->adlistGetEnd()) cout << "OK" << endl;
    else cout << *((*three)->get_value()) << endl;
    adlist::adlistIter none = list1->adlistSearch(new StringObject("none"));
    if(none == list1->adlistGetEnd()) cout << "OK" << endl;
    else cout <<"error: " << *((*none)->get_value()) << endl;


    list1->adlistInsertNode(two, new StringObject("base3"), 0);
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }

    cout << endl;
    auto one = list1->adlistGetBegin();
    list1->adlistInsertNode(one, new StringObject("base5"), 1);
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }


    begin = list1->adlistGetBegin();
    
    delete *begin;
    list1->adlistDelNode(begin);
    cout << endl;
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }
    cout << endl;
    while (list1->adlistGetBegin() != list1->adlistGetEnd()){

        begin = list1->adlistGetBegin();
        cout << *((*begin)->get_value()) << endl;
        delete *begin;
        list1->adlistDelNode(begin);
        cout << "del ok" << endl;
    }


    cout << "OK" << endl;
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }
    list1->adlistAddNodeHead(new StringObject("asdf"));
    list1->adlistAddNodeTail(new StringObject("list1test1"));

    for (int i = 0; i < 100; i++) list1->adlistAddNodeHead(new StringObject((long long)i));
    // delete list1;

    cout << list1->length() << endl;
    begin = list1->adlistGetBegin();
    end = list1->adlistGetEnd();
    while (begin != end){
        cout << *((*begin)->get_value()) << endl;
        ++begin;
    }
    createSharedObjects();
    // delete list1;
    cout << endl;
    cout << *((*list1->adlistIndex(101))->get_value()) << endl;
    auto iter = list1->adlistGetEnd();
    iter--;
    cout << *((*iter)->get_value()) << endl;

    
}