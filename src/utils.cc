#include "utils.h"

#include <errno.h>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <arpa/inet.h>
#include <limits.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "feature.h"

using namespace std;

void Utils::TrimLeft(string& s)
{
    s.erase(s.begin(), find_if_not(
            s.begin(), s.end(), static_cast<int(*)(int)>(isspace)));
}

void Utils::TrimRight(string& s)
{
    s.erase(find_if_not(s.rbegin(), s.rend(),
            static_cast<int(*)(int)>(isspace)).base(), s.end());
}

void Utils::Trim(string& s)
{
    TrimLeft(s);
    TrimRight(s);
}

void Utils::Split(const string& str, char c, vector<string>& vec)
{
    stringstream ss(str);
    string item;
    while (getline(ss, item, c)) {
        vec.push_back(item);
    }
}

string Utils::GetSocketPair(int connfd)
{
	char buf[1024];
    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
    socklen_t len;

    memset(&localAddr, 0, sizeof(localAddr));
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    if (getsockname(connfd, (struct sockaddr *)&localAddr, &len) == -1) {
		strerror_r(errno, buf, sizeof(buf));
		return string("getsockname error: ") + buf;
    }
    if (getpeername(connfd, (struct sockaddr *)&remoteAddr, &len) == -1) {
		strerror_r(errno, buf, sizeof(buf));
		return string("getpeername error: ") + buf;
    }

    char localIp[20];
    char remoteIp[20];
    inet_ntop(AF_INET, &localAddr.sin_addr, localIp, sizeof(localIp));
    inet_ntop(AF_INET, &remoteAddr.sin_addr, remoteIp, sizeof(remoteIp));

    snprintf(buf, sizeof(buf), "%s:%u-%s:%u",
        localIp, ntohs(localAddr.sin_port),
        remoteIp, ntohs(remoteAddr.sin_port));
    return string(buf);
}

string Utils::GetAbsolutePath(const string& path)
{
	char rpath[4096];
    return realpath(path.c_str(), rpath) == NULL ? "" : rpath;
}

string Utils::GetDirectoryName(const string& path)
{
   string::size_type pos = path.find_last_of('/');
   if (pos == string::npos) {
        return "";
   } else if (pos == 0) {
        return "/";
   }

   pos = path.find_last_not_of('/', pos);
   if (pos == string::npos) {
        return "/";
   }
   return path.substr(0, pos + 1);
}

/*
 * 返回指定路径所在目录的绝对路径
 * 如果传入的是非根目录，截掉最后的'/'返回
 * 参数无效返回空串
 */
string Utils::GetAbsDir(const string& path)
{
	char rpath[4096];
	if (realpath(path.c_str(), rpath) == NULL) {
		return "";
	}
	string strpath = string(rpath);	

	struct stat sbuf;
	if (stat(strpath.c_str(), &sbuf) == -1) {
		return "";
	}

    return S_ISDIR(sbuf.st_mode) ? strpath :
        strpath.substr(0, strpath.find_last_of('/'));
}

/*
 * 将指定的dir和path组合成绝对路径
 * dir必须是绝对路径
 * 如果path是绝对路径则返回path
 * 参数无效返回空串
 */
string Utils::JoinPath(const string& dir, const string& path)
{
    if (!path.empty() && *path.begin() == '/') {
        return path;
    }

    if (dir.empty()) {
        return path;
    } else {
        return *dir.rbegin() == '/' ? dir + path : dir + "/" + path;
    }
}
