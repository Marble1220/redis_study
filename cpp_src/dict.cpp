#include "dict.h"

void dictEnableResize(void) {
    dict_can_resize = 1;
}

void dictDisableResize(void) {
    dict_can_resize = 0;
}

void dictht::dictReset(){
    table = nullptr;
    size = 0;
    sizemask = 0;
    used = 0;
}

int dict::dictResize(){
    int minimal;
    if (!dict_can_resize || rehashidx != -1) return DICT_ERR;

    minimal = ht[0].used;
    if (minimal < DICT_HT_INITIAL_SIZE) minimal = DICT_HT_INITIAL_SIZE;

    return dictExpand(minimal);
}


int dict::dictExpand(unsigned long size){
    cout << "call expand, size is: " << size << endl;
    dictht n;
    unsigned long realsize = _dictNextPower(size);

    if (rehashidx != -1 || ht[0].used > size) return DICT_ERR;
    
    n.size = realsize;
    n.sizemask = realsize - 1;
    n.table = new dictEntry*[realsize]();
    // for (int i = 0; i < realsize; i++) n.table[i] = nullptr;
    // memset(n.table, 0, sizeof(dictEntry*)*realsize);
    n.used = 0;

    if (ht[0].table == nullptr){
        // cout << "new dict" << endl;
        ht[0] = n;
        return DICT_OK;
    }
    // cout << "rehash start" << endl;
    ht[1] = n;
    rehashidx = 0;
    n.table = nullptr;
    return DICT_OK;
}

int dict::dictRehash(int n){
    if (rehashidx == -1) return 0;
    // cout << "rehash step: " << n << endl;
    // cout << "rehashidx: " << rehashidx << endl;
    
    while (n--){
        dictEntry *de, *nextde;

        if (ht[0].used == 0){
            delete[] ht[0].table;
            ht[0] = ht[1];
            ht[1].dictReset();
            rehashidx = -1;
            // cout << "rehash OK" << endl;
            return 0;
        }

        assert(ht[0].size > (unsigned)rehashidx);
        
        while (ht[0].table[rehashidx] == nullptr) rehashidx++;
        
        de = ht[0].table[rehashidx];

        while (de){
            unsigned int h;
            nextde = de->next;
            
            h = de->key->hash() & ht[1].sizemask;

            de->next = ht[1].table[h];
            ht[1].table[h] = de;

            ht[0].used --;
            ht[1].used ++;
            // cout << "old index is: " << rehashidx << " new hashtable index is: " << h << " key: " << *(ht[1].table[h]->key->get_value()) << endl;

            de = nextde;
        }

        ht[0].table[rehashidx] = nullptr;
        rehashidx++;
    }

    return 1;
}


int dict::dictRehashMillliseconds(int ms){
    long long start = timeInMilliseconds();
    int rehashes = 0;
    while (dictRehash(100)){
        rehashes += 100;
        if (timeInMilliseconds() - start > ms) break;
    }
    return rehashes;
}

void dict::_dictRehashStep(){
    if (iterators == 0) dictRehash(1);
}

int dict::dictAdd(StringObject *key, BaseObject *val){
    dictEntry *entry = dictAddRaw(key);

    if (!entry) return DICT_ERR;

    entry->v = val;

    return DICT_OK;
}

dictEntry* dict::dictAddRaw(StringObject *key){
    int index;
    dictEntry *entry;
    dictht *ht_;


    if (rehashidx != -1) _dictRehashStep();

    // 如果key已存在， 返回nullptr
    if ((index = _dictKeyIndex(key)) == -1) return nullptr;

    ht_ = rehashidx != -1? &(ht[1]): &(ht[0]);

    entry = new dictEntry(key);
    entry->next = ht_->table[index];
    ht_->table[index] = entry;
    ht_->used ++;

    return entry;
}

int dict::dictReplace(StringObject *key, BaseObject *val){
    dictEntry *entry;

    if (dictAdd(key, val) == DICT_OK) return 1;

    entry = dictFind(key);
    decrRefCount(entry->v);
    entry->v = val;
    // delete auxentry.v;  // v指向一个object 调用object的析构函数

    return 0;
}


dictEntry* dict::dictReplaceRaw(StringObject *key){
    dictEntry *entry = dictFind(key);

    return entry? entry: dictAddRaw(key);
}

