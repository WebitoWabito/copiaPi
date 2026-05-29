#ifndef PROTOCOLUTILS_H
#define PROTOCOLUTILS_H

#include <string>

inline bool parseRequest(const std::string &req, std::string & cmd,std::string &msg)
{
  if (req.size() < 4)
    return false;
  if (req[0] != 'P' || req[1] != '/')
    return false;

  cmd = req.substr(2, 1);
  msg = (req.size() > 4) ? req.substr(4) : "";

  return true;
}

inline bool isHTTP(const std::string &req)
{
  return req.rfind("GET ", 0) == 0 || req.rfind("HEAD ", 0) == 0;
}

#endif