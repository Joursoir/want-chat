#ifndef WC_CHATROOM_H
#define WC_CHATROOM_H

#include "../const_vars.hpp"

#define DB_BUFFER_SIZE 128

const char first_reg[] = "First you must register using /reg";
const char first_login[] = "First you must log in using /login";
const int std_id_lobby = -1;
const int max_room_lenpass = 24;

class ChatServer;
class UserInfo;
class StorageOfUsers;

class ChatRoom {
    ChatServer *the_server;
    const int code; // if code == std_id_lobby then it's lobby
    char secret_pass[max_room_lenpass];

    StorageOfUsers *users;
public:
    ChatRoom(ChatServer *i_server, int id, char *pass);
    ~ChatRoom();

    void HandleMessage(UserInfo *u, const char *str);
    void HandleCommand(UserInfo *u, int cmd_counter,
        char **commands); // in _cmd

    const char *GetSecretPass();

    void AddSession(UserInfo *u);
    void RemoveSession(UserInfo *u);
    void CloseSession(UserInfo *u);

    static unsigned long Hash(const char *str); // in _cmd
private:
    static char **ParseToArg(const char *input, int &arrc); // in _cmd
    static bool CheckForbiddenSymbols(const char *str); // in _cmd

    void Identification(UserInfo *u, const char *str);
    bool CheckEnterNickname(UserInfo *u, const char *name);
    bool CheckEnterPassword(UserInfo *u, const char *pass);

    // IsPasswordRight
};

#endif