int dict::dictGenericDelete(StringObject *key, int nofree){
    unsigned int h, idx;
    dictEntry *he, *prevHe;
    int table;


    if (ht[0].size == 0) return DICT_ERR;   // dict is empty

    if (rehashidx != -1) _dictRehashStep();

    h = key->hash();

    for (table = 0; table <= 1; table++){
        idx = h & ht[table].sizemask;
        he = ht[table].table[idx];
        prevHe = nullptr;

        while (he){
            
            if (he->key->match(key) == 0){
                if (prevHe) prevHe->next = he->next;
                else ht[table].table[idx] = he->next;

                // if (!nofree){
                //     delete he->key;
                //     delete he->v;
                // }
                delete he;
                ht[table].used --;
                
                return DICT_OK;

            }

            prevHe = he;
            he = he->next;
        }
        if (rehashidx == -1) break;
    }

    return DICT_ERR;

}

int dict::dictDelete(StringObject *key){return dictGenericDelete(key, 0);}
int dict::dictDeleteNoFree(StringObject *key){return dictGenericDelete(key, 1);}

int dict::_dictClear(dictht *ht_, void(callback)(void *)){
    unsigned long i = 0;
    for (i = 0; i < ht_->size && ht_->used > 0; i++){
        dictEntry *he, *nexthe;
        if (callback && (i & 65535) == 0) callback(privdata);
        if ((he = ht_->table[i]) == nullptr) continue;
        
        while (he){
            nexthe = he->next;
            // delete he->key;
            // delete he->v;
            delete he;
            ht_->used--;

            he = nexthe;
        }
        ht_->table[i] = nullptr;

    }
    if (i){
        delete[] ht_->table;
        ht_->dictReset();
    }
    return DICT_OK;
}

dictEntry* dict::dictFind(StringObject *key){
    dictEntry *he;
    unsigned int h, idx;

    if (ht[0].size == 0) return nullptr;

    if (rehashidx != -1) _dictRehashStep();

    h = key->hash();

    for (unsigned long table = 0; table <= 1; table++){
        idx = h & ht[table].sizemask;
        he = ht[table].table[idx];

        while (he){
            if (he->key->match(key) == 0) return he;

            he = he->next;
        }

        if (rehashidx == -1) return nullptr;
    }
    return nullptr;
}

BaseObject* dict::dictFetchValue(StringObject* key){
    dictEntry *he;
    he = dictFind(key);
    return he? he->v: nullptr;
}

long long dict::dictFingerprint(){
    long long integers[6], hash = 0;

    integers[0] = (long) ht[0].table;
    integers[1] = ht[0].size;
    integers[2] = ht[0].used;
    integers[3] = (long) ht[1].table;
    integers[4] = ht[1].size;
    integers[5] = ht[1].used;

    for(int j = 0; j < 6; j++){
        hash += integers[j];
        /* For the hashing step we use Tomas Wang's 64 bit integer hash. */
        hash = (~hash) + (hash << 21); // hash = (hash << 21) - hash - 1;
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8); // hash * 265
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4); // hash * 21
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);
    }
    return hash;
}

dictIterator* dict::dictGetIterator(){
    dictIterator *iter = new dictIterator(this, 0);
    return iter;
}

dictIterator* dict::dictGetSafeIterator(){
    dictIterator *iter = new dictIterator(this, 1);
    return iter;
}

dictEntry* dictIterator::dictNext(){
    while (1){
        if (entry == nullptr){
            // 1.迭代器第一次运行
            // 2.当前索引链表中的节点迭代完毕
            dictht *ht = &d->ht[table];

            if (index == -1 && table == 0){
                // 初次迭代
                if (safe) d->iterators++;
                else fingerprint = d->dictFingerprint();

            }

            index++;

            if (index >= (signed) ht->size){
                //当前hashtable迭代完毕
                if (d->rehashidx != -1 && table == 0){
                    // 如果hash表在rehash状态 则对1号表继续迭代
                    table++;
                    index = 0;
                    ht = &d->ht[table];
                }else{
                    break;
                }
            }

            entry = ht->table[index];
        }
        else{
            entry = nextentry;
        }
        if (entry){
            nextentry = entry->next;
            return entry;
        }
    }
    return nullptr;
}


