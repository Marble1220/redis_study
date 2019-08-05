#include "object.h"

static BaseObject* temp = nullptr;

int main(){

    auto test1 = new ListObject();
    
    for (int i = 0; i < 20; i++){
        temp = new StringObject((long long)i);
        test1->ListObjectPush(temp, TAIL);
        decrRefCount(temp);
    }
    auto test2 = new ListObject();
    for (int i = 0; i < 20; i++){
        temp = new StringObject((long long)i);
        test2->ListObjectPush(temp, TAIL);
        decrRefCount(temp);
    }
    cout << *(test1->get_value()) << endl;
    cout << test1->hash() << endl;

    if (test1->match(test2)!=0) cout << "ok" << endl;
    
    delete test2;
    for (int i = 0; i < 20; i++){
        cout << test1->ListObjectIndex(i)->get_buf() << endl;
    }
    cout << "test ListObjectIndex ok" <<endl;

    for (int i = 20; i < 30; i++){
        temp = new StringObject((long long) i);
        test1->ListObjectPush(temp, HEAD);
        decrRefCount(temp);
    }
    cout << test1->get_value()->get_buf() << endl;

    cout << "test ListObjectPush ok" << endl;

    for (int i = 0; i < 30; i++){
        temp = new StringObject((long long) -i);
        test1->ListObjectSet(i, temp);
        decrRefCount(temp);
    }
    cout << test1->get_value()->get_buf() << endl;

    cout << "test ListObjectSet ok" << endl;

    for (int i = 0; i < 7; i++){
        cout << *(test1->ListObjectPop(TAIL)) << endl;
    }
    for (int i = 0; i < 8; i++){
        cout << *(test1->ListObjectPop(HEAD)) << endl;
    }

    cout << "test ListObjectPop ok" << endl;
    
    cout << test1->get_value()->get_buf() << endl;
    for (int i = 8; i < 23; i++){
        temp = new StringObject((long long) -i);
        cout <<test1->ListObjectRem(-1, temp) << endl;
        decrRefCount(temp);
    }
    cout << test1->get_value()->get_buf() << endl;


    for (int i = 0; i < 10; i++){
        temp = new StringObject("a");
        test1->ListObjectPush(temp, TAIL);
        decrRefCount(temp);        
    }

    for (int i = 0; i < 10; i++){
        temp = new StringObject("b");
        test1->ListObjectPush(temp, TAIL);
        decrRefCount(temp);
    }

    for (int i = 0; i < 10; i++){
        temp = new StringObject("c");
        test1->ListObjectPush(temp, TAIL);
        decrRefCount(temp);
    }

    cout << test1->get_value()->get_buf() << endl;
    temp = new StringObject("b");
    cout << test1->ListObjectRem(-3, temp) << endl;
    cout << test1->ListObjectRem(3, temp) << endl;
    cout << test1->ListObjectRem(0, temp) << endl;
    decrRefCount(temp);
    cout << test1->get_value()->get_buf() << endl;
    temp = new StringObject("a");
    test1->ListObjectRem(0, temp);
    decrRefCount(temp);
    temp = new StringObject("c");

    cout << test1->ListObjectRem(111, temp) << endl;
    decrRefCount(temp);
    cout << test1->get_value()->get_buf() << endl;
}