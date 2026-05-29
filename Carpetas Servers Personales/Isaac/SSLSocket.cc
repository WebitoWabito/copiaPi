#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>
#include <cstring>

#include "SSLSocket.h"

SSLSocket::SSLSocket(bool IPv6) : Socket('s', IPv6)
{
  Context = nullptr;
  BIO = nullptr;

  InitSSL(false);
}

SSLSocket::SSLSocket(char *certFileName, char *keyFileName, bool IPv6) : Socket('s', IPv6)
{
  Context = nullptr;
  BIO = nullptr;

  InitSSL(true);
  LoadCertificates(certFileName, keyFileName);
}

SSLSocket::SSLSocket(int id) : Socket(id)
{
  Context = nullptr;
  BIO = nullptr;
}

SSLSocket::~SSLSocket()
{
  if (BIO)
  {
    SSL_shutdown((SSL *)BIO);
    SSL_free((SSL *)BIO);
    BIO = nullptr;
  }
}

void SSLSocket::InitContext(bool serverContext)
{
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  const SSL_METHOD *method =
      serverContext ? TLS_server_method()
                    : TLS_client_method();

  SSL_CTX *ctx = SSL_CTX_new(method);

  if (!ctx)
    throw std::runtime_error("SSL_CTX_new failed");

  Context = ctx;
}

void SSLSocket::InitSSL(bool serverContext)
{
  InitContext(serverContext);

  SSL *ssl = SSL_new((SSL_CTX *)Context);

  if (!ssl)
    throw std::runtime_error("SSL_new failed");

  BIO = ssl;
}

void SSLSocket::LoadCertificates(const char *certFileName,
                                 const char *keyFileName)
{
  SSL_CTX *ctx = (SSL_CTX *)Context;

  if (SSL_CTX_use_certificate_file(
          ctx, certFileName, SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("Error cargando certificado");
  }

  if (SSL_CTX_use_PrivateKey_file(
          ctx, keyFileName, SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("Error cargando llave privada");
  }

  if (!SSL_CTX_check_private_key(ctx))
  {
    throw std::runtime_error(
        "La llave privada no coincide con el certificado");
  }
}

int SSLSocket::Connect(const char *host, int port)
{
  int st = MakeConnection(host, port);

  SSL *ssl = (SSL *)BIO;
  SSL_set_fd(ssl, idSocket);

  st = SSL_connect(ssl);

  if (st <= 0)
    throw std::runtime_error("SSL_connect failed");

  return st;
}

int SSLSocket::Connect(const char *host,
                       const char *service)
{
  int st = MakeConnection(host, service);

  SSL *ssl = (SSL *)BIO;
  SSL_set_fd(ssl, idSocket);

  st = SSL_connect(ssl);

  if (st <= 0)
    throw std::runtime_error("SSL_connect failed");

  return st;
}

size_t SSLSocket::Read(void *buffer, size_t size)
{
  int st = SSL_read((SSL *)BIO, buffer, size);

  if (st <= 0)
    throw std::runtime_error("SSL_read failed");

  return st;
}

size_t SSLSocket::Write(const void *buffer, size_t size)
{
  int st = SSL_write((SSL *)BIO, buffer, size);

  if (st <= 0)
    throw std::runtime_error("SSL_write failed");

  return st;
}

size_t SSLSocket::Write(const char *text)
{
  return Write(text, strlen(text));
}

VSocket *SSLSocket::AcceptConnection()
{
  int clientId = WaitForConnection();

  return new SSLSocket(clientId);
}

void SSLSocket::AttachSSL(void *sharedContext)
{
  Context = sharedContext;

  SSL *ssl = SSL_new((SSL_CTX *)Context);

  if (!ssl)
    throw std::runtime_error("SSL_new failed");

  SSL_set_fd(ssl, idSocket);

  if (SSL_accept(ssl) <= 0)
  {
    ERR_print_errors_fp(stderr);
    SSL_free(ssl);
    throw std::runtime_error("SSL_accept failed");
  }

  BIO = ssl;
}

void SSLSocket::ShowCerts()
{
  X509 *cert = SSL_get_peer_certificate((SSL *)BIO);

  if (cert)
  {
    char *line;

    line = X509_NAME_oneline(
        X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);

    line = X509_NAME_oneline(
        X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);

    X509_free(cert);
  }
}

const char *SSLSocket::GetCipher()
{
  return SSL_get_cipher((SSL *)BIO);
}