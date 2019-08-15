#include "db.h"
#include "python3.6m/Python.h"

static db* _db = new db();
static StringObject *temp_str1, *temp_str2, *temp_str3, *temp_str4;

static PyObject* py_init(PyObject *self, PyObject *args){
    createSharedObjects();
    return PyUnicode_FromString(shared.integers[1]->get_value()->get_buf());
}

static PyObject* py_set(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    int temp3;
    if (!PyArg_ParseTuple(args, "sy#", &temp1, &temp2, &temp3)) return nullptr;
    temp_str1 = new StringObject(temp1);
    // cout << temp3 << endl;

    temp_str2 = new StringObject(temp2, temp3);
    _db->set_value(temp_str1, STRINGOBJECT, temp_str2);
    decrRefCount(temp_str1);
    decrRefCount(temp_str2);
    
    return PyUnicode_FromString(shared.integers[1]->get_value()->get_buf());
}

static PyObject* py_get(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto temp_obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (temp_obj){
        if (temp_obj->object_type == STRINGOBJECT)
            return PyUnicode_FromStringAndSize(temp_obj->get_value()->get_buf(), temp_obj->get_value()->length());
        auto temp_sds = temp_obj->get_value();
        auto py_res = PyUnicode_FromStringAndSize(temp_sds->get_buf(), temp_sds->length());
        delete temp_sds;
        return py_res;
    }
    return PyUnicode_FromString(shared.none->get_value()->get_buf());
}

static PyObject* py_exists(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    int res = _db->exists(temp_str1);
    decrRefCount(temp_str1);
    return PyLong_FromLong(res);
}

static PyObject* py_del(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1))  return nullptr;
    temp_str1 = new StringObject(temp1);
    int res = _db->delete_key(temp_str1);
    decrRefCount(temp_str1);
    return PyLong_FromLong(res);
}


static PyObject* py_rename(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto val = _db->get_value(temp_str1);
    if (!val) {
        decrRefCount(temp_str1);
        return PyUnicode_FromString(shared.err->get_value()->get_buf());
    }
    temp_str2 = new StringObject(temp2);

    _db->delete_key(temp_str2);


    auto dentry = _db->val->dictAddRaw(temp_str2);
    incrRefCount(temp_str2);

    dentry->v = val;
    incrRefCount(val);

    _db->delete_key(temp_str1);

    decrRefCount(temp_str1);
    decrRefCount(temp_str2);
    
    return PyUnicode_FromString(shared.ok->get_value()->get_buf());
}

static PyObject* py_type(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    int target_type = _db->get_type(temp_str1);
    decrRefCount(temp_str1);


    if (!target_type) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());

    std::string _type = strObjecttype(target_type);
    return PyUnicode_FromString(_type.c_str());

}

static PyObject* py_getset(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "sy", &temp1, &temp2)) return nullptr;
    
    PyObject *res;
    temp_str1 = new StringObject(temp1);
    temp_str2 = new StringObject(temp2);
    auto dentry = _db->val->dictFind(temp_str1);
    if (dentry){
        auto old_val = dentry->v;

        if (old_val){
            if (old_val->object_type == STRINGOBJECT) res = PyUnicode_FromString(old_val->get_value()->get_buf());
            else{
                sdshdr* temp_sds = old_val->get_value();
                res = PyUnicode_FromString(temp_sds->get_buf());
                delete temp_sds;
            }
            decrRefCount(old_val);
        }
        else{
            res = PyUnicode_FromString(shared.none->get_value()->get_buf());
        }
        dentry->v = temp_str2;
        incrRefCount(temp_str2);
    }
    else{
        res = PyUnicode_FromString(shared.none->get_value()->get_buf());
        _db->set_value(temp_str1, STRINGOBJECT, temp_str2);
    }
    decrRefCount(temp_str1);
    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_getrange(PyObject *self, PyObject *args){
    char *temp1;
    int temp2, temp3;
    if (!PyArg_ParseTuple(args, "sii", &temp1, &temp2, &temp3)) return nullptr;
    if (temp3 >=0) temp3 -= 1;
    if (temp2 < 0) temp2 += 1;
    PyObject *res;

    temp_str1 = new StringObject(temp1);
    auto val = _db->get_value(temp_str1);
    if (!val) {
        decrRefCount(temp_str1);
        return PyUnicode_FromString(shared.none->get_value()->get_buf());
    }
    if (val->object_type != STRINGOBJECT){
        decrRefCount(temp_str1);
        return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    }
    sdshdr* temp_sds = static_cast<StringObject*>(val)->getrangefromStringObject(temp2, temp3);
    if (!temp_sds) {
        decrRefCount(temp_str1);
        return PyUnicode_FromString("wrong range");
    }

    res = PyUnicode_FromString(temp_sds->get_buf());
    
    delete temp_sds;
    decrRefCount(temp_str1);
    return res;
}

