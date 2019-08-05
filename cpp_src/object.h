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
    StringObject *none, *wrap, 
    
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
        sdshdr* ListObjectPop(int where);

        // nums为删除的数量， 为负数从尾开始， 正数从头开始， 0全部删除
        // 返回成功删除的数量
        int ListObjectRem(int nums, BaseObject* value);






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