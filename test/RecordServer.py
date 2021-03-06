#!/usr/bin/env python

import socket
import struct
import os

HOST = "127.0.0.1"
PORT = 8390

SFMT = '>h'
IFMT = '>I'


def BytesToNumber(fmt, message, offset):
    num = struct.unpack_from(fmt, message, offset)[0]
    offset += struct.calcsize(fmt)
    return num, offset


def OutputMessage(message):
    offset = 0

    version, offset = BytesToNumber(SFMT, message, offset)

    s_sec, offset = BytesToNumber(IFMT, message, offset)
    s_usec, offset = BytesToNumber(IFMT, message, offset)
    e_sec, offset = BytesToNumber(IFMT, message, offset)
    e_usec, offset = BytesToNumber(IFMT, message, offset)
    ip, offset = BytesToNumber(IFMT, message, offset)
    port, offset = BytesToNumber(IFMT, message, offset)
    upload, offset = BytesToNumber(IFMT, message, offset)
    download, offset = BytesToNumber(IFMT, message, offset)

    user_len, offset = BytesToNumber(SFMT, message, offset)
    user = message[offset : offset + user_len].decode('ascii')
    offset += user_len

    host_len, offset = BytesToNumber(SFMT, message, offset)
    host = message[offset : offset + host_len].decode('ascii')
    offset += host_len

    outstr = "\
STime: {1},{2}{0}\
ETime: {3},{4}{0}\
User: {5}{0}\
Connect: {6},{7}{0}\
Host: {8}{0}\
Upload: {9}{0}\
Download: {10}{0}\
".format(os.linesep,
        s_sec, s_usec,
        e_sec, e_usec,
        user,
        socket.inet_ntoa(struct.pack("=I", ip)), port,
        host,
        upload, download)
    print outstr


if __name__ == '__main__':
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))

    while 1:
        message = s.recv(4096)
        OutputMessage(message)
