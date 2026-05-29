#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include "VSocket.h"

void VSocket::Init(char t, bool IPv6)
{
    int st = -1;

    this->type = t;
    this->IPv6 = IPv6;
    int domain = IPv6 ? AF_INET6 : AF_INET;

    int socketType = t == 's' ? SOCK_STREAM : SOCK_DGRAM;

    this->sockId = socket(domain, socketType, 0);

    st = this->sockId;
    if (-1 == st)
    {
        throw std::runtime_error("VSocket::Init, (reason)");
    }
}

VSocket::~VSocket()
{
    this->Close();
}

void VSocket::Close()
{
    int st = close(this->sockId);
    if (-1 == st)
    {
        throw std::runtime_error("VSocket::Close()");
    }
}

int VSocket::TryToConnect(const char *hostip, int port)
{
    int st;
    struct sockaddr_in host4;

    memset((char *)&host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    st = inet_pton(AF_INET, hostip, &host4.sin_addr);

    if (-1 == st)
    {
        throw std::runtime_error("VSocket::TryToConnect, inet_pton");
    }

    host4.sin_port = htons(port);

    st = connect(this->sockId, (sockaddr *)&host4, sizeof(host4));

    if (-1 == st)
    {
        throw std::runtime_error("VSocket::TryToConnect");
    }

    return st;
}

int VSocket::TryToConnect(const char *host, const char *service)
{

    struct addrinfo hints, *result, *rp;

    int st;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = this->IPv6 ? AF_INET6 : AF_INET;

    hints.ai_socktype = (this->type == 's') ? SOCK_STREAM : SOCK_DGRAM;

    st = getaddrinfo(host, service, &hints, &result);

    if (st != 0)
    {
        throw std::runtime_error("VSocket::TryToConnect getaddrinfo");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {

        st = connect(this->sockId, rp->ai_addr, rp->ai_addrlen);

        if (st == 0)
        {
            break;
        }
    }

    freeaddrinfo(result);

    if (st == -1)
    {
        throw std::runtime_error("VSocket::TryToConnect");
    }

    return st;
}