#include "intset.h"



int64_t intset::operator[](int pos) const{
    return contents->operator[](pos);
}

int intsetmatch(void* ptr, void* key){

    intset* new_ptr = static_cast<intset*> (ptr);
    intset* new_key = static_cast<intset*> (key);

    if (new_key->encoding != new_ptr->encoding || 
    new_key->length() != new_ptr->length()) return 0;


    for (int i = 0; i < new_key->length(); i++){
        if ((*new_key)[i] != (*new_ptr)[i]) return 0;
    }
    return 1;
}






