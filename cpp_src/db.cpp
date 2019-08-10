#include "db.h"

db::db(){
    val = new dict();
    expires = new dict();
}

db::~db(){
    delete val;
    delete expires;
}


int db::check_expires(StringObject* key){
    auto target = expires->dictFind(key);   
    if (!target) return 1;
    std::stringstream ss(target->v->get_value()->get_buf());
    long long cur_time = timeInMilliseconds();
    long long expires_time;
    ss >> expires_time;
    if (cur_time < expires_time) return 1;
    
    expires->dictDelete(key);
    val->dictDelete(key);
    return -1;
}

int db::remove_expires(StringObject* key){
    return expires->dictDelete(key);
}

long long db::get_expires(StringObject* key){
    auto target = expires->dictFind(key);
    if (!target) return 0;
    std::stringstream ss(target->v->get_value()->get_buf());
    long long res;
    ss >> res;
    return res;
}

int db::set_expires(StringObject *key, long long when){
    auto target = val->dictFind(key);
    if (!target) return 0;
    expires->dictAdd(target->key, new StringObject(when));
    incrRefCount(target->key);
    target->key->lru = timeInMilliseconds();
    return 1;
}


BaseObject* db::set_value(StringObject *key, int object_type, BaseObject* v){
    lru_eliminated();
    auto target = val->dictFind(key);
    if (target && check_expires(key)==1){
        remove_expires(key);
    }

    BaseObject *value;

    switch(object_type){
        case STRINGOBJECT: value = new StringObject(); break;
        case LISTOBJECT: value = new ListObject(); break;
        case SETOBJECT: value = new SetObject(v); break;
        case HASHOBJECT: value = new HashObject(); break;
        case ZSETOBJECT: value = new ZsetObject(); break;
        default: value = nullptr;
    }
    if (target){
        decrRefCount(target->v);
        target->v = value;
    }
    else{
        val->dictAdd(key, value);
        incrRefCount(key);
    }
    return value;
}

BaseObject* db::get_value(StringObject *key){
    if (check_expires(key) == 1){
        auto dentry = val->dictFind(key);
        if (dentry) {
            dentry->key->lru = timeInMilliseconds();
            return dentry->v;
        }
    }
    return nullptr;
}

int db::get_type(StringObject* key){
    if (check_expires(key) == 1){
        auto dentry = val->dictFind(key);
        if (dentry) {
            dentry->key->lru = timeInMilliseconds();
            return dentry->v->object_type;
        }
    }
    return 0;
}

int db::exists(StringObject* key){
    if (check_expires(key) == 1){
        auto dentry = val->dictFind(key);
        if (dentry) {
            dentry->key->lru = timeInMilliseconds();
            return 1;
        }
    }
    return 0;
}

int db::delete_key(StringObject *key){
    if (val->dictDelete(key) == DICT_ERR) return 0;
    expires->dictDelete(key);
    return 1;
}

void db::set_lru(int flag){
    lru = flag;
}

void db::update_eliminatepool(){
    dictEntry *samples[ELIMINATE_POOL_SIZE];

    int k, count;
    count = val->dictGetRandomKeys(samples, ELIMINATE_POOL_SIZE);

    for (int j = 0; j < count; j++){
        unsigned long long idle;
        BaseObject *o;
        dictEntry *de;
        de = samples[j];

        o = de->key;

        idle = estimateObjectIdleTime(o);
        k = 0;
        while (k < ELIMINATE_POOL_SIZE && 
                eliminatepool[k].key &&
                eliminatepool[k].idle < idle)
                k++;
        
        if (k == 0 && eliminatepool[ELIMINATE_POOL_SIZE-1].key != nullptr){
            continue;
        }
        else if (k < ELIMINATE_POOL_SIZE && eliminatepool[k].key == nullptr){}
        else{
            if (eliminatepool[ELIMINATE_POOL_SIZE-1].key == nullptr){
                memmove(eliminatepool+k+1, eliminatepool+k, sizeof(eliminatedele)*(ELIMINATE_POOL_SIZE-k-1));
            }
            else{
                k--;
                memmove(eliminatepool, eliminatepool+1, sizeof(eliminatedele)*k);
            }
        }
        eliminatepool[k].key = de->key;
        eliminatepool[k].idle = idle;
    }
}

size_t db::lru_eliminated(){
    if (!lru)return 0;
    if (maxmemory > get_used_memory()) return 0;
    size_t tofree = get_used_memory() - maxmemory;
    size_t freed = 0;
    cout << "tofree: " << tofree << endl;
    dictEntry *de;
    while (freed < tofree){
        update_eliminatepool();
        for (int k = ELIMINATE_POOL_SIZE - 1; k >= 0; k--){
            if (eliminatepool[k].key == nullptr)continue;
            de = val->dictFind(eliminatepool[k].key);
            eliminatepool[k].key = nullptr;
            memmove(eliminatepool+k, eliminatepool+k+1, sizeof(eliminatedele)*(ELIMINATE_POOL_SIZE-k-1));
            eliminatepool[ELIMINATE_POOL_SIZE-1].key = nullptr;
            eliminatepool[ELIMINATE_POOL_SIZE-1].idle = 0;

            if (de){
                long long delta = (long long)get_used_memory();
                delete_key(de->key);
                delta -= (long long)get_used_memory();
                freed += delta;
            }

        }

    }




}