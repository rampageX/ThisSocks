#ifndef PROXY_H
#define PROXY_H
#include "../Passwd.h"
#include "../Config.h"
#include "../Logger.h"
#include "../Utils.h"
#include "../Encrypt.h"
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
    Proxy();
    void Run(int connfd);
    virtual ~Proxy()
    {
        if (encrypter != NULL) {
            delete encrypter;
            encrypter = NULL;
        }
    }

protected:
    virtual bool isMatch(const char *, int) { return false; }
    virtual void Run(int, const char *, int) {}

    Proxy* SelectLocalProxy(bool isClient, const char *request, int len);
    bool ValidateProxyClient();
    int ConnectProxyServer();
    bool LoginProxyServer();
    int ConnectRealServer(uint32_t ip, uint16_t port);

    void ForwardData(int srcfd, int tarfd);
    int ReadN(int fd, void *buf, size_t count);

    EncryptBase *encrypter;

private:
    int ForwardData(int srcfd, int tarfd, bool fromClient);
    Passwd pwd;
};

#endif
