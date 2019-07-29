#include "skiplist.h"


int main(){
    skiplist *sl_test1 = new skiplist();
    for (int i = 0; i < 20; i++){
        sl_test1->slInsert(i, new TestObject(i));
    }
    skiplistNode *tmp;
    int j = 1;
    while (1){
        tmp = sl_test1->slGetElementByRank(j);
        if (tmp == nullptr) break;
        
        cout << tmp->score << " " <<*(tmp->obj->get_value()) << endl;
        j++;
    }

    sl_test1->slInsert(6.5, new TestObject("6.5"));
    
    j = 1;
    while (1){
        tmp = sl_test1->slGetElementByRank(j);
        if (tmp == nullptr) break;
        
        cout << tmp->score << " " <<*(tmp->obj->get_value()) << endl;
        j++;
    }
    cout << sl_test1->length() << endl << endl;

    cout << *(sl_test1->tail->obj->get_value()) << endl << endl; 
    tmp = sl_test1->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->level[0].span << endl;

        tmp = tmp->level[0].forward;
    }
    cout << endl;

    sl_test1->slDelete(14, new TestObject(14));
    tmp = sl_test1->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->level[0].span << endl;

        tmp = tmp->level[0].forward;
    }
    cout << endl;
    sl_test1->slDelete(19, new TestObject(19));
    tmp = sl_test1->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->level[0].span << endl;

        tmp = tmp->level[0].forward;
    }

    cout << endl;
    sl_test1->slDelete(0, new TestObject("0"));
    tmp = sl_test1->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->level[0].span << endl;

        tmp = tmp->level[0].forward;
    }
    cout << endl;


    tmp = sl_test1->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->score << endl;

        tmp = tmp->level[0].forward;
    }
    cout << endl;

    cout << sl_test1->slGetRank(6.5, new TestObject("6.5"));


    delete sl_test1;
    cout << endl << endl;

    skiplist *sl_test2 = new skiplist();
    for (int i = 1; i < 10; i++){
        sl_test2->slInsert(i, new TestObject(i));
    }

    tmp = sl_test2->header->level[0].forward;
    while (tmp != nullptr){
        cout << *(tmp->obj->get_value()) << " " << tmp->score << endl;
        tmp = tmp->level[0].forward;
    }

    cout << endl;
    rangespec rangetest = {3, 8, 0, 0};
    int k = sl_test2->slIsInRange(&rangetest);
    cout << k << endl;
    tmp = sl_test2->slLastInRange(&rangetest);
    cout << tmp->score << endl;
    tmp = sl_test2->slFirstInRange(&rangetest);
    cout << tmp->score << endl;


}