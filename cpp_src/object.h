#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdint.h>
#include <sstream>
#include <string>

#include "log.h"
#include "sds.h"



// 基本类型分类底层数据结构(BaseStruct)和对象(BaseObject)
// 所有对象都是BaseObject的直接派生类, 内部保存所使用的底层数据结构类和其信息以及自己的类别信息
// 基类定义统一接口, 子类根据情况实现
// 数据结构由BaseStruct派生， BaseStruct的直接派生类为对应的对象可能使用的数据结构
// 例如set对象可能使用intset或dict， 则BaseStruct派生SetStruct， 在SetStruct中定义set对象需要的接口
// 实际的实现类(inset和dict)继承SetStruct, 实现方法
// 在Set类中定义SetStruct成员使用即可 

#define TESTOBJECT 20
#define STRINGOBJECT 25
#define LISTOBJECT 26
#define SETOBJECT 27
#define HASHOBJECT 28
#define SSETOBJECT 29

static uint32_t hash_function_seed = 5381;
//共享对象
class StringObject;
struct sharedObjectsStruct{
    StringObject *none, *wrap, *space, 
    
    *integers[SHARED_INTEGERS];
};

extern struct sharedObjectsStruct shared;
void createSharedObjects();



class BaseObject{
    friend class TestObject;
    friend class StringObject;
    // protected:
        
    public:
        BaseStruct* ptr;

        int object_type;
        int ref;
        unsigned lru;
        BaseObject() = default;
        BaseObject(int type): object_type(type), ref(1), lru(getLRUClock()){};

        virtual ~BaseObject(){cout << "awsl" << endl;};

        virtual sdshdr* get_value() const = 0;

        // 类型不同的比较报错 大于返回1等于返回0小于返回-1
        virtual int match(BaseObject*) const = 0;

        virtual unsigned int hash() const = 0;

};





class StringObject: public BaseObject{
    public:
        StringObject(const char *s, size_t len):BaseObject(STRINGOBJECT){
            ptr = new sdshdr(s, len);
        }

        StringObject(const char *s): StringObject(s, strlen(s)) {};
        StringObject(long long value): BaseObject(STRINGOBJECT) {
            ptr = new sdshdr(value);
        };
        StringObject(long double value): BaseObject(STRINGOBJECT){
            std::stringstream ss;
            ss << value;
            const char *s = ss.str().c_str();
            ptr = new sdshdr(s);
        }

        ~StringObject(){
            // cout << get_value()->get_buf() << endl;
            delete ptr;
        }


        sdshdr* get_value() const;

        int match(BaseObject* other) const;

        // 得到一个自己的复制， 总是产生一个非共享对象， refcot总为1
        StringObject* dupStringObject();

        // 返回值:相等返回 0 ，自己较大返回正数， 另一个较大返回负数
        // 二进制安全
        // 突然发现之前写了match
        // int compareStringObject(BaseObject *other);

        // 返回长度
        size_t length();

        long getlongintfromstring();

        int StringObjectcat(sdshdr*);

        sdshdr* getrangefromStringObject(int start, int end);


        unsigned int hash() const ;
};




class ListObject: public BaseObject{
    public:
        ListObject();
        ~ListObject();



        sdshdr* get_value() const;
        int match(BaseObject*) const;
        unsigned hash() const;

        // where 为0添加到头， 为1添加到尾
        int ListObjectPush(BaseObject *value, int where);

        // 返回index位置的值, 如果为空返回nullptr
        sdshdr* ListObjectIndex(int index);

        // 将index处的值设置为value， 成功返回py_ok, 失败返回py_err
        int ListObjectSet(int index, BaseObject* value);

        // 弹出元素， where决定从表头还是表尾
        // 调用者负责回收返回sdshdr的内存, 返回none也要释放内存
        sdshdr* ListObjectPop(int where);

        // nums为删除的数量， 为负数从尾开始， 正数从头开始， 0全部删除
        // 返回成功删除的数量
        int ListObjectRem(int nums, BaseObject* value);

};


#define SET_INTSET_MAX 512

class SetObject: public BaseObject{
    public:
        SetObject(BaseObject* value);
        ~SetObject();

        sdshdr* get_value() const;
        int match(BaseObject* ) const;
        unsigned hash() const;

        // 判断value是否存在与集合， 存在返回1, 不在返回0
        int SetObjectExist(BaseObject* value);
        // 将value添加到set中， 成功返回py_ok， 失败返回py_err
        int SetObjectAdd(BaseObject* value);
        // 返回保存的元素个数
        int SetObjectLen();
        // 删除set中给定的元素, 成功返回py_ok, 失败返回py_err
        int SetObjectRem(BaseObject* value);
        // 以sdshdr的形式随机返回并删除一个保存的元素
        // 为空返回shared.none, 失败返回nullptr
        // 调用者负责回收返回sdshdr的内存, 返回none也要释放内存
        sdshdr* SetObejctPop();

    private:

        //将类型从整数集合升级为dict
        void SetObjectUpgrade();
        
};

class dictIterator;
class dictEntry;
class HashObject: public BaseObject{
    private:
        dictIterator *keyiter;
        dictIterator *valiter;
    public:
        HashObject();
        ~HashObject();


        unsigned int hash() const;

        sdshdr* get_value() const;

        int match(BaseObject*) const;


        // 返回key对应的v, key不存在返回shared.none;
        BaseObject* HashGet(StringObject* key);

        //判断key的是否在hash中， 存在返回1 不存在返回0
        int HashExists(StringObject* key);
        
        // 将给定的k, v设置到hash
        // 如果存在旧值， 旧值将被覆盖
        // 如果是新添加返回1， 存在旧值返回0
        int HashSet(StringObject* key, BaseObject* value);

        // 删除给定的key所在的dictentry， 成功返回PY_OK, 因不存在而失败返回py_err
        int HashDel(StringObject* key);

        int HashLen();

        // 以下两个函数分别返回hash全部的key或value
        // 这两个函数有一个共同特点
        // 一般来说， 起将必定会被调用多次
        // 每次返回hash中的一个对应元素(k or v)
        // 直到迭代完毕返回nullptr
        // 注意  线程不安全, 如果两个线程同时调用， 则不保证返回结果正确(基本肯定不正确)
        // 可以设置dict保存对应线程的迭代信息(暂时先不弄了)
        sdshdr* HashKeys();
        sdshdr* HashValues();

    private:
        // 传入一个iter迭代器指针, 处理对应, 返回sdshdr*或nullptr
        dictEntry* _HashIter(dictIterator* &);
};





void decrRefCount(BaseObject*);     // 引用计数-1 如果为0 回收
void incrRefCount(BaseObject*);

//返回编码的字符串表示

std::string strEncoding(int encoding);

// 返回给定对象的空转时长
unsigned long long estimateObjectIdleTime(BaseObject *obj);




















class TestObject: public BaseObject{
    public:
        TestObject(const char *s, size_t len):BaseObject(TESTOBJECT){
            ptr = new sdshdr(s, len);
        }

        TestObject(const char *s): TestObject(s, strlen(s)) {};
        TestObject(long long value): BaseObject(TESTOBJECT) {
            ptr = new sdshdr(value);
        };

        sdshdr* get_value() const {
            return static_cast<sdshdr*>(ptr);
        }

        int match(BaseObject* other) const {
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
};



#endif