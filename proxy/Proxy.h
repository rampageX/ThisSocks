#ifndef PROXY_H
#define PROXY_H
#include "Passwd.h"
#include "Config.h"
#include "Logger.h"
#include "Utils.h"
#include "Encrypt.h"
#include <unistd.h>
#include <math.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAXBUF 8192

std::string GetSocketPair(int connfd);

class Proxy
{
public:
    void Run(int connfd);

protected:
    bool virtual isMatch(const char *request, int len) {}
    void virtual Run(int srcfd, const char *request, int len) {}

    Proxy* SelectLocalProxy(bool isClient, const char *request, int len);
    int ConnectProxyServer();

    void ForwardData(int srcfd, int tarfd);
    int ReadN(int fd, void *buf, size_t count);

    EncryptBase *encrypter;

private:
    int ForwardData(int srcfd, int tarfd, bool fromClient);
};

#endif
