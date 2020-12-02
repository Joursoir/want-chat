#ifndef WC_CHATROOM_H
#define WC_CHATROOM_H

#include "../const_vars.hpp"

const int std_id_lobby = -1;
const int max_room_lenpass = 24;
const int max_player_lenpass = 24;

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
        char **commands);

    const char *GetSecretPass();

    void AddSession(UserInfo *u);
    void RemoveSession(UserInfo *u);
    void CloseSession(UserInfo *u);

    static unsigned long hash(const char *str);
private:
    static char **ParseToArg(const char *input, int &arrc);
    static bool checkForbiddenSymbols(const char *str);
};

#endif