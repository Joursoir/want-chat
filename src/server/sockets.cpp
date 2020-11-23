#include <errno.h>

#include "sockets.hpp"

#define INVALID_FD -1

EventSelector::~EventSelector()
{
    if(fd_array)
        delete[] fd_array;
    if(fds)
    	delete[] fds;
}

void EventSelector::Add(FdHandler *h)
{
	int fd = h->GetFd();
	if(!fd_array) {
		fd_array_len = fd > 23 ? fd + 1 : 24;
		fd_array = new FdHandler*[fd_array_len];
		// maybe use memset too?
		fds = new struct pollfd[fd_array_len];

		for(int i = 0; i < fd_array_len; i++) {
			fd_array[i] = 0;
			fds[i] = { INVALID_FD, 0, 0 };
		}
	}
	if(fd >= fd_array_len) {
		FdHandler **tmp_arr = new FdHandler*[fd+1];
		struct pollfd *tmp_fds = new struct pollfd[fd+1];

		for(int i = 0; i <= fd; i++) {
			if(i < fd_array_len) {
				tmp_arr[i] = fd_array[i];
				tmp_fds[i] = fds[i];
			}
			else {
				fd_array[i] = 0;
				fds[i] = { INVALID_FD, 0, 0 };
			}
			fd_array_len = fd + 1;

			delete[] fd_array;
			fd_array = tmp_arr;
			delete[] fds;
			fds = tmp_fds;
		}
	}
	if(fd > max_fd)
		max_fd = fd;

	fd_array[fd] = h;
	fds[fd].fd = fd;
	if(fd_array[fd]->WantRead())
		fds[fd].events |= POLLIN;
    if(fd_array[fd]->WantWrite())
    	fds[fd].events |= POLLOUT;
}

bool EventSelector::Remove(FdHandler *h)
{
	int fd = h->GetFd();
	if(fd >= fd_array_len || fd_array[fd] != h)
        return false;

    fd_array[fd] = 0;
    fds[fd] = { INVALID_FD, 0, 0 };
    if(fd == max_fd) {
        while(max_fd >= 0 && !fd_array[max_fd])
            max_fd--;
    }

    return true;
}

void EventSelector::Run()
{
    exit_flag = false;
    do {
        int res = poll(fds, max_fd, -1);

        if(res < 0) {
        	if(errno == EINTR)
        		continue;
        	else
        		break;
        }

        if(res > 0) {
        	for(int i = 0; i <= max_fd; i++) {
        		if(!fd_array[i])
        			continue;
        		bool r = (fds[i].revents & POLLIN);
        		bool w = (fds[i].revents & POLLOUT);
        		if(r || w)
        			fd_array[i]->Handle(r, w);
        	}
        }
    } while(!exit_flag);
}