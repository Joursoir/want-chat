#ifndef CHATREALIZATION_H
#define CHATREALIZATION_H

#include "sockets.hpp"
#include "../const_vars.hpp"

#define CONSOLE_LOG(f_, ...) printf((f_), ##__VA_ARGS__)

class ChatRoom;
class Server;

class ChatSession : FdHandler {
    friend class Server;

	char name[max_name_len];
	char buffer[max_msg_len];
	int buf_used;
	bool ignoring;

	ChatRoom *the_master;

	ChatSession(ChatRoom *i_master, int i_fd) : FdHandler(i_fd),
		buf_used(0), ignoring(false), the_master(i_master) {}
    ~ChatSession() {}

    virtual void Handle(bool r, bool w); 
	void ReadAndIgnore();
    void ReadAndCheck();
    void CheckLines();
public:
    const char *GetName() const { return name; }

    void ChangeName(const char *n_name);
    void Send(const char *msg);
};

class Server : public FdHandler {
    EventSelector *the_selector;
    ChatRoom **room;
    ChatRoom *lobby;

    int room_len;

    Server(EventSelector *sel, int fd);
public:
    ~Server();

    static Server *Start(EventSelector *sel, int port);

    int AddRoom();
    // RemoveRoom();
    // void AddSessionToRoom(ChatSession *s, int id);
    void CloseConnection(ChatSession *s)
        { the_selector->Remove(s); delete s; }
private:
    virtual void Handle(bool r, bool w); 
};


#endif