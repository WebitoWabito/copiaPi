#ifndef VSocket_h
#define VSocket_h
class VSocket
{
public:
    void Init(char, bool = false);
    ~VSocket();

    void Close();
    int TryToConnect(const char *, int);
    int TryToConnect(const char *, const char *);
    virtual int Connect(const char *, int) = 0;
    virtual int Connect(const char *, const char *) = 0;
    virtual size_t Read(void *, size_t) = 0;
    virtual size_t Write(const void *, size_t) = 0;
    virtual size_t Write(const char *) = 0;

protected:
    int sockId;
    bool IPv6;
    int port;
    char type;
};

#endif