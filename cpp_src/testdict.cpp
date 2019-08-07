#include "dict.h"

int main(){
    // dict *dict1;
    // dict1 = new dict();
    // cout << dict1->length() << endl;
    // // StringObject *str1 = new StringObject(10);
    // // StringObject *str2 = new StringObject(10);

    // // if (str1->match(str2) == 0) cout << "ok" << endl;
    // for (int i = 0; i < 15; i++){
    //     // cout << "StringObject num: " << i << endl;
    //     dict1->dictAdd(new StringObject(i), new StringObject(i));
    // }
    // // dict1->dictAddRaw(str1);
    // cout << dict1->length() << endl;

    // auto node1 = dict1->dictFind(new StringObject(10));
    // cout << *(node1->key->get_value()) << ' ' << *(node1->v->get_value()) << endl;
    // auto node2 = dict1->dictFind(new StringObject(20));
    // if (node2 == nullptr) cout << "ok" << endl;
    // else cout << "error find" << endl;

    // auto val1 = dict1->dictFetchValue(new StringObject(14));
    // cout << *(val1->get_value()) << endl;
    // auto val2 = dict1->dictFetchValue(new StringObject(-10));
    // if (val2 == nullptr) cout << "ok" << endl;
    // else cout << "error fetchvalue" << endl;

    // node1 = dict1->dictFind(new StringObject("0"));

    // int i = 1;
    // while(node1 != nullptr){    
    //     cout << *(node1->v->get_value()) << endl;
    //     node1 = dict1->dictFind(new StringObject(i++));
    // }

    // i = 1;
    // val1 = dict1->dictFetchValue(new StringObject("0"));
    // while (val1 != nullptr){
    //     cout << *(val1->get_value()) << endl;
    //     val1 = dict1->dictFetchValue(new StringObject(i++));
    // }

    
    // cout << dict1->length() << endl;
    // dict1->dictEmpty(nullptr);

    // if (dict1->ht[0].table == nullptr) cout << "ok" << endl;


    // for (i = 0; i < 15; i++){
    //     // cout << "StringObject num: " << i << endl;
    //     dict1->dictAdd(new StringObject(i), new StringObject(i));
    // }







    // auto dict2 = new dict();
    // for (int j = 0; j < 15; j++){
    //     cout << "StringObject num: " << j << endl;
    //     dict2->dictAdd(new StringObject(j), new StringObject(j));
    // }

    // cout << dict2->dictFind(new StringObject(15)) << endl;

    // cout << dict2->dictReplace(new StringObject(14), new StringObject(16)) << endl;
    // cout << *(dict2->dictFetchValue(new StringObject(14))->get_value()) << endl;
    // cout << dict2->dictReplace(new StringObject(17), new StringObject(16)) << endl;
    // cout << *(dict2->dictFetchValue(new StringObject(17))->get_value()) << endl;

    // cout << endl;

    // auto node3 = dict2->dictReplaceRaw(new StringObject(3));
    // if (node3->v != nullptr) cout << *(node3->v->get_value()) << endl;
    // else cout << "node3 is new node" << endl;

    // node3 = dict2->dictReplaceRaw(new StringObject(20));
    // if (node3->v != nullptr) cout << *(node3->v->get_value()) << endl;
    // else cout << "node3 is new node" << endl;

    // cout << endl;
    
    // cout << dict2->dictDelete(new StringObject(20)) << endl;
    // val1 = dict2->dictFetchValue(new StringObject(20)) ;
    // if (val1 == nullptr) cout << "delete ok" << endl;
    // dict2->dictEmpty(nullptr);

    // for (int j = 0; j < 15; j++){
    //     dict2->dictAdd(new StringObject(j), new StringObject(j));
    // }

    // auto iter1 = dict2->dictGetIterator();
    // node1 = iter1->dictNext();
    // while (node1){
    //     cout << *(node1->key->get_value()) << ' ' << *(node1->v->get_value()) << endl;
    //     node1 = iter1->dictNext();
    // }

}