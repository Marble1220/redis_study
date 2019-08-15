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

std::string strObjecttype(int type_){
    switch(type_){
        case STRINGOBJECT: return "STRING";
        case LISTOBJECT: return "LIST";
        case SETOBJECT: return "SET";
        case HASHOBJECT: return "HASH";
        case ZSETOBJECT: return "ZSET";
        default: return "UNKNOWN";
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
    shared.err = new StringObject("err\r\n");
    shared.ok = new StringObject("ok\r\n");
    shared.notexist = new StringObject("key not exist\r\n");
    shared.typrerr = new StringObject("type err\r\n");
    shared.exist = new StringObject("value already exists\r\n");
    shared.unerr = new StringObject("unknown error\r\n");
    shared.rangeout = new StringObject("range out\r\n");

    for (int i = 0; i < SHARED_INTEGERS; i++){
        shared.integers[i] = new StringObject((long long) i);
    }
}