#include "object.h"
#include "dict.h"
int main(){
    createSharedObjects();
    HashObject *hash1 = new HashObject();
    for (int i = 0; i < 20; i++){
        hash1->HashSet(new StringObject((long long) i), new StringObject((long long)i+100));
    }
    
    cout << hash1->HashLen() << endl;
    
    sdshdr *temp1, *temp2;
    temp1 = hash1->HashKeys();
    temp2 = hash1->HashValues();

    while (temp1 != nullptr || temp2 != nullptr){
        cout << temp1->get_buf() << " " << temp2->get_buf() << endl;
        temp1 = hash1->HashKeys();
        temp2 = hash1->HashValues();
    }
    cout << endl;
    cout << hash1->HashGet(new StringObject("10"))->get_value()->get_buf() << endl;
    cout << hash1->HashGet(new StringObject("12"))->get_value()->get_buf() << endl;

    cout << hash1->HashGet(new StringObject("asdf"))->get_value()->get_buf() << endl;
    
    cout << endl;
    cout << hash1->HashExists(new StringObject("17")) << endl;
    cout << hash1->HashExists(new StringObject("asf")) << endl;
    hash1->HashSet(new StringObject("asf"), new StringObject("akk"));
    cout << hash1->HashExists(new StringObject("asf")) << endl;
    cout << hash1->HashDel(new StringObject("asf")) << endl;
    cout << hash1->HashExists(new StringObject("asf")) << endl;

    cout << hash1->get_value()->get_buf() << endl;

    hash1->HashSet(new StringObject("0"), new StringObject("asdf"));
    cout << hash1->get_value()->get_buf() << endl;
}

