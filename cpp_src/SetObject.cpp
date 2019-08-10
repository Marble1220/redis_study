#include "object.h"
#include "dict.h"
#include "intset.h"

SetObject::SetObject(BaseObject *value): BaseObject(SETOBJECT){
    long long temp_val;
    if (string2ll(value->get_value()->get_buf(), value->get_value()->length(), &temp_val)){
        ptr = new intset();
    }
    else{
        ptr = new dict();
    }
}

SetObject::~SetObject(){
    delete ptr;
    ptr = nullptr;
}


sdshdr* SetObject::get_value() const{
    if (ptr->length() == 0) return shared.none->get_value();

    sdshdr* res = new sdshdr();
    if (ptr->struct_type == INTSETSTRUCT){
        int len = ptr->length();
        auto temp = static_cast<intset*> (ptr);
        StringObject* temp_str;
        for (int i = 0; i < len; i++){
            temp_str = new StringObject((long long) i);
            res->sdscatlen(temp_str->get_value()->get_buf(), temp_str->get_value()->length());
            res->sdscatlen(shared.wrap->get_value()->get_buf(), 2);
            delete temp_str;
            temp_str = nullptr;
        }
    }
    else{
        auto temp = static_cast<dict*>(ptr);
        auto iter = temp->dictGetIterator();
        auto node1 = iter->dictNext();
        while (node1){
            res->sdscatlen(node1->key->get_value()->get_buf(), node1->key->get_value()->length());
            res->sdscatlen(shared.wrap->get_value()->get_buf(), 2);
            node1 = iter->dictNext();
        }

    }
    return res;
}


int SetObject::match(BaseObject* other) const {
    if (other == this) return 0;
    return -1;
}

unsigned SetObject::hash() const{
    unsigned res;
    if (ptr->struct_type == INTSETSTRUCT){
        auto temp = static_cast<intset*>(ptr);
        int len = temp->length();
        for (int i = 0; i < len; i++){
            res ^= (*temp)[i];
        }
    }
    else{
        auto temp = static_cast<dict*>(ptr);
        res = temp->dictFingerprint();
    }
    return res;
}


int SetObject::SetObjectExist(BaseObject *value){
    if (ptr->struct_type == INTSETSTRUCT){
        long long temp_long;
        intset *temp = static_cast<intset*>(ptr);
        if (string2ll(value->get_value()->get_buf(), value->get_value()->length(), &temp_long)){
            return temp->intsetFind(temp_long);
        }
    }
    else if (ptr->struct_type == DICTSTRUCT){
        dict *temp = static_cast<dict*>(ptr);
        auto str_obj = static_cast<StringObject*>(value);
        auto res = temp->dictFind(str_obj);
        if (res != nullptr) return 1;
    }

    return 0;

}

void SetObject::SetObjectUpgrade(){
    auto dict_temp = new dict();
    auto intset_temp = static_cast<intset*>(ptr);
    dict_temp->dictExpand(intset_temp->length());
    for(int i = 0; i < intset_temp->length(); i++){
        dict_temp->dictAdd(new StringObject((long long)(*intset_temp)[i]), nullptr);
    }
    delete ptr;
    ptr = dict_temp;
}


int SetObject::SetObjectAdd(BaseObject *value){
    if (ptr->struct_type == DICTSTRUCT){
        auto temp = static_cast<dict*>(ptr);
        auto res = temp->dictAdd(static_cast<StringObject*>(value), nullptr);
        if (res == DICT_ERR) return PY_EXIST;
        else {
            incrRefCount(value);
            return PY_OK;
        }
    }
    else if (ptr->struct_type == INTSETSTRUCT){
        long long temp_long;
        if (string2ll(value->get_value()->get_buf(), value->get_value()->length(), &temp_long) == 1){
            auto temp = static_cast<intset*> (ptr);
            int success = 0;
            temp->intsetAdd(temp_long, &success);
            if (success){
                if (temp->length() > SET_INTSET_MAX){
                    SetObjectUpgrade();
                }
                return PY_OK;
            }
        }
        else{
            SetObjectUpgrade();
            auto temp = static_cast<dict*>(ptr);
            auto res = temp->dictAdd(static_cast<StringObject*>(value), nullptr);
            if (res == DICT_ERR) return PY_EXIST;
            else {
                incrRefCount(value);
                return PY_OK;
            }
        }
    }
    return PY_ERR;
}

int SetObject::SetObjectLen(){
    return ptr->length();
}


int SetObject::SetObjectRem(BaseObject *value){
    if (ptr->struct_type == DICTSTRUCT){
        auto temp = static_cast<dict*>(ptr);
        auto res = temp->dictDelete(static_cast<StringObject*>(value));
        if (res == DICT_OK) return PY_OK;
    }
    else if (ptr->struct_type == INTSETSTRUCT){
        long long temp_long;
        // auto temp_string = static_cast<StringObject*>(value);
        if (string2ll(value->get_value()->get_buf(), value->get_value()->length(), &temp_long)){
            int success;
            auto temp = static_cast<intset*>(ptr);
            temp->intsetRemove(temp_long, &success);
            if (success) return PY_OK;
        }
    }

    return PY_ERR;
}

sdshdr* SetObject::SetObejctPop(){
    if (SetObjectLen() == 0){
        incrRefCount(shared.none); 
        return shared.none->get_value();
    }
    sdshdr* res;

    if (ptr->struct_type == DICTSTRUCT){
        auto temp = static_cast<dict*>(ptr);
        auto random_key = temp->dictGetRandomKey()->key;
        res = new sdshdr(random_key->get_value()->get_buf(), random_key->get_value()->length());
        temp->dictDelete(random_key);
        return res;
    }
    else if (ptr->struct_type == INTSETSTRUCT){
        auto temp = static_cast<intset*>(ptr);
        int len = temp->length();
        int rand = random() % len;
        res = new sdshdr((*temp)[rand]);

        int success;
        temp->intsetRemove((*temp)[rand], &success);
        if (success) return res; 
    }

    return nullptr;

}