static PyObject* py_strlen(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    PyObject *res;

    temp_str1 = new StringObject(temp1);

    auto val = _db->get_value(temp_str1);
    if (!val) return PyLong_FromLong(-1);
    if (val->object_type != STRINGOBJECT){ 
        decrRefCount(temp_str1);
        return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    }
    res = PyLong_FromLong(val->get_value()->length());
    return res;
}
// 对于set的sadd, scard, spop, srem

static PyObject* py_sadd(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;

    temp_str1 = new StringObject(temp1);
    temp_str2 = new StringObject(temp2);
    auto val = _db->get_value(temp_str1);
    if (!val) val = _db->set_value(temp_str1, SETOBJECT, temp_str2);
    if (val->object_type != SETOBJECT) {
        decrRefCount(temp_str1);
        decrRefCount(temp_str2);
        return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    }
    auto target_set = dynamic_cast<SetObject*> (val);
    PyObject *res;
    if ((target_set->SetObjectAdd(temp_str2)) == PY_EXIST){
        res = PyUnicode_FromString(shared.exist->get_value()->get_buf());
    }
    else{
        res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    }

    decrRefCount(temp_str1);
    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_scard(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto val = _db->get_value(temp_str1);
    decrRefCount(temp_str1);

    if (!val || val->object_type != SETOBJECT){
        return PyLong_FromLong(0);
    }

    PyObject *res = PyLong_FromLong(dynamic_cast<SetObject*>(val)->SetObjectLen());
    return res;
}


static PyObject* py_spop(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto val = _db->get_value(temp_str1);
    decrRefCount(temp_str1);

    if (!val) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    if (val->object_type != SETOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());

    sdshdr *temp_sds = dynamic_cast<SetObject*>(val)->SetObejctPop();
    PyObject *res;
    
    if (!temp_sds) return PyUnicode_FromString(shared.unerr->get_value()->get_buf());
    res = PyUnicode_FromString(temp_sds->get_buf());
    if (temp_sds != shared.none->get_value()) delete temp_sds;
    return res;
}

static PyObject* py_srem(PyObject *self, PyObject *args){
    char *temp1, *temp2;

    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;
    temp_str1 = new StringObject(temp1);
    // temp_str2 = new StringObject(temp2);
    auto val = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!val) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    if (val->object_type != SETOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    
    PyObject *res;
    auto target_set = dynamic_cast<SetObject*>(val);
    temp_str2 = new StringObject(temp2);
    
    if ((target_set->SetObjectRem(temp_str2)) == PY_ERR){
        res = PyUnicode_FromString("not exists");
    }
    else{
        res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    }
    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_expire(PyObject *self, PyObject *args){
    char *temp1;
    long temp2;
    if(!PyArg_ParseTuple(args, "sl", &temp1, &temp2)) return nullptr;
    PyObject *res;

    temp_str1 = new StringObject(temp1);
    if (_db->set_expires(temp_str1, temp2) == 1){
        res = PyUnicode_FromString("ok\r\n");
    }
    else{
        res = PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    }
    decrRefCount(temp_str1);
    return res;
}




// (3) 对于hash的hdel, hexists, hget, hlen, hkeys, hset, hvals
static PyObject* py_hset(PyObject *self, PyObject *args){
    char *temp1, *temp2, *temp3;
    if (!PyArg_ParseTuple(args, "sss", &temp1, &temp2, &temp3)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    if (!obj) obj = _db->set_value(temp_str1, HASHOBJECT, nullptr);
    if (obj->object_type != HASHOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    decrRefCount(temp_str1);

    temp_str2 = new StringObject(temp2);
    temp_str3 = new StringObject(temp3);

    auto hash_obj = dynamic_cast<HashObject*>(obj);

    hash_obj->HashSet(temp_str2, temp_str3);
    decrRefCount(temp_str2);
    decrRefCount(temp_str3);

    return PyUnicode_FromString(shared.ok->get_value()->get_buf());
}


static PyObject* py_hget(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2))return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    if (obj->object_type != HASHOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());

    temp_str2 = new StringObject(temp2);
    auto hash_obj = dynamic_cast<HashObject*>(obj);
    PyObject *res = PyUnicode_FromString(hash_obj->HashGet(temp_str2)->get_value()->get_buf());
    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_hdel(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    if (obj->object_type != HASHOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());

    PyObject *res;
    auto hash_obj = dynamic_cast<HashObject*>(obj);
    temp_str2 = new StringObject(temp2);
    if (hash_obj->HashDel(temp_str2) == PY_OK) res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    else res = PyUnicode_FromString(shared.none->get_value()->get_buf());

    decrRefCount(temp_str2);
    return res;

}

static PyObject* py_hexists(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != HASHOBJECT) return PyLong_FromLong(-1);
    
    PyObject *res;
    auto hash_obj = dynamic_cast<HashObject*>(obj);

    temp_str2 = new StringObject(temp2);

    if (hash_obj->HashExists(temp_str2)) res = PyLong_FromLong(1);
    else res = PyLong_FromLong(0);

    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_hlen(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != HASHOBJECT) return PyLong_FromLong(0);

    return PyLong_FromLong(dynamic_cast<HashObject*>(obj)->HashLen());
    
}


static PyObject* py_hkeys(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != HASHOBJECT) return PyUnicode_FromString(shared.none->get_value()->get_buf());

    auto hash_obj = dynamic_cast<HashObject*>(obj);
    auto key = hash_obj->HashKeys();
    if (!key) return PyLong_FromLong(-1);
    return PyUnicode_FromString(key->get_buf());
}

static PyObject* py_hvals(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != HASHOBJECT) return PyUnicode_FromString(shared.none->get_value()->get_buf());

    auto hash_obj = dynamic_cast<HashObject*>(obj);
    auto val = hash_obj->HashValues();
    if (!val) return PyLong_FromLong(-1);
    return PyUnicode_FromString(val->get_buf());
}

