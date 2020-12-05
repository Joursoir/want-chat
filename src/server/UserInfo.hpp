#ifndef WC_USER_H
#define WC_USER_H

#include <string.h>

#include "sockets.hpp"
#include "ChatRoom.hpp"
#include "../const_vars.hpp"

class ChatServer;

enum enum_status {
    wait_name, // expecting a username from player
    wait_reg, // expecting registration
    wait_login, // expecting login
    no_wait
};

#define CONSOLE_LOG(f_, ...) printf((f_), ##__VA_ARGS__) // delete later

class UserInfo : FdHandler {
    friend class ChatServer;

	char name[max_name_len];
    char password[max_player_lenpass];
    char buffer[max_msg_len];
    int buf_used;
    bool ignoring;

    enum_status state;
    ChatRoom *the_master;

    UserInfo(ChatRoom *i_master, int i_fd) : FdHandler(i_fd), name("0"),
        password("0"), buf_used(0), ignoring(false), state(wait_name),
        the_master(i_master) {}
    ~UserInfo() {}

    void SetRoom(ChatRoom *new_master);

    // === realization in UserInfo_io.cpp ===
    virtual void Handle(bool r, bool w); // w disabled, only read
    void ReadAndIgnore();
    void ReadAndCheck();
    void CheckLines();
public:
    void Send(const char *msg, const int spec_msg = system_msg);
    // === realization in UserInfo_io.cpp ===

    const char *GetName() const;
    void SetName(const char *n_name);
    
    const char *GetPassword() const;
    void SetPassword(const char *pass);

    enum_status GetStatus() const { return state; }
    void SetStatus(const enum_status n_status);
};

#endif