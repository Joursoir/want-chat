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

class StorageOfUsers {
    int online;
    struct item {
        UserInfo *u;
        item *next;
    };
    item *first;
public:
    StorageOfUsers() : online(0), first(0) {}
    ~StorageOfUsers();

    void SendAllUsers(const char *msg, UserInfo *except = 0,
        const char spec_ch = SYSTEM_CHAR);
    void SendAllUsersOnline(const char spec_ch);
    
    void SendAllUsersName();
    void SendUsersNameTo(UserInfo *u);
    UserInfo *SearchUserByName(const char *name);

    void AddUser(UserInfo *u);
    void RemoveUser(UserInfo *u);
    // func for fast clear storage:
    UserInfo *Disconnect();

    int GetOnline() const { return online; }
};

class ChatServer : public FdHandler {
    EventSelector *the_selector;
    DatabaseManager *dbase;

    ChatRoom **room;
    int room_len;
    ChatRoom *lobby;

    StorageOfUsers *talkers;

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
    int QueryInsert(const char *sql) 
        { return dbase->QueryInsert(sql); }

    // players actions:
    UserInfo *IsUserOnline(const char *name)
        { return talkers->SearchUserByName(name); }
private:
    virtual void Handle(bool r, bool w); 
};

#endif