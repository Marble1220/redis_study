import socket
import hiredis

import pydis
from event_driven import *


class client(object):

    def __init__(self, fd: socket.socket, el: EventLoop):
        self.fd = fd
        self.reply: list = []  # 待回复的内容
        self.query: bytes = b''  # 待查询的内容
        self.el = el
        self.parser = hiredis.Reader(encoding="utf-8")
        # logging.info("new client, fd: {}".format(fd.fileno()))

        # 创建时便将自己加入事件循环中
        if fd:
            fd.setblocking(False)
            el.create_file_event(fd, READABLE, el.read_qurey_from_client, self)


    def processInputBuff(self):
        '''
        处理客户端输入的内容， 从socket中读取数据
        :return:
        '''
        self.parser.feed(self.query)
        # print(self.query)
        # req = self.query
        self.query = b''

        req = self.parser.gets()
        if req:
            self.el.create_file_event(self.fd, WRITEABLE, self.el.send_reply_to_client, self)

        while req:
            # if not req:
            #     break
            # else:
            # res = getattr(self, req[0])(*req[1:])
            res = str(req).encode()
            # res = req
            self.reply.append(res)
            req = self.parser.gets()
            # req = False
    # def add_reply(self, res):
    #     '''
    #     将res添加到待返回列表中， 并将
    #     :param res:
    #     :return:
    #     '''
    #
    # def