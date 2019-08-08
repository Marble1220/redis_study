#include "object.h"

int main(){
    createSharedObjects();
    StringObject* temp;

    ZsetObject* zset1 = new ZsetObject();

    for(int i = 0; i < 20; i++){
        temp = new StringObject((long long) i);
        zset1->ZsetAdd(temp, i+100.6);
        decrRefCount(temp);
    }

    cout << zset1->get_value()->get_buf() << endl;
    // delete zset1;
    for (int i = 1; i <= 21; i++){
        if (zset1->ZsetGetByRank(i))
            cout << zset1->ZsetGetByRank(i)->get_value()->get_buf() << endl;
        else
            cout << shared.none->get_value()->get_buf() << endl;
    }

    cout << zset1->ZsetLen() << endl;
    temp = new StringObject("10");
    zset1->ZsetAdd(temp, 2000);
    cout << endl;    
    decrRefCount(temp);
    cout << endl;
    cout << zset1->get_value()->get_buf() << endl;
    delete zset1;
    auto zset2 = new ZsetObject();

    for(int i = 0; i < 20; i++){
        temp = new StringObject((long long) i);
        zset2->ZsetAdd(temp, i+100.6);
        decrRefCount(temp);
    }
    temp = new StringObject("20");
    cout << zset2->ZsetRank(temp) << endl;
    decrRefCount(temp);
    temp = new StringObject("10");
    cout << zset2->ZsetRank(temp) << endl;
    decrRefCount(temp);

    cout << endl;

    for (int i = 0; i < 21; i++){
        temp = new StringObject((long long) i);
        cout << zset2->ZsetScore(temp) << endl;
        decrRefCount(temp);
    }
    cout << endl;
    rangespec range = {104, 109, 0, 0};
    cout << zset2->ZsetCount(range) << endl;

    cout << zset2->ZsetRangeByScore(range)->get_buf() << endl;

    temp = new StringObject("20");
    cout << zset2->ZsetRem(temp) << endl;
    decrRefCount(temp);

    temp = new StringObject("10");
    cout << zset2->ZsetRem(temp) << endl;
    decrRefCount(temp);

    cout << zset2->get_value()->get_buf() << endl;
    

    // zset1->ZsetAdd()
}