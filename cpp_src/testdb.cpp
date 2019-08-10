#include "db.h"
#include <unistd.h>
#include <signal.h>
static long long  cot = 0;

void sigala_handler(int sig){
    cout << cot << endl;
    cout << get_used_memory() << endl;
    exit(0);
}

int main(){
    StringObject *temp1, *str1;
    BaseObject *temp2;

    db *db1 = new db();
    // auto iter = db1->val->dictGetIterator();
    // auto temp = iter->dictNext();
    // while (temp){
    //     cout << temp->v->object_type << endl;
    //     temp = iter->dictNext();
    // }
    for (int i = 0; i <= 50000000; i++){
        temp1 = new StringObject((long long) i);
        // cout << get_used_memory() << endl;
        db1->set_value(temp1, i, temp1);
        decrRefCount(temp1);
    }
    // for (int i = 25; i <= 29; i++){
    //     temp1 = new StringObject((long long) i);
    //     cout << db1->get_type(temp1) << ' ' << db1->get_value(temp1)->object_type << endl;

    //     decrRefCount(temp1);
    // }
    // for (int i = 25; i <= 30; i++){
    //     temp1 = new StringObject((long long) i);
    //     cout << db1->exists(temp1) << endl;

    //     decrRefCount(temp1);
    // }

    // for (int i = 25; i <= 30; i++){
    //     temp1 = new StringObject((long long) i);
    //     cout << db1->delete_key(temp1) << endl;

    //     decrRefCount(temp1);
    // }

    // for (int i = 25; i <= 29; i++){
    //     temp1 = new StringObject((long long) i);
    //     db1->set_value(temp1, i, temp1);
    //     decrRefCount(temp1);
    // }
    // cout << endl;
    // temp1 = new StringObject("25");
    // db1->set_expires(temp1, timeInMilliseconds()+1000);
    // cout << db1->get_value(temp1)->object_type << endl;
    // cout << db1->get_expires(temp1) << endl;
    // cout << db1->expires->length() << endl;
    // cout << db1->remove_expires(temp1) << endl;
    // sleep(2);
    // cout << db1->get_value(temp1) << endl;
    // cout << get_used_memory() << endl;
    // delete db1;
    // createSharedObjects();
    cout << get_used_memory() << endl;
    // db1->update_eliminatepool();
    // db1->update_eliminatepool();
    // db1->update_eliminatepool();
    // db1->update_eliminatepool();
    // db1->update_eliminatepool();
    // for (int i = 0; i < ELIMINATE_POOL_SIZE; i++){
    //     if (db1->eliminatepool[i].key)
    //         cout << db1->eliminatepool[i].key->lru<< endl;
    // }
    





}