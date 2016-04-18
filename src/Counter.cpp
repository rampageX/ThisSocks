#include "Counter.h"
#include "Logger.h"
#include "Config.h"

pthread_key_t Counter::pkey;

pthread_once_t Counter::once = PTHREAD_ONCE_INIT;

void Counter::CreateKey()
{
	if (!IsNeedRecord()) {
		return;
	}

	pthread_once(&once, InitThread);
	RecordSTime();
}

void Counter::InitThread()
{
	pthread_key_create(&pkey, DeleteKey);
}

void Counter::DeleteKey(void* arg)
{
	if (arg != NULL) {
		ThreadInfo* info = (ThreadInfo*)arg;
		RecordETime(info);
		info->Print();

		delete info;
		info = NULL;
		arg = NULL;
	}
}

ThreadInfo* Counter::GetThreadInfo()
{
	ThreadInfo* info = (ThreadInfo*)pthread_getspecific(pkey);
	if (info == NULL) {
		info = new ThreadInfo();
		pthread_setspecific(pkey, info);
	}
	return info;
}

bool Counter::IsNeedRecord()
{
	return !GConfig.StatAddress.empty() && GConfig.StatPort != 0;
}

void Counter::RecordUser(std::string user)
{
	if (!IsNeedRecord()) {
		return;
	}

	ThreadInfo* info = GetThreadInfo();
	info->User = user;
}

void Counter::RecordAddress(unsigned int ip, unsigned short port)
{
	if (!IsNeedRecord()) {
		return;
	}

	ThreadInfo* info = GetThreadInfo();
	info->IP = ip;
	info->Port = port;
}

void Counter::RecordUpload(unsigned int size)
{
	if (!IsNeedRecord()) {
		return;
	}

	ThreadInfo* info = GetThreadInfo();
	info->Upload += size;
}

void Counter::RecordDownload(unsigned int size)
{
	if (!IsNeedRecord()) {
		return;
	}

	ThreadInfo* info = GetThreadInfo();
	info->Download += size;
}

void Counter::RecordSTime()
{
	ThreadInfo* info = GetThreadInfo();
	gettimeofday(&info->STime, NULL);
}

void Counter::RecordETime(ThreadInfo* info)
{
	gettimeofday(&info->ETime, NULL);
}

ThreadInfo::ThreadInfo()
	: IP(0), Port(0), Upload(0), Download(0)
{
}

void ThreadInfo::Print()
{
	GLogger.LogMsg(
			LOG_DEBUG,
			"\nSTime: %ld.%ld"
			"\nETime: %ld.%ld"
			"\nUser: %s"
		   	"\nConnect: %u,%u"
		    "\nUpload: %u"
		   	"\nDonwload: %u"
			"\n",
			STime.tv_sec, STime.tv_usec,
			ETime.tv_sec, ETime.tv_usec,
			User.c_str(),
			IP, Port,
			Upload, Download);
}
