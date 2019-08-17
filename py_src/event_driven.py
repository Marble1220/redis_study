import time
from typing import *
import socket
import select
import hiredis
# import fcntl

from Client import *

READABLE = 1
WRITEABLE = (1 << 1)
import logging
logging.basicConfig(level=logging.INFO)


socket.socket()
class FileEvent(object):
    __slots__ = ("fd", "mask", "readhandler", "writehandler", "client")

    def __init__(self, fd=-1, mask=0, readhandler=None, writehandler=None, client=None):
        self.fd: socket.socket = fd
        self.mask: int = mask    # mask为1, 2, 3分别为读，写，读和写
        self.readhandler = readhandler
        self.writehandler = writehandler
        self.client = client


class ReadyEvent(object):
    __slots__ = ("fd", "mask")

    def __init__(self, fd, mask):
        self.fd: int = fd  # 文件描述符
        self.mask: int = mask


class EventLoop(object):
    __slots__ = ("maxfd", "lasttime", "events", "readys", "io_data")

    def __init__(self):
        self.maxfd: int = 0
        self.lasttime: float = time.time()
        self.events: Dict(int, FileEvent) = {}
        self.readys: List[ReadyEvent] = []
        self.io_data: select.epoll = None
        # self.parser = hiredis.Reader()

    def create_file_event(self, fd: socket.socket, mask: int, handler, privdata):
        '''
        将给定fd的给定事件添加到events中
        添加到events的FileEvent结构由mask, handler, privdata共同决定
        如果已存在， 则更新FileEvent
        :param fd: 描述符(socket)
        :param mask: 表示读写的标志， 根据此更新FileEvent的可读写状态
        :param handler: 事件发生时的回调函数
        :param privdata: handler需要的数据， 保存在FileEvent中
        :return:
        '''
        # logging.info("new event, fd: {}, mask: {}".format(fd.fileno(), mask))
        fe = self.events.setdefault(fd.fileno(), FileEvent())
        self.event_add(fd, mask)

        fe.mask |= mask
        if mask & READABLE:
            fe.readhandler = handler
        if mask & WRITEABLE:
            fe.writehandler = handler
        fe.client = privdata
        fe.fd = fd

        if fd.fileno() > self.maxfd:
            self.maxfd = fd.fileno()



    def process_event(self) -> int:
        '''
        等待io复用模块返回并处理对应事件
        io复用返回时已将对应的事件描述符和对应状态(读或写)以ReadyEvent形式
        保存在readys中, 实际只返回事件的数量
        :return: 返回处理的事件数量
        '''
        eventnums = self.event_poll(-1)
        # logging.info("start process event, quantity is {}".format(eventnums))
        for i in range(eventnums):
            fe = self.events[self.readys[i].fd]
            mask = self.readys[i].mask

            if fe.mask & mask & READABLE:
                fe.readhandler(fe.fd, fe.client, mask)
            elif fe.mask & mask & WRITEABLE:
                fe.writehandler(fe.fd, fe.client, mask)
        self.readys = []
        return eventnums



    def accept_tcp_handler(self, fd: socket.socket, privdata, mask):
        '''
        创建client
        :param fd: 用于accept的socket
        :param privdata: 基本为null
        :param mask: 基本为read， 表示可读
        :return:
        '''
        try:
            cfd, caddr = fd.accept()
        except BlockingIOError:
            return

        client(cfd, self)







    def read_qurey_from_client(self, fd: socket.socket, client, mask):
        '''
        从socket中读取客户端发来的数据并将其保存到client的查询缓存中

        :param fd: 与client连接的描述符
        :param client: client对象， 表示一个client并保存了其状态
        :param mask: 基本为read， 表示可读
        :return:
        '''
        max_size = 16 * 1024 * 1024
        try:
            # logging.info("client sends message")
            while max_size > 0:
                # print(1)
                data = fd.recv(1024)
                if data == b'':
                    # print(1)
                    self.event_del(fd, mask)
                    # fd.close()
                    break
                client.query += data
                max_size -= len(data)
        except BlockingIOError:
            pass
        finally:
            client.processInputBuff()







    def send_reply_to_client(self, fd: socket.socket, client, mask):
        '''
        将client的就绪列表中的数据全部发回, 并将事件从io复用中删除
        :param fd: 与client连接的描述符
        :param client: client对象， 表示一个client并保存了其状态
        :param mask: 基本为write, 表示可写
        :return:
        '''

        reply = client.reply
        while reply:
            fd.sendall(reply[0])
            del reply[0]
        # logging.info("answer sent")
        self.event_del(fd, WRITEABLE)






    def event_del(self, fd, delmask):
        '''
        将给定描述符的给定事件从io复用模块中删除
        :param fd: 待删除的描述符
        :param delmask: 要删除的事件掩码
        :return:
        '''
        fe: FileEvent = self.events[fd.fileno()]
        mask = fe.mask & (~delmask)
        fe.mask = mask
        evts = 0
        if mask & READABLE: evts |= select.EPOLLIN
        if mask & WRITEABLE: evts |= select.EPOLLOUT

        if mask:
            self.io_data.modify(fd.fileno(), evts)
        else:
            # logging.info("delete fd: {}".format(fd))
            self.io_data.unregister(fd.fileno())
            # print(fe.client.query)
            if fe.client.query == b'':
                # print(2)
                del self.events[fd.fileno()]
                fd.close()





    def event_poll(self, times) -> int:
        '''
        使用io复用模块等待事件发生
        :param times: 需要等待的时间
        :return: 待处理事件数量
        '''
        # logging.info("start waiting for event")
        fd_event_list = self.io_data.poll(times)

        for fd, e in fd_event_list:
            mask = 0
            if e == select.EPOLLIN:
                mask |= READABLE
            if e == select.EPOLLOUT:
                mask |= WRITEABLE
            if e == select.EPOLLERR:
                mask |= WRITEABLE
            if e == select.EPOLLHUP:
                mask |= WRITEABLE

            self.readys.append(ReadyEvent(fd, mask))

        return len(fd_event_list)

    def event_add(self, fd, mask):
        '''
        将给定时间添加到io复用中
        :param fd: 待添加的描述符
        :param mask: 要添加的事件掩码
        :return:
        '''
        # logging.info("add event fd: {}, mask: {}".format(fd, mask))
        evts = 0
        if mask & READABLE:
            evts |= select.EPOLLIN
        if mask & WRITEABLE:
            evts |= select.EPOLLOUT

        fe: FileEvent = self.events[fd.fileno()]
        if fe.mask == 0:

            self.io_data.register(fd.fileno(), evts)
        else:
            self.io_data.modify(fd.fileno(), evts)





    def create_io_multiplexing(self):
        '''
        创建io复用实例并将起数据绑定到EventLoop
        :return:
        '''
        # logging.info("create io multiplexing")
        self.io_data = select.epoll()


if __name__ == '__main__':
    el = EventLoop()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setblocking(False)
    s.bind(('localhost', 5555))
    s.listen(256)
    el.create_io_multiplexing()
    el.create_file_event(s, READABLE, el.accept_tcp_handler, None)
    while True:
        el.process_event()
