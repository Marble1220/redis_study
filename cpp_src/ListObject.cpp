#include "object.h"
#include "adlist.h"




ListObject::ListObject():BaseObject(LISTOBJECT){
    ptr = new adlist();
}

ListObject::~ListObject(){
    delete ptr;
    ptr = nullptr;
    
}

int ListObject::match(BaseObject *other) const{
    if (ptr == other->ptr) return 0;
    else return -1; 
}

sdshdr* ListObject::get_value() const {
    sdshdr *res = new sdshdr(), temp = sdshdr();
    int len = ptr->length();
    auto iter = static_cast<adlist*>(ptr)->lst->begin();
    for (int i = 0; i < len; i++){
        temp.sdscatlen((*iter)->get_value()->get_buf(), (*iter)->get_value()->length());
        temp.sdscatlen("\r\n", 2);

        res->sdscatlen(temp.get_buf(), temp.length());
        temp.sdsclear();
        ++iter;
    }
    return res;    

}

unsigned ListObject::hash() const{
    unsigned int res;
    auto iter = static_cast<adlist*>(ptr)->lst->begin();
    int len = ptr->length();
    for (int i = 0; i < len; i++){
        res ^= (*iter)->hash();
    }
    return res;
}


int ListObject::ListObjectPush(BaseObject* value, int where){
    adlist *temp = static_cast<adlist*>(ptr);
    try{
        if (where) temp->adlistAddNodeTail(value);
        else temp->adlistAddNodeHead(value);
    }
    catch (std::exception){
        return PY_ERR;
    }
    incrRefCount(value);
    return PY_OK;
}

sdshdr* ListObject::ListObjectIndex(int index){
    int length = ptr->length();
    if (index < 0) index = length + index;
    if (index < 0 || index >= length) return shared.none->get_value();

    adlist *temp = static_cast<adlist*>(ptr);

    auto iter = temp->adlistGetBegin();
    for (int i = 0; i < index; i++) iter++;
    return (*iter)->get_value();
}



int ListObject::ListObjectSet(int index, BaseObject* value){
    auto temp = static_cast<adlist*>(ptr);
    auto origin_value = temp->adlistIndex(index);
    if (origin_value == temp->adlistGetEnd()) return PY_ERR;
    
    decrRefCount(*origin_value);
    *origin_value = value;
    incrRefCount(*origin_value);
    return PY_OK;
}

sdshdr* ListObject::ListObjectPop(int where){
    auto temp = static_cast<adlist*>(ptr);
    adlist::adlistIter iter;
    if ((temp->length()) == 0) return shared.none->get_value();
    if (where == HEAD) iter = temp->adlistGetBegin();
    else iter = --temp->adlistGetEnd();

    sdshdr* res = new sdshdr(*(*iter)->get_value());
    decrRefCount(*iter);
    temp->adlistDelNode(iter);
    return res;
}

int ListObject::ListObjectRem(int nums, BaseObject *value){
    int cot = 0;
    auto temp = static_cast<adlist*>(ptr);
    auto lst = temp->lst;

    if (nums >= 0){
        for (auto iter = lst->begin(); iter != lst->end() && (nums==0 || cot<nums);){
            if ((*iter)->match(value) == 0){
                decrRefCount(*iter);
                iter = lst->erase(iter);
                cot++;
            }
            else{
                iter++;
            }
        }
    }
    else{
        auto iter = lst->end();
        iter --;
        auto begin = lst->begin();
        while (iter != begin && cot < -nums){
            if ((*iter)->match(value) == 0){
                decrRefCount(*iter);
                iter = lst->erase(iter);
                cot++;
            }
            iter--;
        }

        if (cot < -nums && ((*iter)->match(value)) == 0){
            decrRefCount(*iter);
            lst->erase(iter);
            cot++;
        }

    }
    return cot;

}

