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

    void SetRoom(ChatRoom *new_master);
public:
    const char *GetName();

    void SetName(const char *n_name);
    void Send(const char *msg);
};

class Server : public FdHandler {
    EventSelector *the_selector;
    ChatRoom **room;
    int room_len;

    ChatRoom *lobby;

    Server(EventSelector *sel, int fd);
public:
    ~Server();

    static Server *Start(EventSelector *sel, int port);

    bool RoomExist(int id) const;

    int AddRoom();
    bool DeleteRoom(int id); // call only if room is empty

    void GotoLobby(ChatRoom *cur_room, ChatSession *s);
    bool ChangeSessionRoom(ChatRoom *cur_room, ChatSession *s, int id);
    void CloseConnection(ChatSession *s)
        { the_selector->Remove(s); delete s; }
private:
    virtual void Handle(bool r, bool w); 
};


#endif