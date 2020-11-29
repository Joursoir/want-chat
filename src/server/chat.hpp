#ifndef CHATREALIZATION_H
#define CHATREALIZATION_H

#include "sockets.hpp"
#include "../const_vars.hpp"

#define CONSOLE_LOG(f_, ...) printf((f_), ##__VA_ARGS__)

enum handle_room_enter {
    enter_noexist,
    enter_private,
    enter_uncorrect_pass,
    enter_success
};

enum enum_status {
    wait_name, // expecting a username from player
    wait_reg, // expecting registration
    wait_log, // expecting login
    no_wait
};

class ChatRoom;
class Server;

class ChatSession : FdHandler {
    friend class Server;
    friend class ChatRoom;

	char name[max_name_len];
	char buffer[max_msg_len];
	int buf_used;
	bool ignoring;

    enum_status state;
	ChatRoom *the_master;

	ChatSession(ChatRoom *i_master, int i_fd) : FdHandler(i_fd),
		buf_used(0), ignoring(false), state(wait_name),
        the_master(i_master) {}
    ~ChatSession() {}

    void SetRoom(ChatRoom *new_master);

    virtual void Handle(bool r, bool w); 
    void ReadAndIgnore();
    void ReadAndCheck();
    void CheckLines();
public:
    const char *GetName();

    void SetName(const char *n_name);
    void Send(const char *msg, const int spec_msg = system_msg);
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

    int AddRoom(char *password);
    bool DeleteRoom(int id); // call only if room is empty

    void GotoLobby(ChatRoom *cur_room, ChatSession *s);
    handle_room_enter ChangeSessionRoom(ChatRoom *cur_room,
        ChatSession *s, int id, char *pass);
    void CloseConnection(ChatSession *s)
        { the_selector->Remove(s); delete s; }
private:
    virtual void Handle(bool r, bool w); 
};


#endif