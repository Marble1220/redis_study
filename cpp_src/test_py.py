import time
import pydis

NONE = "none\r\n"
WRAP = "\r\n"
SPACE = " \r\n"
ERR = "err\r\n"
OK = "ok\r\n"
NOTEXIST = "key not exist\r\n"
typeerr = "type err\r\n"
EXIST = "value already exists\r\n"
UNERR = "unknown error\r\n"
RANGEOUT = "range out\r\n"



pydis.init()

# for i in range(100):
#     pydis.set(str(i), str(i+100).encode())
# # for i in range(100):
#     # pydis.delete(str(i))
# for i in range(100):
#     print(pydis.getset(str(i), str(i+10000).encode()))

# for i in range(100):
#     print(pydis.get(str(i)))
# print()

# for i in range(100):
#     print(pydis.strlen(str(i)))


# # (5) 对于set的sadd, scard, spop, srem

# for i in range(200, 300):
#     print(pydis.sadd("test", "test" + str(i)))


# print(pydis.scard('test'))

# for i in range(102):
#     print(pydis.spop("test"))
# for i in range(200, 300):
#     pydis.sadd("test1", str(i))
# print(pydis.get("test1"))

# for i in range(200, 300):
#     print(pydis.srem("test1", str(i)))
# # for i in range(100):
# pydis.set("expiretest", "1".encode())
# print(pydis.get("expiretest"))

# when = int((time.time()*1000)) + 1000
# pydis.expire("expiretest", when)
# print(pydis.get("expiretest"))
# time.sleep(0.5)
# print(pydis.get("expiretest"))


# (3) 对于hash的hdel, hexists, hget, hlen, hkeys, hset, hvals
class TestHashObject:
    def test_hset(self):
        for i in range(100):
           assert pydis.hset('a', str(i), str(i+100)) == OK
        pydis.set("b", "b".encode())
        assert pydis.hset('b', 'a', 'a') == typeerr

        pydis.hset('a', "100", "200")
        assert pydis.hget('a', '100') == "200"
        pydis.hset('a', '100', '100')
        assert pydis.hget('a', '100') == '100'

    def test_hget(self):
        for i in range(100):
            assert pydis.hget('a', str(i)) == str(i+100)

    def test_hexists(self):
        for i in range(100):
            assert pydis.hexists('a', str(i)) == 1
        for i in range(101, 200):
            assert pydis.hexists('a', str(i)) == 0
        assert pydis.hexists('b', 'a') == -1
        assert pydis.hexists('c', 'a') == -1
    
    def test_hlen(self):
        assert pydis.hlen('a') == 101
        assert pydis.hlen('b') == 0
        assert pydis.hlen('c') == 0
        assert pydis.hlen('d') == 0        

    def test_hkeys(self):
        
        pydis.delete('a')
        s = set()

        for i in range(100):
            assert pydis.hset('a', str(i), str(i+100)) == OK
            s.add(str(i))

        keyval = pydis.hkeys('a')
        while keyval != -1:
            s.remove(keyval)
            keyval = pydis.hkeys('a')

        assert len(s) == 0        
        
    def test_hvals(self):
        s = set()
        for i in range(100):
            s.add(str(i+100))
        
        valval = pydis.hvals('a')
        while valval != -1:
            s.remove(valval)
            valval = pydis.hvals('a')
        assert len(s) == 0

# (4) 对于list的lpush, rpush, llen, lpop, rpop, lindex, lrange, lrem, lset

class TestListObject:
    def test_push_and_pop(self):
        for i in range(100):
            assert pydis.lpush('list_a', str(i)) == OK
        for i in range(100):
            assert pydis.rpop('list_a') == str(i)
        
        for i in range(100):
            assert pydis.rpush('list_a', str(i)) == OK
        for i in range(100):
            assert pydis.lpop('list_a') == str(i)
    
    def test_llen(self):
        for i in range(100):
            pydis.rpush("list_b", str(i))
        assert pydis.llen("list_b") == 100
        assert pydis.llen("list_a") == 0
        assert pydis.llen("list_c") == 0

    def test_lindex(self):
        assert pydis.lindex("list_a", 10) == NONE
        assert pydis.lindex("list_b", 10) == str(10)
        assert pydis.lindex("asdf", 100) == NOTEXIST
        assert pydis.lindex("a", 100) == typeerr
    
    def test_lset(self):
        for i in range(9):
            assert pydis.lset("list_b", i*10, str(1)) == OK
        
        assert pydis.lset("list_b", 111, 'asdf') == RANGEOUT

        for i in range(9):
            assert pydis.lindex("list_b", i*10) == '1'
    
    def test_lrem(self):
        assert pydis.lrem("list_b", '1', 0) == 10
        assert pydis.llen("list_b") == 90
        for i in range(9):
            assert pydis.lset("list_b", i*10, str(1)) == OK

        assert pydis.lrem("list_b", '1', -5) == 5
        assert pydis.llen("list_b") == 85
        assert pydis.lrem("list_b", "1", 4) == 4
        assert pydis.llen("list_b") == 81


# for i in range(100):
#     print(pydis.lpush('list_a', str(i)))
# for i in range(100):
#     print(pydis.lpop('list_a'))
# 对于sorted set的zadd, zcard, zcount, zrangebyscore, zrank, zrem, zscore
class TestZsetObject:
    def test_zadd_zrank(self):
        for i in range(100):
            assert pydis.zadd("zset_a", str(i), i) == OK
        for i in range(100):
            assert pydis.zrank("zset_a", i+1) == str(i)
        
        assert pydis.zadd("list_b", "asd", 0) == typeerr
        assert pydis.zrank("zset_a", 1000) == RANGEOUT
        assert pydis.zrank("zset_a", -1) == RANGEOUT
        assert pydis.zrank("zset_b", -1) == NOTEXIST

    def test_zcard_zrem(self):
        assert pydis.zcard("zset_a") == 100

        assert pydis.zrem("zset_a", "0") == 1
        assert pydis.zrem("zset_a", "2434") == 0

        assert pydis.zcard("zset_a") == 99
        assert pydis.zrank("zset_a", 1) == '1'
        for i in range(1, 100):
            assert pydis.zrem("zset_a", str(i)) == 1
        assert pydis.zcard("zset_a") == 0

    def test_zcount_zscore(self):
        for i in range(100):
            assert pydis.zadd("zset_b", str(i), i+100.5) == OK
        
        assert pydis.zcount("zset_b", 100, 110.5) == 11
        assert pydis.zcount("list_a", 1, 2) == 0
        assert pydis.zcount("zset_b", 0, 10) == 0
        
        assert pydis.zscore("zset_b", "10") == "110.5"
        assert pydis.zscore("zset_b", "-11") == NONE
        assert pydis.zscore("list_b", "11") == typeerr
        assert pydis.zscore("zasw", '11') == NOTEXIST

    def test_zrangebyscore(self):
        for i in range(100):
            assert pydis.zadd("zset_c", str(i), i+100.5) == OK

        assert pydis.zrangebyscore("zset_c", 0, 10) == ''
        assert pydis.zrangebyscore("zawd", 0, 1) == NOTEXIST
        assert pydis.zrangebyscore("list_a", 1, 2) == typeerr

for i in range(100):
    assert pydis.zadd("zset_c", str(i), i+100.5) == OK

print(pydis.zrangebyscore("zset_c", 100, 122))