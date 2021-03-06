#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <unistd.h>
#ifdef USE_CRYPTOPP
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>  
#endif

class EncryptBase
{
public:
	EncryptBase();
    virtual ~EncryptBase();
    virtual bool SetServerFd(int fd) = 0;
    virtual bool SetClientFd(int fd) = 0;
    virtual ssize_t Read(void *buf, size_t len) = 0;
    virtual ssize_t Write(const void *buf, size_t len) = 0;
	virtual EncryptBase* clone() const = 0;
    int GetFd() const;

protected:
    int fd;
};

class SimpleEncrypter : public EncryptBase
{
public:
	SimpleEncrypter();
    bool SetServerFd(int fd);
    bool SetClientFd(int fd);
    ssize_t Read(void *buf, size_t len);
    ssize_t Write(const void *buf, size_t len);
	SimpleEncrypter* clone() const;

private:
    char randChar;
};

#ifdef USE_CRYPTOPP
class Aes128Ecb : public EncryptBase
{
public:
	bool SetServerFd(int fd);
	bool SetClientFd(int fd);
	ssize_t Read(void *buf, size_t len);
	ssize_t Write(const void *buf, size_t len);
	Aes128Ecb* clone() const;

private:
	void InitAes(std::string pwd);
	CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;
	CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;

	static const int iterations = 100;
};
#endif

class EncryptFactory
{
public:
    EncryptBase* GetEncrypter();
};

extern EncryptFactory GEncryptFactory;

#endif // ENCRYPT_H
