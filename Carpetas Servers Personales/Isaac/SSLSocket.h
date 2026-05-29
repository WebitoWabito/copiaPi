#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include "Socket.h"

class SSLSocket : public Socket
{
private:
  void *Context;
  void *BIO;

public:
  SSLSocket(char *certFileName, char *keyFileName, bool IPv6 = false);
  SSLSocket(int id);
  SSLSocket(bool IPv6 = false);
  virtual ~SSLSocket();

  void InitSSL(bool serverContext = false);
  void InitContext(bool serverContext = false);
  void LoadCertificates(const char *certFileName, const char *keyFileName);

  int Connect(const char *host, int port);
  int Connect(const char *host, const char *service);

  size_t Read(void *buffer, size_t size) override;
  size_t Write(const void *buffer, size_t size) override;
  size_t Write(const char *text) override;

  VSocket *AcceptConnection() override;

  void AttachSSL(void *sharedContext);

  void ShowCerts();
  const char *GetCipher();

  void *GetContext() const { return Context; }
};

#endif