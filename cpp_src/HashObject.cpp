#include "object.h"
#include "dict.h"


HashObject::HashObject():BaseObject(HASHOBJECT), keyiter(nullptr), valiter(nullptr){
    ptr = new dict();
}

HashObject::~HashObject(){
    delete ptr;
    ptr = nullptr;
}

BaseObject* HashObject::HashGet(StringObject* key){
    auto res = static_cast<dict*>(ptr)->dictFetchValue(key);
    return res != nullptr? res: shared.none;
}

int HashObject::HashExists(StringObject* key){
    return static_cast<dict*>(ptr)->dictFind(key)!=nullptr? 1: 0;
}

int HashObject::HashSet(StringObject* key, BaseObject* value){
    auto temp = static_cast<dict*>(ptr);
    // if (temp->dictAdd(key, value) == DICT_OK) return PY_OK;
    // else return PY_ERR;
    return temp->dictReplace(key, value);
}

int HashObject::HashDel(StringObject* key){
    auto temp = static_cast<dict*>(ptr);
    if (temp->dictDelete(key) == DICT_OK) return PY_OK;
    else return PY_ERR;
}


int HashObject::HashLen(){return ptr->length();};


dictEntry* HashObject::_HashIter(dictIterator *&iterptr){
    if (iterptr == nullptr){
        auto temp = static_cast<dict*>(ptr);
        iterptr = temp->dictGetIterator();
    }
    auto dentry = iterptr->dictNext();
    if (dentry == nullptr){
        delete iterptr;
        iterptr = nullptr;
        return nullptr;
    }
    return dentry;
    // auto res = new sdshdr(dentry->key->get_value()->get_buf(), dentry->key->get_value()->length());
    // return res;
}

sdshdr* HashObject::HashKeys(){
    auto dentry = _HashIter(keyiter);
    if (dentry)
        return dentry->key->get_value();
    return nullptr;
}

sdshdr* HashObject::HashValues(){
    auto dentry = _HashIter(valiter);
    if (dentry)
        return dentry->v->get_value();
    return nullptr;
}


unsigned int HashObject::hash() const{
    return static_cast<dict*>(ptr)->dictFingerprint();
}

sdshdr* HashObject::get_value() const{
    dict *temp = static_cast<dict*> (ptr);
    sdshdr* res = new sdshdr();
    dictIterator* iter = temp->dictGetIterator();
    dictEntry *dentry;
    while ((dentry = iter->dictNext()) != nullptr){
        res->sdscatlen(dentry->key->get_value()->get_buf(), dentry->key->get_value()->length());
        res->sdscatlen(shared.space->get_value()->get_buf(), 1);
        if (dentry->v){
            res->sdscatlen(dentry->v->get_value()->get_buf(), dentry->v->get_value()->length());
        }
        res->sdscatlen(shared.wrap->get_value()->get_buf(), 2);
    }
    delete iter;
    return res;
}

int HashObject::match(BaseObject* other) const{
    if (this == other) return 0;
    return -1;
}