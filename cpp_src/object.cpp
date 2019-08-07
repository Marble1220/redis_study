#include "object.h"

struct sharedObjectsStruct shared;
void decrRefCount(BaseObject *obj){
    if (obj->ref <= 0) loge("object refcount <= 0\n");
    if (obj->ref == 1){
        delete obj;
        obj = nullptr;
    }
    else{
        obj->ref--;
    }

}

void incrRefCount(BaseObject *obj){
    obj->ref++;
}


std::string strEncoding(int encoding){
    switch(encoding){
        case SDSSTRUCT: return "sds";
        case INTSETSTRUCT: return "intset";
        case ADLISTSTRUCT: return "linkedlist";
        case SKIPLISTSTRUCT: return "skiplist";
        case DICTSTRUCT: return "dict";
        case INTSTRUCT: return "int";
        default: return "unknown";
    }
}

unsigned long long estimateObjectIdleTime(BaseObject *obj){
    unsigned long long lruclock = getLRUClock();
    if (lruclock >= obj->lru){
        return (lruclock - obj->lru) * LRU_CLOCK_RESOLUTION;
    }else{
        return (lruclock + (LRU_CLOCK_MAX - obj->lru)) * LRU_CLOCK_RESOLUTION;
    }
}

void createSharedObjects(){
    shared.none = new StringObject("none\r\n");
    shared.wrap = new StringObject("\r\n");
    shared.space = new StringObject(" ");

    for (int i = 0; i < SHARED_INTEGERS; i++){
        shared.integers[i] = new StringObject((long long) i);
        
    }
}