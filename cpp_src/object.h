#include "sds.h"

// 基本类型分类底层数据结构(BaseStruct)和对象(BaseObject)
// 所有对象都是BaseObject的直接派生类, 内部保存所使用的底层数据结构类和其信息以及自己的类别信息
// 基类定义统一接口, 子类根据情况实现
// 数据结构由BaseStruct派生， BaseStruct的直接派生类为对应的对象可能使用的数据结构
// 例如set对象可能使用intset或dict， 则BaseStruct派生SetStruct， 在SetStruct中定义set对象需要的接口
// 实际的实现类(inset和dict)继承SetStruct, 实现方法
// 在Set类中定义SetStruct成员使用即可 





class BaseObject{
    private:
        uint8_t object_type;
        
    public:
        BaseObject() = default;

        virtual ~BaseObject() = default;

        virtual sdshdr* get_value() const = 0;

        virtual bool match(BaseObject*) const = 0;
};