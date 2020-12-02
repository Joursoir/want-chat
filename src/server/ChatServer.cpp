#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ChatServer.hpp"
#include "UserInfo.hpp"
#include "ChatRoom.hpp"

const int qlen_for_listen = 6;

ChatServer::ChatServer(EventSelector *sel, DatabaseManager *db, int fd)
    : FdHandler(fd), the_selector(sel), dbase(db)
{
    the_selector->Add(this);
    lobby = new ChatRoom(this, std_id_lobby, 0);
    talkers = new StorageOfUsers();
}

ChatServer::~ChatServer()
{
    // right delete this stuff?
    if(room) {
        for(int i = 0; i < room_len; i++)
            DeleteRoom(i);

        delete[] room;
    }
    the_selector->Remove(this);

    while(talkers->Disconnect())
        ;
    delete talkers;
}

ChatServer *ChatServer::Start(EventSelector *sel, DatabaseManager *db, int port)
{
    int in_d = socket(AF_INET, SOCK_STREAM, 0);
    if(in_d == -1)
        return 0;

    int opt = 1;
    setsockopt(in_d, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int res = bind(in_d, (struct sockaddr*) &addr, sizeof(addr));
    if(res == -1)
        return 0;

    res = listen(in_d, qlen_for_listen);
    if(res == -1)
        return 0;
    
    return new ChatServer(sel, db, in_d);
}

int ChatServer::AddRoom(char *password)
{
    if(!room) {
        room_len = 16;
        room = new ChatRoom*[room_len];
        for(int i = 0; i < room_len; i++) 
            room[i] = 0;
    }

    int id = -1;
    for(int i = 0; i < room_len; i++) {
        if(room[i] == 0) {
            id = i;
            room[i] = new ChatRoom(this, id, password);
            break;
        }
    }

    if(id == -1) {
        ChatRoom **tmp = new ChatRoom*[room_len+1];
        for(int i = 0; i < room_len; i++)
            tmp[i] = room[i];

        id = room_len;
        tmp[room_len] = new ChatRoom(this, id, password);
        room_len += 1;

        delete[] room;
        room = tmp;
    }

    return id;
}

bool ChatServer::DeleteRoom(int id)
{
    if(!room[id])
        return false;

    delete room[id];
    room[id] = 0;

    return true;
}

bool ChatServer::RoomExist(int id) const
{
    if(!room[id])
        return false;
 
    return true;
}

void ChatServer::GotoLobby(ChatRoom *cur_room, UserInfo *u)
{
    cur_room->RemoveSession(u);
    lobby->AddSession(u);
    u->SetRoom(lobby);
}

handle_room_enter ChatServer::ChangeSessionRoom(ChatRoom *cur_room,
    UserInfo *u, int id, char *pass)
{
    if(id < 0 || id >= room_len || !room[id])
        return enter_noexist;

    const char *secret_word = room[id]->GetSecretPass();
    CONSOLE_LOG("right pass: %s; user enter pass: %s\n", secret_word, pass);
    if(secret_word != 0) {
        if(!pass)
            return enter_private;
        if(strcmp(secret_word, pass) != 0)
            return enter_uncorrect_pass;
    }

    cur_room->RemoveSession(u);
    room[id]->AddSession(u);
    u->SetRoom(room[id]);
    return enter_success;
}

void ChatServer::CloseConnection(UserInfo *u) 
{
    talkers->RemoveUser(u);
    the_selector->Remove(u);
    delete u;
}

void ChatServer::Handle(bool r, bool w)
{
    if(!r)
        return;

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int sd = accept(GetFd(), (struct sockaddr*) &addr, &len);
    if(sd == -1)
        return;

    UserInfo *u = new UserInfo(lobby, sd);
    talkers->AddUser(u);
    lobby->AddSession(u);
    the_selector->Add(u);
}

///////////////////////////////////////////////////////////////

StorageOfUsers::~StorageOfUsers()
{
    while(first) {
        item *tmp = first;
        first = first->next;
        delete tmp;
    }
}

void StorageOfUsers::SendAllUsers(const char *msg, UserInfo *except,
    const int spec_msg)
{
    CONSOLE_LOG("Send message all: %s\n", msg);
    item *p;
    for(p = first; p; p = p->next)
        if(p->u != except)
            p->u->Send(msg, spec_msg);
}

void StorageOfUsers::AddUser(UserInfo *u)
{
    item *p = new item;
    p->next = first;
    p->u = u;
    first = p;
    online++;
}

void StorageOfUsers::RemoveUser(UserInfo *u)
{
    item **p;
    for(p = &first; *p; p = &((*p)->next)) {
        if( ((*p)->u) == u ) {
            item *tmp = *p;
            *p = tmp->next;
            // not delete UserInfo!
            online--;
            delete tmp;
            return;
        }
    }
}

UserInfo *StorageOfUsers::Disconnect()
{
    if(first) {
        item *tmp = first;
        first = first->next;
        UserInfo *rn = tmp->u;
        online--;
        delete tmp;
        return rn;
    }
    return 0;
}