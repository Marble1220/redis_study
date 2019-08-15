#include "object.h"


sdshdr* StringObject::get_value() const {
    return static_cast<sdshdr*>(ptr);
}

int StringObject::match(BaseObject* other) const {
    if (other->object_type != object_type){
        throw std::runtime_error("type error. can not compare two different class");
    }
    sdshdr *temp1, *temp2;
    temp1 = static_cast<sdshdr*> (ptr);
    temp2 = static_cast<sdshdr*> (other->ptr);
    if (*temp1 == *temp2) return 0;
    int len1 = temp1->sdslen(), len2 = temp2->sdslen();
    int len = (len1 > len2)? len2: len1;
    auto array1=temp1->get_buf(), array2 = temp2->get_buf();

    for (int i = 0; i < len; i++){
        if (array1[i] == array2[i]) continue;
        if (array1[i] > array2[i]) return 1;
        return -1;
    }
    return (len1 > len2)? 1: -1;
}


unsigned int StringObject::hash() const {
    sdshdr *sdstemp = static_cast<sdshdr*>(ptr);
    int sds_len = sdstemp->length();
    const char *c_str = sdstemp->get_buf();
    uint32_t seed = hash_function_seed;
    const uint32_t m = 0x5db1e995;
    const int r = 24;

    uint32_t h = seed ^ sds_len;

    while (sds_len >= 4){
        uint32_t k = *(uint32_t*)c_str;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h^= k;

        c_str += 4;
        sds_len -= 4;
    }

    switch (sds_len){
        case 3: h ^= c_str[2] << 16;
        case 2: h ^= c_str[1] << 8;
        case 1: h ^= c_str[0]; h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    
    return (unsigned int)h; 
};


StringObject* StringObject::dupStringObject(){
    StringObject* res;
    res = new StringObject(this->get_value()->get_buf(), this->length());
    return res;
};



size_t StringObject::length(){
    return this->ptr->length();
};

int StringObject::StringObjectcat(sdshdr* other){
    this->get_value()->sdscatlen(other->get_buf(), other->length());
}

long StringObject::getlongintfromstring(){
    std::stringstream ss(this->get_value()->get_buf());
    long res;
    ss >> res;
    return res;
}

sdshdr* StringObject::getrangefromStringObject(int start, int end){
    sdshdr *temp = this->get_value(), *res;
    int len = temp->length();
    int reslen;
    if (len == 0) return nullptr;

    if (start < 0){
        start = len + start;
        if (start < 0) start = 0;
    }

    if (end < 0){
        end = len + end;
        if (end < 0) end = 0;
    }
    reslen = (start > end)? 0: (end - start + 1);
    if (reslen != 0){
        if (start >= (signed) len) return nullptr;
        else if(end > (signed) len){
            end = len - 1;
            reslen = (start > end)? 0: (end - start)+1;
        }
    }else{
        start = 0;
    }
    
    res = new sdshdr();
    res->sdsgrowzero(reslen);
    memcpy(res->get_buf(), temp->get_buf()+start, reslen);
    return res;
}