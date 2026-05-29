#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdexcept>

#include "SSLSocket.h"

SSLSocket::SSLSocket(bool IPv6)
{

    this->Init('s', IPv6);

    this->Context = nullptr;
    this->BIO = nullptr;

    this->InitSSL();
}

SSLSocket::SSLSocket(char *certFileName, char *keyFileName, bool IPv6)
{
}

SSLSocket::SSLSocket(int id)
{

    this->Init(id);
}

SSLSocket::~SSLSocket()
{
    if (nullptr != this->BIO)
    {
        SSL_free(reinterpret_cast<SSL *>(this->BIO));
    }
    if (nullptr != this->Context)
    {
        SSL_CTX_free(reinterpret_cast<SSL_CTX *>(this->Context));
    }

    this->Close();
}

void SSLSocket::InitSSL(bool serverContext)
{
    this->InitContext(serverContext);

    SSL *ssl = SSL_new((SSL_CTX *)this->Context);

    if (nullptr == ssl)
    {
        throw std::runtime_error("SSLSocket::InitSSL( bool )");
    }

    this->BIO = (void *)ssl;
}

void SSLSocket::InitContext(bool serverContext)
{
    const SSL_METHOD *method;
    SSL_CTX *context;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if (serverContext)
    {
        method = TLS_server_method();
    }
    else
    {
        method = TLS_client_method();
    }

    if (nullptr == method)
    {
        throw std::runtime_error("Method SSLSocket::InitContext( bool )");
    }

    context = SSL_CTX_new(method);

    if (nullptr == context)
    {
        throw std::runtime_error("Context SSLSocket::InitContext( bool )");
    }

    this->Context = (void *)context;
}

void SSLSocket::LoadCertificates(const char *certFileName, const char *keyFileName)
{
}

int SSLSocket::Connect(const char *host, int port)
{
    int st;

    st = this->TryToConnect(host, port);

    if (st < 0)
    {
        throw std::runtime_error("TCP SSLSocket::Connect( const char *, int )");
    }

    SSL *ssl = (SSL *)this->BIO;

    SSL_set_fd(ssl, this->sockId);

    st = SSL_connect(ssl);

    if (st != 1)
    {
        throw std::runtime_error("SSL SSLSocket::Connect( const char *, int )");
    }

    return st;
}

int SSLSocket::Connect(const char *host, const char *service)
{
    int st;

    st = this->TryToConnect(host, service);

    if (st < 0)
    {
        throw std::runtime_error("TCP SSLSocket::Connect( const char *, const char * )");
    }

    SSL *ssl = (SSL *)this->BIO;

    SSL_set_fd(ssl, this->sockId);

    st = SSL_connect(ssl);

    if (st != 1)
    {
        throw std::runtime_error("SSL SSLSocket::Connect( const char *, const char * )");
    }

    return st;
}

size_t SSLSocket::Read(void *buffer, size_t size)
{
    int st;

    st = SSL_read((SSL *)this->BIO, buffer, size);

    if (st < 0)
    {
        throw std::runtime_error("SSLSocket::Read( void *, size_t )");
    }

    return st;
}

size_t SSLSocket::Write(const char *string)
{
    int st = this->Write(string, strlen(string));
    ;

    if (-1 == st)
    {
        throw std::runtime_error("SSLSocket::Write( const char * )");
    }

    return st;
}

size_t SSLSocket::Write(const void *buffer, size_t size)
{
    int st;

    st = SSL_write((SSL *)this->BIO, buffer, size);

    if (st <= 0)
    {
        throw std::runtime_error("SSLSocket::Write( void *, size_t )");
    }

    return st;
}

void SSLSocket::ShowCerts()
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate((SSL *)this->BIO);
    if (nullptr != cert)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
    {
        printf("No certificates.\n");
    }
}

const char *SSLSocket::GetCipher()
{
    return SSL_get_cipher((SSL *)this->BIO);
}