// (4) 对于list的lpush, rpush, llen, lpop, rpop, lindex, lrange, lrem, lset
static PyObject* py_lpush(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    if (!obj) {
        obj = _db->set_value(temp_str1, LISTOBJECT, nullptr);
    }
    if (obj->object_type != LISTOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    decrRefCount(temp_str1);

    PyObject *res;
    auto list_obj = dynamic_cast<ListObject*>(obj);
    temp_str2 = new StringObject(temp2);
    
    if (list_obj->ListObjectPush(temp_str2, 0) == PY_OK) res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    else res = PyUnicode_FromString(shared.unerr->get_value()->get_buf());

    decrRefCount(temp_str2);
    return res;
}

// 这两个函数完全可以抽象简化为一个函数(底层list就是如此) 但是为了保持后续python端的简单性选择分成两个
static PyObject* py_rpush(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;

    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    if (!obj) obj = _db->set_value(temp_str1, LISTOBJECT, nullptr);
    if (obj->object_type != LISTOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    decrRefCount(temp_str1);

    PyObject *res;
    auto list_obj = dynamic_cast<ListObject*>(obj);
    temp_str2 = new StringObject(temp2);
    
    if (list_obj->ListObjectPush(temp_str2, 1) == PY_OK) res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    else res = PyUnicode_FromString(shared.unerr->get_value()->get_buf());

    decrRefCount(temp_str2);
    return res;
}

PyObject* param_check(const char* keyname, int object_type, BaseObject *& obj){
    temp_str1 = new StringObject(keyname);
    obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj) return PyUnicode_FromString(shared.notexist->get_value()->get_buf());
    if (obj->object_type != object_type) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    return nullptr;
}

static PyObject* py_llen(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != LISTOBJECT) return PyLong_FromLong(0);

    auto list_obj = dynamic_cast<ListObject*>(obj);
    return PyLong_FromLong(list_obj->ptr->length());
}

