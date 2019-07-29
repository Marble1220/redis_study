#include "skiplist.h"



skiplistNode* skiplist::slInsert(double score, BaseObject *obj){
    skiplistNode *update[SKIPLIST_MAXLEVEL], *x;
    unsigned int rank[SKIPLIST_MAXLEVEL];
    int i, cur_level;
    

    x = header;
    for (i = level-1; i>=0; i--){
        // rank累加
        rank[i] = (i == (level-1)? 0: rank[i+1]);
        // 为啥double直接比较了 （虽然并不是强制要求）
        while (x->level[i].forward && 
            (x->level[i].forward->score < score || (
                x->level[i].forward->score == score &&
                x->level[i].forward->obj->match(obj) < 0))){

            rank[i] += x->level[i].span;    // 记录跨越了多少节点
            x = x->level[i].forward;
            }
        update[i] = x;
    }

    cur_level = slRandomLevel();

    if (cur_level > level){
        for (i = level; i < cur_level; i++){
            rank[i] = 0;
            update[i] = header;
            update[i]->level[i].span = _length;
        }
        level = cur_level;
    }

    x = new skiplistNode(cur_level, score, obj);

    for (i = 0; i < cur_level; i++){
        x->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = x;
        x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;

    }

    for (i = cur_level; i < level; i++){
        update[i]->level[i].span++;
    }

    x->backward = (update[0] == header)? nullptr: update[0];
    if (x->level[0].forward){
        x->level[0].forward->backward = x;
    }
    else
        tail = x;
    
    _length ++;
    return x;
}

void skiplist::slDeleteNode(skiplistNode *x, skiplistNode **update){
    for (int i = 0; i < level; i++){
        if (update[i]->level[i].forward == x){
            update[i]->level[i].span += x->level[i].span - 1;
            update[i]->level[i].forward = x->level[i].forward;
        }else{
            update[i]->level[i].span -= 1;
        }
    }

    if (x->level[0].forward){
        x->level[0].forward->backward = x->backward;
    }else{
        tail = x->backward;
    }

    while (level > 1 && header->level[level-1].forward == nullptr){
        level--;
    }
    _length--;
}


int skiplist::slDelete(double score, BaseObject *obj){
    skiplistNode *update[SKIPLIST_MAXLEVEL], *x;
    x = header;

    for (int i = level-1; i >= 0; i--){
        while (x->level[i].forward && (x->level[i].forward->score <score || (x->level[i].forward->score == score && x->level[i].forward->obj->match(obj) < 0))){
            // cout << *(x->level[i].forward->obj->get_value()) << endl;
            x = x->level[i].forward;
        }
        update[i] = x;
    }

    x = x->level[0].forward;

    if (x && score == x->score && (x->obj->match(obj) == 0)){
        slDeleteNode(x, update);
        delete x;
        return 1;
    
    }else{
        return 0;
    }
    return 0;
}

int skiplist::slIsInRange(rangespec *range){
    if (range->min > range->max || (range->min == range->max && (range->minex || range->maxex))){
        return 0;
    }

    skiplistNode *x;
    x = tail;
    if (x == nullptr || !slValueGteMin(x->score, range))
        return 0;
    x = header->level[0].forward;
    if (x == nullptr || !slValueLteMax(x->score, range))
        return 0;
    return 1;
}

skiplistNode* skiplist::slFirstInRange(rangespec *range){
    skiplistNode *x;
    if (!slIsInRange(range)) return nullptr;
    x = header;
    
    for (int i = level-1; i >= 0; i--){
        while (x->level[i].forward && !slValueGteMin(x->level[i].forward->score, range)){
            x = x->level[i].forward;
        }
    }

    x = x->level[0].forward;
    if (x == nullptr || !slValueLteMax(x->score, range)) return nullptr;
    return x;
}

skiplistNode* skiplist::slLastInRange(rangespec *range){
    skiplistNode *x;
    if (!slIsInRange(range)) return nullptr;
    x = header;
    for (int i = level-1; i >= 0; i--){
        while (x->level[i].forward && slValueLteMax(x->level[i].forward->score, range)){
            x = x->level[i].forward;
        }
    }
    if (x == nullptr || !slValueGteMin(x->score, range)) return nullptr;
    return x;
}

unsigned long skiplist::slGetRank(double score, BaseObject *obj){
    skiplistNode *x;

    unsigned long rank = 0;
    cout << "test get rank: " << endl;
    x = header;

    for (int i = level-1; i >= 0; i--){
        cout << "cyle for is " << i << endl;
        while (x->level[i].forward && (x->level[i].forward->score <score || (x->level[i].forward->score == score && x->level[i].forward->obj->match(obj) <= 0))){
            cout << x->level[i].forward->score << endl;
            rank += x->level[i].span;
            x = x->level[i].forward;
        }
    if (x->obj && (x->score == score) && (x->obj->match(obj) == 0)) {
        cout << x->score << " " << *(x->obj->get_value()) << endl;
        return rank;
        }
    }
    return 0;
}

skiplistNode* skiplist::slGetElementByRank(unsigned long rank){
    skiplistNode *x;

    unsigned long traversed = 0;

    x = header;
    for (int i = level-1; i >= 0; i--){
        while (x->level[i].forward && (traversed + x->level[i].span) <= rank){
            traversed += x->level[i].span;
            x = x->level[i].forward;
        }

        if (traversed == rank){
            return x;
        }
    }

    return nullptr;
}




int slRandomLevel(void){
    int level = 1;
    while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF)){
        level += 1;
    }
    return (level < SKIPLIST_MAXLEVEL)? level: SKIPLIST_MAXLEVEL;
}


