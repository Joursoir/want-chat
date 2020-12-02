#ifndef WC_CHATSERVER_H
#define WC_CHATSERVER_H

#include "sockets.hpp"
#include "database.hpp"
#include "../const_vars.hpp"

#define CONSOLE_LOG(f_, ...) printf((f_), ##__VA_ARGS__)

enum handle_room_enter {
    enter_noexist,
    enter_private,
    enter_uncorrect_pass,
    enter_success
};

class ChatRoom;
class UserInfo;

class ChatServer : public FdHandler {
    EventSelector *the_selector;
    DatabaseManager *dbase;

    ChatRoom **room;
    int room_len;
    ChatRoom *lobby;

    ChatServer(EventSelector *sel, DatabaseManager *db, int fd);
public:
    ~ChatServer();
    static ChatServer *Start(EventSelector *sel, 
        DatabaseManager *db, int fd);

    int AddRoom(char *password);
    bool DeleteRoom(int id); // call only if room is empty
    bool RoomExist(int id) const;

    void GotoLobby(ChatRoom *cur_room, UserInfo *u);
    handle_room_enter ChangeSessionRoom(ChatRoom *cur_room,
        UserInfo *u, int id, char *pass);

    void CloseConnection(UserInfo *u);

    // work with database:
    AnswerDB *QuerySelect(const char *sql) 
        { return dbase->QuerySelect(sql); }

private:
    virtual void Handle(bool r, bool w); 
};

/*class StorageOfUsers {
    struct item {
        ChatSession *s;
        item *next;
    }
    item *first;
public:
    AddSession(ChatSession *s);
    RemoveSession(ChatSession *s);
};*/

#endif