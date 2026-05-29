/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2025-i
 *  Grupos: 1 y 3
 *
 ****** VSocket base class implementation
 *
 * (Fedora version)
 *
 **/

#include <sys/socket.h>
#include <arpa/inet.h> // ntohs, htons
#include <stdexcept>   // runtime_error
#include <cstring>     // memset
#include <netdb.h>     // getaddrinfo, freeaddrinfo
#include <unistd.h>    // close
/*
#include <cstddef>
#include <cstdio>

//#include <sys/types.h>
*/
#include "VSocket.h"

/**
 *  Class creator (constructor)
 *     use Unix socket system call
 *
 *  @param     char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param     bool ipv6: if we need a IPv6 socket
 *
 **/
void VSocket::BuildSocket(char t, bool IPv6)
{

  int domain = IPv6 ? AF_INET6 : AF_INET;

  if (t == 's')
  {
    this->type = SOCK_STREAM;
  }
  else if (t == 'd')
  {
    this->type = SOCK_DGRAM;
  }
  else
  {
    throw std::runtime_error("Tipo de socket invalido");
  }

  this->idSocket = socket(domain, this->type, 0);

  if (this->idSocket == -1)
  {
    throw std::runtime_error("VSocket::BuildSocket");
  }

  this->IPv6 = IPv6;
}

/**
 * Class destructor
 *
 **/
VSocket::~VSocket()
{

  this->Close();
}

/**
 * Close method
 *    use Unix close system call (once opened a socket is managed like a file in Unix)
 *
 **/
void VSocket::Close()
{
  if (this->idSocket == -1)
    return;

  int st = close(this->idSocket);

  if (st == -1)
  {
    return;
  }

  this->idSocket = -1;
}

/**
 * EstablishConnection method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dot notation, example "10.84.166.62"
 * @param      int port: process address, example 80
 *
 **/
int VSocket::EstablishConnection(const char *hostip, int port)
{

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, hostip, &server_addr.sin_addr) <= 0)
  {
    throw std::runtime_error("Error en inet_pton");
  }

  int st = connect(this->idSocket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (st == -1)
  {
    throw std::runtime_error("VSocket::EstablishConnection");
  }

  return st;
}

/**
 * EstablishConnection method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
 * @param      char * service: process address, example "http"
 *
 **/
int VSocket::EstablishConnection(const char *host, const char *service)
{
  int st = -1;

  throw std::runtime_error("VSocket::EstablishConnection");

  return st;
}

/**
 * Bind method
 *    use "bind" Unix system call (man 3 bind) (server mode)
 *
 * @param      int port: bind a unamed socket to a port defined in sockaddr structure
 *
 *  Links the calling process to a service at port
 *
 **/
int VSocket::Bind(int port)
{

  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  int st = bind(this->idSocket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (st == -1)
  {
    throw std::runtime_error("VSocket::Bind");
  }

  this->port = port;

  return st;
}

/**
 * MarkPassive method
 *    use "listen" Unix system call (man listen) (server mode)
 *
 * @param      int backlog: defines the maximum length to which the queue of pending connections for this socket may grow
 *
 *  Establish socket queue length
 *
 **/
int VSocket::MarkPassive(int backlog)
{

  int st = listen(this->idSocket, backlog);

  if (st == -1)
  {
    throw std::runtime_error("VSocket::MarkPassive");
  }

  return st;
}

/**
 * WaitForConnection method
 *    use "accept" Unix system call (man 3 accept) (server mode)
 *
 *
 *  Waits for a peer connections, return a sockfd of the connecting peer
 *
 **/
int VSocket::WaitForConnection(void)
{

  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);

  int st = accept(this->idSocket, (struct sockaddr *)&client_addr, &len);

  if (st == -1)
  {
    throw std::runtime_error("VSocket::WaitForConnection");
  }

  return st;
}

/**
 * Shutdown method
 *    use "shutdown" Unix system call (man 3 shutdown) (server mode)
 *
 *
 *  cause all or part of a full-duplex connection on the socket associated with the file descriptor socket to be shut down
 *
 **/
int VSocket::Shutdown(int mode)
{

  int st = shutdown(this->idSocket, mode);

  if (st == -1)
  {
    throw std::runtime_error("VSocket::Shutdown");
  }

  return st;
}

// UDP methods 2025

/**
 *  sendTo method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to send data
 *
 *  Send data to another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::sendTo(const void *buffer, size_t size, void *addr)
{
  int st = -1;

  return st;
}

/**
 *  recvFrom method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to receive from data
 *
 *  @return	size_t bytes received
 *
 *  Receive data from another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::recvFrom(void *buffer, size_t size, void *addr)
{
  int st = -1;

  return st;
}
