#include <stdint.h>
#ifndef __DICT_H
#define __DICT_H

#include <sys/time.h>
#include <limits.h>

#include "object.h"


#define DICT_OK 0
#define DICT_ERR 1
#define DICT_HT_INITIAL_SIZE 4

class dictEntry;
class dictIterator;
typedef void (dictScanFunction)(BaseObject *privdata, const dictEntry *de);


// 计算第一个大于等于 size 的 2 的 N 次方，用作哈希表的值

// 指示字典是否启用 rehash 的标识
static int dict_can_resize = 1;
// 强制 rehash 的比率
static unsigned int dict_force_resize_ratio = 5;

// 开启自动 rehash
void dictEnableResize(void);

// 关闭自动 rehash
void dictDisableResize(void);

static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size > LONG_MAX){
        return LONG_MAX;
    }
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

class dictEntry{
    public:
        StringObject *key;
        BaseObject *v;

        dictEntry *next;

        dictEntry(StringObject *key_, BaseObject *v_): key(key_), v(v_) {};
        dictEntry(StringObject *key_): dictEntry(key_, nullptr) {};
        dictEntry(): dictEntry(nullptr) {};

        ~dictEntry(){
            delete key; 
            key = nullptr;
            if (v != nullptr){
            delete v;
            v = nullptr;
            }
        };
};

class dictht{
    friend class dict;
    friend class dictEntry;
    public:
        dictEntry **table;        
        unsigned long size;
        unsigned long sizemask; // 掩码
        unsigned long used; // 已有节点数量

        dictht():table(nullptr), size(0), sizemask(0), used(0){};

    private:
        void dictReset();// 初始化hashtable各项的值


};

class dict: public BaseStruct{
    friend class dictIterator;
    public:
        BaseObject* privdata; //私有数据
        dictht ht[2];
        int rehashidx;
        int iterators; 

        dict(BaseObject* privdataptr):BaseStruct(DICTSTRUCT), privdata(privdataptr), rehashidx(-1), iterators(0){
            ht[0] = dictht();
            ht[1] = dictht();
        }
        dict(): dict(nullptr) {};

        ~dict(){
            _dictClear(&ht[0], NULL);
            _dictClear(&ht[1], NULL);
            if (privdata != nullptr) delete privdata;
        }

        size_t length() const {return ht[0].used + ht[1].used;};

        // 缩小给定字典 返回DICT_ERR标示字典已经在rehash， 或dict_can_resize为false
        // 成功创建体积更小的ht[1] 可以开始resize时返回DICT_OK
        int dictResize();

        // 创建一个新的哈希表，并根据字典的情况，选择以下其中一个动作来进行：
        
        // 1) 如果字典的 0 号哈希表为空，那么将新哈希表设置为 0 号哈希表
        // 2) 如果字典的 0 号哈希表非空，那么将新哈希表设置为 1 号哈希表，
        //    并打开字典的 rehash 标识，使得程序可以开始对字典进行 rehash
        //    size 参数不够大，或者 rehash 已经在进行时，返回 DICT_ERR 。

        //    成功创建 0 号哈希表，或者 1 号哈希表时，返回 DICT_OK 。
        int dictExpand(unsigned long size);

        // 执行 N 步渐进式 rehash 。

        // 返回 1 表示仍有键需要从 0 号哈希表移动到 1 号哈希表，
        // 返回 0 则表示所有键都已经迁移完毕。
        int dictRehash(int n);

        //在给定毫秒内 以100步为单位对dict进行rehash
        int dictRehashMillliseconds(int ms);

        //在字典不存在安全迭代器的情况下进行单步rehash
        // 字典有安全迭代器的情况下不能rehash

        void _dictRehashStep();

        // 讲给定kv添加到dict中， 只有key不存在于dict时才会成功
        // 成功返回DICT_OK 失败返回DICT_ERR
        int dictAdd(StringObject *key, BaseObject* val);

        //尝试将key插入字典， 如果key已存在返回nullptr
        // 如果不存在， 则新建dictEntry将其和key关联，并插入字典， 然后返回节点的ptr
        dictEntry* dictAddRaw(StringObject *key);

        // 将kv添加到dict中， 如果k已存在， 删除旧有的
        // 如果为全新添加返回1， 如果更新原有kv返回0
        int dictReplace(StringObject *key, BaseObject *val);


        // dictReplaceRaw() 根据给定 key 释放存在，执行以下动作：
       
        // 1) key 已经存在，返回包含该 key 的字典节点
        // 2) key 不存在，那么将 key 添加到字典
        
        // 不论发生以上的哪一种情况，
        // dictReplaceRaw() 都总是返回包含给定 key 的字典节点。

        dictEntry* dictReplaceRaw(StringObject* key);

        // 查找并删除包含给定k的节点
        // nofree决定是否调用kv的delete， 0为调用 1为不调用
        // 成功找到并删除返回DICT_OK 没找到返回DICT_ERR
        int dictGenericDelete(StringObject *key, int nofree);
        int dictDelete(StringObject *key);
        int dictDeleteNoFree(StringObject *key);

        // 删除hash表上的所有节点， 并且重置hash表的属性
        int _dictClear(dictht *ht, void(callback)(void*));

        //返回字典中包含k的节点
        // 找不到返回nullptr

        dictEntry* dictFind(StringObject* key);


        // 获取包含key的节点的值
        // 如果为空返回nullptr
        BaseObject* dictFetchValue(StringObject *key);

        // 返回dict的指纹
        long long dictFingerprint();

        // 创建并返回字典的不安全迭代器
        dictIterator* dictGetIterator();

        // 创建并返回字典的不安全迭代器
        dictIterator* dictGetSafeIterator();

        // dictScan() 函数用于迭代给定字典中的元素
        // 迭代按以下方式执行

        // 一开始，你使用 0 作为游标来调用函数
        //  函数执行一步迭代操作，
        //  并返回一个下次迭代时使用的新游标
        //  当函数返回的游标为 0 时，迭代完成。

        //  函数保证，在迭代从开始到结束期间，一直存在于字典的元素肯定会被迭代到，
        //  但一个元素可能会被返回多次。
        //  每当一个元素被返回时，回调函数 fn 就会被执行
        //  fn 函数的第一个参数是 privdata ，而第二个参数则是字典节点 de
        unsigned long dictScan(unsigned long v, dictScanFunction *fn, BaseObject *privdata);

        // 清空字典上的所有哈希表节点 并且重置字典属性
        void dictEmpty(void(callback)(void*));




    private:
        // 根据需要，初始化字典（的哈希表），或者对字典（的现有哈希表）进行扩展
        int _dictExpandIfNeeded();

        // 返回可以将key插入到hash表的索引位置
        // 如果key已存在返回-1

        // 如果正在进行rehash 总是返回1号表的索引（在进行rehash时新节点总是插入到1号表）
        int _dictKeyIndex(StringObject* key);






};

class dictIterator{
    public:
        dict *d;
        int table, index, safe; // 正在被使用的hashtable号码（0 or1） hashtable索引 安全标识
        dictEntry *entry, *nextentry; //entry 当前迭代到节点的指针 nextentry 下一个

        long long fingerprint;

        dictIterator(dict* d_, int safe_): d(d_), table(0), index(-1), safe(safe_), entry(nullptr), nextentry(nullptr){};

        ~dictIterator(){
            if (!(index == -1 && table == 0)){
                if (safe) d->iterators--;
                else assert(fingerprint == d->dictFingerprint());
            }
        }
        
        
        // 返回迭代器指向的当前节点
        // 迭代完毕返回nullptr

        dictEntry* dictNext();

        

};







#endif
