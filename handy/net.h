#pragma once
#include <string.h>
#include <netinet/in.h>
#include <string>
#include <algorithm>

namespace handy {

namespace {
    int16_t htobe(int16_t v) { return htobe16(v); }
    int32_t htobe(int32_t v) { return htobe32(v); }
    int64_t htobe(int64_t v) { return htobe64(v); }
    uint16_t htobe(uint16_t v) { return htobe16(v); }
    uint32_t htobe(uint32_t v) { return htobe32(v); }
    uint64_t htobe(uint64_t v) { return htobe64(v); }
}

struct net {
    template<class T> T hton(T v) { return htobe(v); }
    template<class T> T ntoh(T v) { return htobe(v); }
    static int setNonBlock(int fd, bool value=true);
    static int setReuseAddr(int fd, bool value=true);
    static int setNoDelay(int fd, bool value=true);
};

struct Ip4Addr {
    Ip4Addr(const char* host, short port);
    Ip4Addr(short port=0): Ip4Addr("", port) {}
    Ip4Addr(const struct sockaddr_in& addr): addr_(addr) {};
    std::string toString() const;
    std::string ip() const;
    short port() const;
    unsigned int ipInt() const;
    //if you pass a hostname to constructor, then use this to check error
    bool isIpValid() const;
    struct sockaddr_in& getAddr() { return addr_; }
private:
    struct sockaddr_in addr_;
};

struct Buffer {
    Buffer(): buf_(NULL), b_(0), e_(0), cap_(0), exp_(4096) {}
    ~Buffer() { delete[] buf_; }
    void clear() { if (cap_ > SmallBuf) { delete[] buf_; buf_ = NULL; cap_ = 0; } b_ = e_ = 0; }
    size_t size() const { return e_ - b_; }
    bool empty() const  { return e_ == b_; }
    char* data() const  { return buf_ + b_; }
    char* begin() const  { return buf_ + b_; }
    char* end() const  { return buf_ + e_; }
    char* makeRoom(size_t len);
    void makeRoom() { if (space() < exp_) expand(0);}
    size_t space() const  { return cap_ - e_; }
    void addSize(size_t len) { e_ += len; }
    char* allocRoom(size_t len) { char* p = makeRoom(len); addSize(len); return p; }
    Buffer& append(const char* p, size_t len) { memcpy(allocRoom(len), p, len); return *this; }
    template<class T> Buffer& appendValue(const T& v) { append((const char*)&v, sizeof v); return *this; }
    Buffer& consume(size_t len) { b_ += len; if (size() == 0) clear(); return *this; }
    Buffer& absorb(Buffer& buf);
    void setSuggestSize(size_t sz) { exp_ = sz; }
    Buffer(const Buffer& b) { copyFrom(b); }
    Buffer& operator=(const Buffer& b) { delete[] buf_; buf_ = NULL; copyFrom(b); return *this; }
private:
    enum { SmallBuf = 1024, };
    char* buf_;
    size_t b_, e_, cap_, exp_;
    void moveHead() { std::copy(begin(), end(), buf_); e_ -= b_; b_ = 0; }
    void expand(size_t len);
    void copyFrom(const Buffer& b);
};

}