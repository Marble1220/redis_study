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
    createSharedObjects();
    StringObject *temp1, *str1;
    BaseObject *temp2;

    db *db1 = new db();

    signal(SIGALRM, sigala_handler);
    alarm(1);
    

    for (;;){
        temp1 = new StringObject(cot);
        temp2 = new StringObject(cot);
        db1->set_value(temp1, STRINGOBJECT, temp2);
        cot ++;
        decrRefCount(temp1);
        decrRefCount(temp2);
        // 性能最差的zset设置速度为57w/s
    }
    // for (int i = 0; i < 1000000; i++){
    //     temp1 = new StringObject((long long) i);
    //     temp2 = new StringObject((long long) i+100);
    //     db1->set_value(temp1, STRINGOBJECT, temp2);
    //     decrRefCount(temp1);
    //     decrRefCount(temp2);

    // }



    // for (;;){
        // temp1 = new StringObject(cot+500000);
        // temp2 = new StringObject("asd");
        // db1->get_value(temp1);
        // cot ++;
        // decrRefCount(temp1);
        // 基本查找性能为181w/s
    // }
    long long sizemask = (1 << 14)-1;
    for (long long i = 0; i <= (1 << 14)-1; i++){
        temp1 = new StringObject(i);
        db1->set_value(temp1, LISTOBJECT, nullptr);

    }

    long long flag;
    for (;;){
        // cout << cot << endl;
        temp1 = new StringObject(cot);
        flag = random() & sizemask;
        // cout << flag << endl;
        str1 = new StringObject(flag);
        static_cast<ListObject*>(db1->get_value(str1))->ListObjectPush(temp1, 1);
        cot += 1;
        decrRefCount(temp1);
        decrRefCount(str1);
        if (cot >= 1000000) break;
        // list的基本插入性能 125w/s
    }


    cot = 0;
    for (;;){
        temp1 = new StringObject(cot);
        flag = random() & sizemask;
        // cout << flag << endl;
        str1 = new StringObject(flag);
        auto val = static_cast<ListObject*>(db1->get_value(str1))->ListObjectPop(1);
        // cout << val->get_buf() << endl;
        if (val == shared.none->get_value()){}
        else delete val;
        cot += 1;
        // list的基本弹出性能为80w
    }

    


    // auto iter = db1->val->dictGetIterator();
    // auto temp = iter->dictNext();
    // while (temp){
    //     cout << temp->v->object_type << endl;
    //     temp = iter->dictNext();
    // }
    // for (int i = 0; i <= 50000000; i++){
    //     temp1 = new StringObject((long long) i);
    //     // cout << get_used_memory() << endl;
    //     db1->set_value(temp1, i, temp1);
    //     decrRefCount(temp1);
    // }
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