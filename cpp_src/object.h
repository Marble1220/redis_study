#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "sds.h"

// 基本类型分类底层数据结构(BaseStruct)和对象(BaseObject)
// 所有对象都是BaseObject的直接派生类, 内部保存所使用的底层数据结构类和其信息以及自己的类别信息
// 基类定义统一接口, 子类根据情况实现
// 数据结构由BaseStruct派生， BaseStruct的直接派生类为对应的对象可能使用的数据结构
// 例如set对象可能使用intset或dict， 则BaseStruct派生SetStruct， 在SetStruct中定义set对象需要的接口
// 实际的实现类(inset和dict)继承SetStruct, 实现方法
// 在Set类中定义SetStruct成员使用即可 

#define TESTOBJECT 20



class BaseObject{
    friend class TestObject;
    protected:
        BaseStruct* ptr;
        
    public:

        int object_type;
        int ref;
        BaseObject() = default;
        BaseObject(int type): object_type(type), ref(1){};

        virtual ~BaseObject(){delete ptr;};

        virtual sdshdr* get_value() const = 0;

        // 类型不同的比较报错 大于返回1等于返回0小于返回-1
        virtual int match(BaseObject*) const = 0;
        
};



class TestObject: public BaseObject{
    public:
        TestObject(const char *s, size_t len):BaseObject(TESTOBJECT){
            ptr = new sdshdr(s, len);
        }

        TestObject(const char *s): TestObject(s, strlen(s)) {};

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

static void decrRefCount(BaseObject*){};     // 引用计数-1 如果为0 回收








#endif