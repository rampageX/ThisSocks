#include "TcpServer.h"
#include "Config.h"
#include "Proxy.h"
#include "Logger.h"
#include <string>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOCKDIR "/var/tmp"

using namespace std;

static void daemonize();
static void lockfile();

int main(int argc, char *argv[])
{
	int ret = GConfig.Init(argc, argv);
	if (ret <= 0) {
		return ret;
	}

	if (GConfig.RunAsDaemon) {
		daemonize();
	}
	GLogger.Init(GConfig.RunAsDaemon ? argv[0] : 0);
	if (GConfig.RunAsDaemon) {
		lockfile();
	}

	string listenAddr;
	int listenPort;
	if (GConfig.RunAsClient) {
		listenAddr = GConfig.LocalAddress;
		listenPort = GConfig.LocalPort;
	} else {
		listenAddr = GConfig.ServerAddress;
		listenPort = GConfig.ServerPort;
		GPasswd.LoadFile(GConfig.PwdFile);
	}

    TcpServer srv;
    if (!srv.Init(listenAddr, listenPort)) {
		return -1;
	}
	srv.Run();

	// Proxy *proxy = new Proxy();
    // srv.Run(proxy);
	return 1; // error occured
}

void daemonize()
{
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
		perror("getrlimit error");
		exit(1);
	}

	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	} else if (pid != 0) {
		exit(0);
	}
	setsid();

	if (rl.rlim_max == RLIM_INFINITY) {
		rl.rlim_max = 1024;
	}
    for (unsigned i = 0; i < rl.rlim_max; ++i) {
		close(i);
	}

	int fd0, fd1, fd2;
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(fd0);
	fd2 = dup(fd0);
    assert(fd0 == 0);
    assert(fd1 == 1);
    assert(fd2 == 2);
}

void lockfile()
{
	string lockfile;
	if (GConfig.RunAsClient) {
		lockfile = LOCKDIR + string("/ThisSocks_C.pid");
	} else {
		lockfile = LOCKDIR + string("/ThisSocks_S.pid");
	}

	int fd = open(lockfile.c_str(), O_RDWR | O_CREAT, 
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		GLogger.LogErr(LOG_ERR, "open lockfile error");
		exit(1);
	}

	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	if (fcntl(fd, F_SETLK, &fl) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			exit(1); // already running
		}
		GLogger.LogErr(LOG_ERR, "fcntl lockfile error");
		exit(1);
	}

	char buf[16];
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf));
}
