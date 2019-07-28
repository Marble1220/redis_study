#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include <memory>

#include "base.h"
#include "object.h"

#define SKIPLIST_MAXLEVEL 32
#define SKIPLIST_P 0.25
static int slValueGteMin(double value, rangespec *spec) {
    return spec->minex ? (value > spec->min) : (value >= spec->min);
}
static int slValueLteMax(double value, rangespec *spec) {
    return spec->maxex ? (value < spec->max) : (value <= spec->max);
}



int slRandomLevel(void);

class skiplistNode;

class skiplistLevel{
    friend class skiplistNode;
    friend class skiplist;
    private:
        skiplistNode *forward;  // 前进指针
        unsigned int span;      // 跨度
    public:
        skiplistLevel(): forward(nullptr), span(0) {};
};


class skiplistNode{
    friend class skiplist;

    private:
        BaseObject *obj;
        double score;
        skiplistNode *backward;
        skiplistLevel *level;

    public:
        skiplistNode(int _level, double _score, BaseObject *_obj): score(_score), obj(_obj){
            this->level = new skiplistLevel[_level];
        };
        
        ~skiplistNode(){
            decrRefCount(obj);    
            delete[] level;
        };




};


class skiplist: public BaseStruct{
    private:
        skiplistNode *header, *tail;
        unsigned long _length;
        int level;

    public:
        skiplist(): level(1), _length(0){
            header = new skiplistNode(SKIPLIST_MAXLEVEL, 0, nullptr);
            tail = nullptr;
        }

        ~skiplist(){
            skiplistNode *node = header->level[0].forward, *next;
            delete header;

            while (node){
                next = node->level[0].forward;
                delete node;
                node = next;
            }   
        }

        size_t length(){return _length;};
        
        
        // 创建一个成员为obj 分值为score的新节点并插入 返回生成的节点
        skiplistNode *slInsert(double score, BaseObject *obj);
        // 内部删除函数 调整被删除节点相关的level和forward等等
        void slDeleteNode(skiplistNode *x, skiplistNode **update);
        // 删除包含给定score并带有指定object的节点
        int slDelete(double score, BaseObject *obj);
        //如果给定的分值范围包含在跳跃表的分值范围之内返回1 否则返回0
        int slIsInRange(rangespec *range);
        // 返回第一个分值符合range范围的节点， 如果没有返回nullptr
        skiplistNode* slFirstInRange(rangespec *range);
        // 返回最后一个分值范围符合range中的节点
        skiplistNode* slLastInRange(rangespec *range);
        // 查找给定分值和成员对象的节点在跳跃表中的排位 没有返回0
        unsigned long slGetRank(double score, BaseObject*);
        // 根据排位在跳跃表中查找元素， rank起始为1， 没有返回nullptr
        skiplistNode* slGetElementByRank(unsigned long rank);



};



#endif