static unsigned long rev(unsigned long v) {
    unsigned long s = 8 * sizeof(v); // bit size; must be power of 2
    unsigned long mask = ~0;
    while ((s >>= 1) > 0) {
        mask ^= (mask << s);
        v = ((v >> s) & mask) | ((v << s) & ~mask);
    }
    return v;
}

unsigned long dict::dictScan(unsigned long v, dictScanFunction *fn, BaseObject *privdata){
    dictht *t0, *t1;
    const dictEntry *de;
    unsigned long m0, m1;
    if (ht[0].used + ht[1].used == 0) return 0;

    if (rehashidx == -1){
        t0 = &ht[0];
        m0 = t0->sizemask;

        de = t0->table[v & m0];
        while(de){
            fn(privdata, de);
            de = de->next;
        }
    }else{
        t0 = &ht[0];
        t1 = &ht[1];

        if (t0->size > t1->size){
            t0 = &ht[1];
            t1 = &ht[0];
        }

        m0 = t0->sizemask;
        m1 = t1->sizemask;

        de = t0->table[v & m0];
        while (de){
            fn(privdata, de);
            de = de->next;
        }

        do{
            de = t1->table[v & m1];
            while (de){
                fn(privdata, de);
                de = de->next;
            }

            v = (((v | m0) + 1) & ~m0) | (v & m0);
        }while(v & (m0 ^ m1));
        
    }

    v |= ~m0;
    v = rev(v);
    v++;
    v = rev(v);

    return v;
}

void dict::dictEmpty(void(callback)(void*)){
    _dictClear(&(ht[0]), callback);
    _dictClear(&(ht[1]), callback);
    rehashidx = -1;
    iterators = 0;
}


int dict::_dictExpandIfNeeded(){
    if (rehashidx != -1) return DICT_OK;

    if (ht[0].size == 0) return dictExpand(DICT_HT_INITIAL_SIZE);

    if (ht[0].used >= ht[0].size && (dict_can_resize || ht[0].used/ht[0].size > dict_force_resize_ratio)){
        return dictExpand(ht[0].used * 2);
    }
    return DICT_OK;
}

int dict::_dictKeyIndex(StringObject *key){
    unsigned int h, idx, table;
    dictEntry *he;

    if (_dictExpandIfNeeded() == DICT_ERR) return -1;

    h = key->hash();

    for (table = 0; table <= 1; table++){
        idx = h & ht[table].sizemask;

        he = ht[table].table[idx];
        // if (table == 1 && he) cout << *(he->key->get_value());
        while (he){
            // cout << "table num: "<< table << " curidx: " << idx << endl;
            if (he->key->match(key) == 0) return -1;
            he = he->next;
        }
        if (rehashidx == -1) break;
        // cout << "because rehashing so table+1" << endl;
    }

    // cout << "ultimate insert table: " << table << " index: " << idx << endl;
    return idx;
}

dictEntry* dict::dictGetRandomKey(){
    dictEntry *he, *orighe;
    unsigned int h;
    int listlen, listele;

    if (length() == 0) return nullptr;

    if (rehashidx != -1) _dictRehashStep();

    if (rehashidx != -1){
        do{
            h = random() % (ht[0].size + ht[1].size);
            he = (h >= ht[0].size)? ht[1].table[h - ht[0].size]: ht[0].table[h];
        }while (he == nullptr);
    }else{
        do{
            h = random() & ht[0].sizemask;
            he = ht[0].table[h];
        }while(he == nullptr);
    }
    listlen = 0;
    orighe = he;
    while (he){
        he = he->next;
        listlen++;
    }
    listele = random() % listlen;

    he = orighe;

    while (listele--) he = he->next;
    return he;

}


int dict::dictGetRandomKeys(dictEntry **des, int count){
    int stored = 0;
    if (length() < count) count = length();
    while (stored < count){
        for(int j = 0; j < 2; j++){
            unsigned int i = random() & ht[j].sizemask;
            int size = ht[j].size;
            while (size--){
                dictEntry *he = ht[j].table[i];
                while (he){
                    *des = he;
                    des++;
                    he = he->next;
                    stored++;
                    if (stored == count) return stored;
                }
                i = (i+1) & ht[j].sizemask;
            }
        }
    }
    return stored;
}