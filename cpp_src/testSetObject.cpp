#include "object.h"

static StringObject* temp;

int main(){
    createSharedObjects();
    SetObject *set1 = new SetObject(new StringObject("0"));
    
    for(int i = 0; i < 511; i++){
        temp = new StringObject((long long) i);
        set1->SetObjectAdd(temp);
        decrRefCount(temp);
    }
    cout << set1->get_value()->get_buf() << endl;
    cout << set1->SetObjectLen() << endl;
    cout << set1->ptr->struct_type << endl;
    temp = new StringObject((long long) 10);
    cout << set1->SetObjectExist(temp) << endl;
    decrRefCount(temp);
    cout << set1->SetObjectExist(new StringObject("asd")) << endl;

    // while (set1->SetObjectLen() >0){
    //     cout << set1->SetObejctPop()->get_buf() << endl;
    //     cout << set1->SetObjectLen() << endl;
    // }
    // for(int i = 0; i < 511; i++){
    //     cout << set1->SetObjectRem(new StringObject((long long) i)) << endl;
    // }
    // cout << set1->get_value()->get_buf() << endl;
    // cout << set1->SetObjectLen() << endl;

    set1->SetObjectAdd(new StringObject("addf"));
    cout << set1->ptr->struct_type << endl;
    cout << set1->get_value()->get_buf() << endl;
    cout << set1->SetObjectLen() << endl;
    for (int i = 511; i < 1000; i++){
        set1->SetObjectAdd(new StringObject((long long) i));
    }

    cout << set1->SetObjectLen() << endl;
    cout << set1->get_value()->get_buf() << endl;

    while (set1->SetObjectLen() > 0){
        cout << set1->SetObejctPop()->get_buf() << endl;
    }
    cout << set1->SetObjectLen() << endl;

    cout << set1->ptr->struct_type << endl;
    for (int i = 511; i < 1000; i++){
        set1->SetObjectAdd(new StringObject((long long) i));
    }

    cout << set1->get_value()->get_buf() << endl;

    cout << set1->SetObjectLen() << endl;
    cout << endl;
    cout << set1->SetObjectExist(new StringObject("555")) << endl;
    cout << set1->SetObjectRem(new StringObject("555")) << endl;
    cout << set1->SetObjectExist(new StringObject("555")) << endl;

    cout << set1->SetObjectLen() << endl;

    for (int i = 511; i < 1000; i++)
        set1->SetObjectRem(new StringObject((long long) i));
    
    cout << set1->get_value()->get_buf() << endl;
    cout << set1->SetObjectLen() << endl;
    delete set1;
    
    set1 = new SetObject(new StringObject("0"));
    for (int i = 0; i < 512; i++){
        set1->SetObjectAdd(new StringObject((long long) i));
    }
    cout << set1->ptr->struct_type << endl;
    cout << set1->SetObjectLen() << endl;
    cout << set1->get_value()->get_buf() << endl;
    
    cout << endl;

    set1->SetObjectAdd(new StringObject((long long) 555));
    
    cout << set1->ptr->struct_type << endl;
    cout << set1->get_value()->get_buf() << endl;
    cout << set1->SetObjectLen() << endl;

    

}