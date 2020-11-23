#ifndef SOCKETS_H
#define SOCKETS_H

#include <unistd.h>
#include <sys/poll.h>

class FdHandler { // abstract class
    int fd;
public:
    FdHandler(int i_fd) : fd(i_fd) {} 
    virtual ~FdHandler() { close(fd); }

    int GetFd() const { return fd; }

    virtual bool WantRead() const { return true; }
    virtual bool WantWrite() const { return false; }
    virtual void Handle(bool r, bool w) = 0;
};

class EventSelector {
    FdHandler **fd_array;
    struct pollfd *fds;
    int fd_array_len;
    int max_fd;

    bool exit_flag;
public:
    EventSelector() : fd_array(0), fds(0), max_fd(-1), exit_flag(false) {}
    ~EventSelector();

    void Add(FdHandler *h);
    bool Remove(FdHandler *h);

    void Run();
    void BreakLoop() { exit_flag = true; }
};
#endif