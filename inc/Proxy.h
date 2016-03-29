#ifndef PROXY_H
#define PROXY_H
#include "Passwd.h"
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

class Proxy
{
public:
    Proxy();
	Proxy(const Proxy& proxy);
    virtual ~Proxy();
	Proxy& operator=(const Proxy& proxy);
    void Run(int connfd, int &srvfd);

protected:
	virtual void Process(int srcfd);
    virtual void Run(int, const char *, int, int&) {}
    void ForwardData(int srcfd, int tarfd) const;
    EncryptBase *encrypter;

private:
    Proxy* SelectLocalProxy(bool isClient, const char *request, int len);
    int ConnectRealServer(uint32_t ip, uint16_t port);
    bool ValidateProxyClient();
    int ConnectProxyServer();
    bool LoginProxyServer();
    int ForwardData(int srcfd, int tarfd, bool fromClient) const;
};

#endif
