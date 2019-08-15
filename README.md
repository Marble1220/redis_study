1.预计使用cpp以oop的方式实现redis的基本数据结构\
2.使用python完成redis的网络部分， 使用python调用cpp库进行操作


基本完成数据存储与python调用部分
使用方法：
    cd cpp_src && python build.py all
    得到pydis.so动态库
    python import 即可使用
测试:
    cd cpp_src && py.test