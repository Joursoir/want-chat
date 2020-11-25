#ifndef ROOMREALIZATION_H
#define ROOMREALIZATION_H

class Server;
class ChatSession;

class ChatRoom {
    Server *the_server;
    const bool it_lobby;

    struct item {
        ChatSession *s;
        item *next;
    };
    item *first;
public:
    ChatRoom(Server *i_server, bool i_lobby)
        : the_server(i_server), it_lobby(i_lobby), first(0) {}
    ~ChatRoom();

    void SendAll(const char *msg, ChatSession *except = 0);
    void LeaveMessage(ChatSession *except);

    void HandleMessage(ChatSession *ses, const char *str);
    void HandleCommand(ChatSession *s, int cmd_counter,
        char **commands);

    void AddSession(ChatSession *ses);
    void RemoveSession(ChatSession *ses);
    void CloseSession(ChatSession *ses);
};

#endif