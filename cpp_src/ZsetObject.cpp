#include "object.h"
#include "dict.h"
#include "skiplist.h"

ZsetObject::ZsetObject():BaseObject(ZSETOBJECT){
    dict_ptr = new dict();
    ptr = new skiplist();
}

ZsetObject::~ZsetObject(){
    delete ptr;
    delete dict_ptr;
}


sdshdr* ZsetObject::get_value() const{
    auto temp = static_cast<skiplist*> (ptr);
    auto eptr = temp->header->level[0].forward;
    auto res = new sdshdr();
    while (eptr){
        res->sdscatlen(eptr->obj->get_value()->get_buf(), eptr->obj->get_value()->length());
        res->sdscatlen(shared.space->get_value()->get_buf(), 1);
       
        auto temp_str = StringObject((long double)eptr->score);
        res->sdscatlen(temp_str.get_value()->get_buf(), temp_str.get_value()->length()); 
        res->sdscatlen(shared.wrap->get_value()->get_buf(), 2);
        eptr = eptr->level[0].forward;
    }
    return res;

}
int ZsetObject::match(BaseObject* other) const{
    if (this == other) return 0;
    return -1;
};
unsigned ZsetObject::hash() const{
    unsigned int res = 0;
    auto temp = static_cast<skiplist*> (ptr);
    auto eptr = temp->header->level[0].forward;
    while (eptr){
        res ^= eptr->obj->hash();
        eptr = eptr->level[0].forward;
    }
    return res;
}




int ZsetObject::ZsetAdd(StringObject* value, double score){
    auto temp = static_cast<skiplist*>(ptr);
    auto score_str = new StringObject((long double) score);
    auto dentry = dict_ptr->dictFind(value);
    if (dentry){
        std::stringstream ss(dentry->v->get_value()->get_buf());
        double origin_score;
        ss >> origin_score;
        temp->slDelete(origin_score, value);
        temp->slInsert(score, dentry->key);
        incrRefCount(dentry->key);
        decrRefCount(dentry->v);
        dentry->v = score_str;
        return 0;
    }
    else{
        temp->slInsert(score, value);
        dict_ptr->dictAdd(value, score_str);
        incrRefCount(value);
        incrRefCount(value);
        return 1;
    }
    
}

int ZsetObject::ZsetLen(){return dict_ptr->length();};

int ZsetObject::ZsetRank(StringObject* value){
    auto temp = static_cast<skiplist*>(ptr);
    double score;
    auto dentry = dict_ptr->dictFind(value);
    if (!dentry) return -1;
    std::stringstream ss(dentry->v->get_value()->get_buf());
    ss >> score;
    return temp->slGetRank(score, value);
}

BaseObject* ZsetObject::ZsetGetByRank(int rank){
    auto temp = static_cast<skiplist*>(ptr);
    auto res = temp->slGetElementByRank(rank);
    return res == nullptr? nullptr: res->obj;

}

double ZsetObject::ZsetScore(StringObject* value){
    double res;
    auto dentry = dict_ptr->dictFind(value);
    if (dentry){
        std::stringstream ss(dentry->v->get_value()->get_buf());
        ss >> res;
        return res;
    }
    return -1;
}

int ZsetObject::ZsetRem(StringObject* value){
    auto dentry = dict_ptr->dictFind(value);
    
    if (!dentry) return PY_ERR;

    auto temp = static_cast<skiplist*> (ptr);    
    std::stringstream ss(dentry->v->get_value()->get_buf());
    double score;
    ss >> score;
    temp->slDelete(score, value);
    dict_ptr->dictDelete(value);
    return PY_OK;
}

int ZsetObject::ZsetCount(rangespec range){
    int res = 0;
    auto temp = static_cast<skiplist*>(ptr);

    auto eptr = temp->slFirstInRange(&range);
    while (eptr && slValueLteMax(eptr->score, &range)){
        res += 1;
        eptr = eptr->level[0].forward;

        // if (!eptr || !slValueLteMax(eptr->score, &range)) break;
    }    

    return res;
}

sdshdr* ZsetObject::ZsetRangeByScore(rangespec range){
    auto res = new sdshdr();
    auto temp = static_cast<skiplist*>(ptr);

    auto eptr = temp->slFirstInRange(&range);
    while (eptr && slValueLteMax(eptr->score, &range)){
        int len = eptr->obj->get_value()->length();
        if (len < 10000){
            res->sdscatlen(shared.integers[len]->get_value()->get_buf(), shared.integers[len]->get_value()->length());
        }
        else{
            auto temp_sds = sdshdr(len);
            res->sdscatlen(temp_sds.get_buf(), temp_sds.length());
        }
        res->sdscatlen(eptr->obj->get_value()->get_buf(), len);
        eptr = eptr->level[0].forward;
    }
    return res;
}