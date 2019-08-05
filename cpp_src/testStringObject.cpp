#include "object.h"


int main(){
    BaseObject *test2, *test1;
    test1 = new StringObject("test1");
    test2 = new StringObject("test2");

    cout << *(test1->get_value()) << endl;
    cout << *(test2->get_value()) << endl;
    cout << test1->ref << endl;
    StringObject *test3 = static_cast<StringObject*>(test1)->dupStringObject();
    cout << *(test3->get_value()) << endl;
    StringObject *test4 = new StringObject((long long)55223);
    cout << test4->getlongintfromstring() << endl;
    cout << static_cast<StringObject*>(test3)->getlongintfromstring() << endl;
    
    test3->StringObjectcat(new sdshdr(5552));
    cout << *(test3->get_value()) << endl;
    sdshdr * test5 = test3->getrangefromStringObject(1, 3);
    cout << test5->get_buf() << endl;

}