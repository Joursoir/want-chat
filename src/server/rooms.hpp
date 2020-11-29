#ifndef ROOMREALIZATION_H
#define ROOMREALIZATION_H

#include "../const_vars.hpp"

const int std_id_lobby = -1;
const int max_room_lenpass = 24;

class Server;
class ChatSession;

class ChatRoom {
    Server *the_server;
    const int code;
    // if code == std_id_lobby then it's lobby

    char secret_pass[max_room_lenpass];

    struct item {
        ChatSession *s;
        item *next;
    };
    item *first;
public:
    ChatRoom(Server *i_server, int id, char *pass);
    ~ChatRoom();

    void SendAll(const char *msg, ChatSession *except = 0,
        const int spec_msg = system_msg);

    void HandleMessage(ChatSession *ses, const char *str);
    void HandleCommand(ChatSession *ses, int cmd_counter,
        char **commands);

    const char *GetSecretPass();

    void AddSession(ChatSession *ses);
    void RemoveSession(ChatSession *ses);
    void CloseSession(ChatSession *ses);
};

#endif