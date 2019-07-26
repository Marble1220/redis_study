#include "intset.h"



int64_t intset::operator[](int pos) const{
    return contents->operator[](pos);
}