static PyObject* py_lpop(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    BaseObject *obj;
    PyObject *res;
    if ((res = param_check(temp1, LISTOBJECT, obj))) return res;

    auto list_obj = dynamic_cast<ListObject*>(obj);
    sdshdr *temp_sds = list_obj->ListObjectPop(0);
    res = PyUnicode_FromString(temp_sds->get_buf());
    if (temp_sds != shared.none->get_value()) delete temp_sds;
    return res;
}
// 原因同上
static PyObject* py_rpop(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    BaseObject *obj;
    PyObject *res;
    if ((res = param_check(temp1, LISTOBJECT, obj))) return res;

    auto list_obj = dynamic_cast<ListObject*>(obj);
    sdshdr *temp_sds = list_obj->ListObjectPop(1);
    res = PyUnicode_FromString(temp_sds->get_buf());
    if (temp_sds != shared.none->get_value()) delete temp_sds;
    return res;
}


static PyObject* py_lindex(PyObject *self, PyObject *args){
    char *temp1;
    int temp2;
    if (!PyArg_ParseTuple(args, "si", &temp1, &temp2)) return nullptr;
    BaseObject *obj;
    PyObject *res;
    if ((res = param_check(temp1, LISTOBJECT, obj))) return res;

    auto list_obj = dynamic_cast<ListObject*>(obj);
    sdshdr *temp_sds = list_obj->ListObjectIndex(temp2);
    res = PyUnicode_FromString(temp_sds->get_buf());
    return res;
}

static PyObject* py_lrange(PyObject *self, PyObject *args){
    char *temp1;
    int temp2, temp3;
    if (!PyArg_ParseTuple(args, "sii", &temp1, &temp2, &temp3)) return nullptr;
    BaseObject *obj;
    PyObject *res;
    if ((res = param_check(temp1, LISTOBJECT, obj))) return res;
    
    auto list_obj = dynamic_cast<ListObject*>(obj);
    int len = list_obj->ptr->length();

    if (temp3 < 0) temp3 = len + temp3;
    if (temp2 < 0) temp2 = len + temp2;
    if (temp3 < 0 || temp2 >= temp3 || temp2 >= len || temp3 > len) return PyUnicode_FromString(shared.rangeout->get_value()->get_buf());

    std::string str;
    sdshdr *temp_sds;
    for (int i = temp2; i < temp3; i++){
        temp_sds = list_obj->ListObjectIndex(i);
        len = temp_sds->length();
        if (len < 10000){
            str +=shared.integers[len]->get_value()->get_buf();
        }
        else{
            auto temp_sds = sdshdr(len);
            str += temp_sds.get_buf();
        }
        str += '\n';
        str += temp_sds->get_buf();
    }
    res = PyUnicode_FromString(str.c_str());
    return res;
}

