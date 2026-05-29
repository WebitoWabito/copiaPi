#ifndef HTTPUTILS_H
#define HTTPUTILS_H

#include <string>
#include <sstream>

inline std::string extractHTTPPath(const std::string &req)
{
  size_t start = req.find(' ');
  if (start == std::string::npos)
    return "/";

  ++start;

  size_t end = req.find(' ', start);
  if (end == std::string::npos)
    return "/";

  return req.substr(start, end - start);
}

inline std::string buildHTTPResponse(
    const std::string &body,
    const std::string &status = "200 OK",
    const std::string &contentType = "text/plain; charset=utf-8")
{

  std::ostringstream out;

  out << "HTTP/1.1 " << status << "\r\n";
  out << "Content-Type: " << contentType << "\r\n";
  out << "Content-Length: " << body.size() << "\r\n";
  out << "Connection: close\r\n";
  out << "\r\n";
  out << body;

  return out.str();
}

#endif