static PyObject* py_lrem(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    int temp3;
    if (!PyArg_ParseTuple(args, "ssi", &temp1, &temp2, &temp3)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != LISTOBJECT) return PyLong_FromLong(-1);

    PyObject *res;
    auto list_obj = dynamic_cast<ListObject*>(obj);
    temp_str2 = new StringObject(temp2);

    res = PyLong_FromLong(list_obj->ListObjectRem(temp3, temp_str2));
    decrRefCount(temp_str2);
    return res;
}


static PyObject* py_lset(PyObject *self, PyObject *args){
    char *temp1, *temp3;
    int temp2;
    if (!PyArg_ParseTuple(args, "sis", &temp1, &temp2, &temp3)) return nullptr;
    
    BaseObject *obj;
    PyObject *res;
    if((res = param_check(temp1, LISTOBJECT, obj))) return res;

    auto list_obj = dynamic_cast<ListObject*>(obj);
    temp_str2 = new StringObject(temp3);
    if (list_obj->ListObjectSet(temp2, temp_str2) == PY_OK) res = PyUnicode_FromString(shared.ok->get_value()->get_buf());
    else res = PyUnicode_FromString(shared.rangeout->get_value()->get_buf());
    
    decrRefCount(temp_str2);
    
    return res;
}


static PyObject* py_zadd(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    double temp3;
    if (!PyArg_ParseTuple(args, "ssd", &temp1, &temp2, &temp3)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    if (!obj) obj = _db->set_value(temp_str1, ZSETOBJECT, nullptr);
    if (obj->object_type != ZSETOBJECT) return PyUnicode_FromString(shared.typrerr->get_value()->get_buf());
    decrRefCount(temp_str1);

    temp_str2 = new StringObject(temp2);
    auto zset_obj = dynamic_cast<ZsetObject*>(obj);
    zset_obj->ZsetAdd(temp_str2, temp3);
    decrRefCount(temp_str2);
    return PyUnicode_FromString(shared.ok->get_value()->get_buf());
}

static PyObject* py_zcard(PyObject *self, PyObject *args){
    char *temp1;
    if (!PyArg_ParseTuple(args, "s", &temp1)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != ZSETOBJECT) return PyLong_FromLong(0);
    return PyLong_FromLong(dynamic_cast<ZsetObject*>(obj)->ZsetLen());
}

static PyObject* py_zcount(PyObject *self, PyObject *args){
    char *temp1;
    double temp2, temp3;
    if (!PyArg_ParseTuple(args, "sdd", &temp1, &temp2, &temp3)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != ZSETOBJECT) return PyLong_FromLong(0);
    rangespec range = {temp2, temp3, 0, 0};
    return PyLong_FromLong(dynamic_cast<ZsetObject*>(obj)->ZsetCount(range));
}

static PyObject* py_zrangebyscore(PyObject *self, PyObject *args){
    char *temp1;
    double temp2, temp3;
    if (!PyArg_ParseTuple(args, "sdd", &temp1, &temp2, &temp3)) return nullptr;
    PyObject *res;
    BaseObject *obj;
    if ((res = param_check(temp1, ZSETOBJECT, obj))) return res;

    rangespec range = {temp2, temp3, 0, 0};
    auto zset_obj = dynamic_cast<ZsetObject*>(obj);
    sdshdr* temp_sds = zset_obj->ZsetRangeByScore(range);
    res = PyUnicode_FromString(temp_sds->get_buf());
    delete temp_sds;
    return res;
}

static PyObject* py_zrank(PyObject *self, PyObject *args){
    char *temp1;
    int temp2;
    if (!PyArg_ParseTuple(args, "si", &temp1, &temp2)) return nullptr;
    PyObject *res;
    BaseObject *obj;
    if ((res = param_check(temp1, ZSETOBJECT, obj))) return res;

    auto zset_obj = dynamic_cast<ZsetObject*>(obj);
    sdshdr *temp_sds;
    auto temp_obj = zset_obj->ZsetGetByRank(temp2);
    if (!temp_obj) return PyUnicode_FromString(shared.rangeout->get_value()->get_buf());
    temp_sds = temp_obj->get_value();

    res = PyUnicode_FromString(temp_sds->get_buf());
    if (temp_obj->object_type != STRINGOBJECT) delete temp_sds;
    return res;
}

static PyObject* py_zrem(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;
    temp_str1 = new StringObject(temp1);
    auto obj = _db->get_value(temp_str1);
    decrRefCount(temp_str1);
    if (!obj || obj->object_type != ZSETOBJECT) return PyLong_FromLong(0);

    auto zset_obj = dynamic_cast<ZsetObject*>(obj);
    temp_str2 = new StringObject(temp2);

    auto res = PyLong_FromLong(zset_obj->ZsetRem(temp_str2));
    decrRefCount(temp_str2);
    return res;
}

static PyObject* py_zscore(PyObject *self, PyObject *args){
    char *temp1, *temp2;
    if (!PyArg_ParseTuple(args, "ss", &temp1, &temp2)) return nullptr;
    PyObject *res;
    BaseObject *obj;

    if ((res = param_check(temp1, ZSETOBJECT, obj))) return res;

    temp_str2 = new StringObject(temp2);
    auto zset_obj = dynamic_cast<ZsetObject*>(obj);
    res = PyUnicode_FromString(zset_obj->ZsetScore(temp_str2)->get_buf());
    decrRefCount(temp_str2);
    return res;
}


static PyMethodDef pydisMethods[] = {
    {"init", py_init, METH_VARARGS, "init env\r\n successfully return 1"},
    {"set", py_set, METH_VARARGS, "set key value"},
    {"get", py_get, METH_VARARGS, "get value according to value"},
    {"exists", py_exists, METH_VARARGS, "determine if the key exists\r\nexists return 1 else return 0"},
    {"delete", py_del, METH_VARARGS, "delete key successfully return 1 not exist return 0"},
    {"rename", py_rename, METH_VARARGS, "param: old_key\r\nparam: new_key\r\nreplace old_key with new_key, val unchanged\r\nsuccessful return ok else return err"},
    {"type", py_type, METH_VARARGS, "param: key\r\nget the type of key corresponding to value"}, 
    {"getset", py_getset, METH_VARARGS, "Sets the value of the specified key and returns the old value of key"},
    {"getrange", py_getrange, METH_VARARGS, "get value in the range, invalid range reutrn 'wrong range', does not exist key return 'none'"},
    {"strlen", py_strlen, METH_VARARGS, "get len of string, does not exist return -1"},
    {"expire", py_expire, METH_VARARGS, "set expire to key\r\n successful return ok\r\n"},

    {"sadd", py_sadd, METH_VARARGS, "add value to set, if value already exists return exists, else return ok"},
    {"scard", py_scard, METH_VARARGS, "return set len\r\nif does not exist set, return 0"},
    {"spop", py_spop, METH_VARARGS, "randomly return an element from set and delete it\r\nif set does not exist return  'not exists'"},
    {"srem", py_srem, METH_VARARGS, "delete element from set\r\nif does not exist set return not exist"},

    {"hset", py_hset, METH_VARARGS, "param: hashkey\r\nparam: key\r\nparam: value\r\nset k,v to hashkey, if does not exist hashkey, create it"},
    {"hget", py_hget, METH_VARARGS, "param: hashkey\r\nparam: key\r\nget the value corresponding to k from hashkey\r\ndoes not exist hashkey return notexists\r\ndoes not exist key return none"},
    {"hdel", py_hdel, METH_VARARGS, "param: hashkey\r\nparam: key\r\ndelete key from hashkey\r\nif does not exist hashkey return notexists\r\nif does not exist key return none"},
    {"hexists", py_hexists, METH_VARARGS, "param: hashkey\r\nparam: key\r\nif key exists in hashkey return 1\r\nif hashkey does not exist return -1\r\nif key does not exist return 0"},
    {"hlen", py_hlen, METH_VARARGS, "param: hashkey\r\nreturn the length of the hashkey\r\nif does not exists hashkey return 0"},
    {"hkeys", py_hkeys, METH_VARARGS, ""},
    {"hvals", py_hvals, METH_VARARGS, ""},

    {"lpush", py_lpush, METH_VARARGS, "param: listkey\r\nparam: value\r\nadd value to listkey head\r\nif listkey does not exist, create new listkey\r\nsuccessful return ok"},
    {"rpush", py_rpush, METH_VARARGS, "param: listkey\r\nparam: value\r\nadd value to listkey tail\r\nif listkey does not exist, create new listkey\r\nsuccessful return ok"}, 
    {"llen", py_llen, METH_VARARGS, "param: listkey\r\nreturn the len of the listkey\r\nif listkey does not exists return 0"},
    {"lpop", py_lpop, METH_VARARGS, "param: listkey\r\nreturn value from head\r\nif listkey length is 0 return none"},
    {"rpop", py_rpop, METH_VARARGS, "param: listkey\r\nreturn value from tail\r\nif listkey length is 0 return none"}, 
    {"lindex", py_lindex, METH_VARARGS, "param: listkey\r\nparam: index\r\nreturn value according to index\r\nrange out return none"}, 
    {"lrange", py_lrange, METH_VARARGS, "param: listkey\r\nparam: start\r\nparam: end\r\nrange out return 'range out'"}, 
    {"lrem", py_lrem, METH_VARARGS, "param: listkey\r\nparam: target_value\r\nparam: nums\r\nremove target_value from listkey according to nums\r\nif nums < 0, start from the end\r\nif nums > 0, start from the begin\r\nif nums = 0 all"},
    {"lset", py_lset, METH_VARARGS, "param: listkey\r\nparam: index\r\nparam: value\r\nset the value at index\r\nthe old value will be overwrite\r\ninvalid index return 'rangeou'"},

    {"zadd", py_zadd, METH_VARARGS, "param: zsetkey\r\nparam: value\r\nparam: socre\r\nadd value and score to sorted set\r\nif zsetkey does not exist, create new zset\r\nsuccessful return ok"},
    {"zcard", py_zcard, METH_VARARGS, "param: zsetkey\r\nreturn the len of th zsetkey\r\nif zsetkey does not exist return 0"},
    {"zcount", py_zcount, METH_VARARGS, "param: zsetkey\r\nparam: start\r\nparam: end\r\nreturn the number of elements in the range(according to score)\r\nzset key does not exist return 0"}, 
    {"zrank", py_zrank, METH_VARARGS, "param: zsetkey\r\nparam: rank\r\nreturn value according to rank\r\nif range out return 'rangeout'"},
    {"zrem", py_zrem, METH_VARARGS, "param: zsetkey\r\nparam: value\r\ndelete value from zsetkey\r\nsuccessful return 1 else return 0"},
    {"zscore", py_zscore, METH_VARARGS, "param: zsetkey\r\nparam: value\r\nget score according to value\r\nvalue does not exist return none"},
    {"zrangebyscore", py_zrangebyscore, METH_VARARGS, "param: zsetkey\r\nparam: start\r\nparam: end\r\nrange out return ''"},



    {NULL, NULL, 0, NULL}
};
// 对于sorted set的zadd, zcard, zcount, zrangebyscore, zrank, zrem, zscore

// 对于list的lpush, rpush, llen, lpop, rpop, lindex, lrange, lrem, lset
static struct PyModuleDef pydismodule = {
    PyModuleDef_HEAD_INIT, 
    "pydis", 
    NULL,
    -1,
    pydisMethods
};

PyMODINIT_FUNC PyInit_pydis(void){
    return PyModule_Create(&pydismodule);
}
