#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "chat.hpp"
#include "rooms.hpp"

const int qlen_for_listen = 6;

void ChatSession::Send(const char *msg, const int spec_msg)
{
    int len = strlen(msg);
    char *tmp_msg = new char[len+1+2]; // for spec_symb + \n

    if(spec_msg == usual_msg)
        sprintf(tmp_msg, "%s\n", msg);
    else
    {
        char spec;
        if(spec_msg == system_msg) spec = system_char;
        sprintf(tmp_msg, "%c%s\n", spec, msg);
    }

    CONSOLE_LOG("%s", tmp_msg);
    write(GetFd(), tmp_msg, strlen(tmp_msg));

    delete[] tmp_msg;
}

void ChatSession::Handle(bool r, bool w)
{
    if(!r)
        return;

    // this functions create for support any client, not one WantChat client:
    if(buf_used >= (int)sizeof(buffer)) {
        buf_used = 0;
        ignoring = true;
    }
    if(ignoring) {
        CONSOLE_LOG("Ignore the message, it's so big\n");
        ReadAndIgnore();
    }
    else
        ReadAndCheck();
}
 
void ChatSession::ReadAndIgnore()
{
    int rc = read(GetFd(), buffer, sizeof(buffer));
    CONSOLE_LOG("readI return %d bytes\n", rc);
    if(rc < 1) {
        the_master->CloseSession(this);
        return;
    }

    for(int i = 0; i < rc; i++)
        if(buffer[i] == '\n')
        { // stop ignoring!
            CONSOLE_LOG("ReadAndIgnore: find \\n\n");
            int rest = rc - i - 1;
            if(rest > 0)
                memmove(buffer, buffer + i + 1, rest);
            buf_used = rest;
            ignoring = 0;
            CheckLines();
        }
}

void ChatSession::ReadAndCheck()
{
    int rc = read(GetFd(), buffer+buf_used, sizeof(buffer)-buf_used);
    CONSOLE_LOG("readC return %d bytes\n", rc);
    if(rc < 1) {
        the_master->CloseSession(this);
        return;
    }
    buf_used += rc;
    CheckLines();
}

void ChatSession::CheckLines()
{
    if(buf_used <= 0)
        return;

    for(int i = 0; i < buf_used; i++) {
        if(buffer[i] == '\n') {
            CONSOLE_LOG("[CheckLines] buffer[i] == \\n i = %d\n", i);
            buffer[i] = 0;
            if(i > 0 && buffer[i-1] == '\r')
                buffer[i-1] = 0;
            
            CONSOLE_LOG("printed: %s\n", buffer);
            the_master->HandleMessage(this, buffer);

            int rest = buf_used - i - 1;
            memmove(buffer, buffer + i + 1, rest);
            buf_used = rest;
            CONSOLE_LOG("[CheckLines] new buf_used = %d\n", buf_used);
            CheckLines();
            return;
        }
    }
}

const char *ChatSession::GetName()
{
    if(name[0])
        return name;
    return 0;
}

void ChatSession::SetName(const char *n_name)
{
    strcpy(name, n_name);
}

void ChatSession::SetRoom(ChatRoom *new_master)
{
    this->the_master = new_master;
}

//////////////////////////////////////////////////////////////

Server::Server(EventSelector *sel, int fd)
    : FdHandler(fd), the_selector(sel)
{
    the_selector->Add(this);
    lobby = new ChatRoom(this, std_id_lobby, 0);
}

Server::~Server()
{
    if(room)
        delete[] room;

    the_selector->Remove(this);
}

Server *Server::Start(EventSelector *sel, int port)
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
    
    return new Server(sel, in_d);
}

bool Server::RoomExist(int id) const
{
    if(!room[id])
        return false;
 
    return true;
}

void Server::Handle(bool r, bool w)
{
    if(!r)
        return;

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int sd = accept(GetFd(), (struct sockaddr*) &addr, &len);
    if(sd == -1)
        return;

    ChatSession *s = new ChatSession(lobby, sd);
    lobby->AddSession(s);
    the_selector->Add(s);
}

int Server::AddRoom(char *password)
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

bool Server::DeleteRoom(int id)
{
    if(!room[id])
        return false;

    delete room[id];
    room[id] = 0;

    return true;
}

void Server::GotoLobby(ChatRoom *cur_room, ChatSession *s)
{
    cur_room->RemoveSession(s);
    lobby->AddSession(s);
    s->SetRoom(lobby);
}

handle_room_enter Server::ChangeSessionRoom(ChatRoom *cur_room,
    ChatSession *s, int id, char *pass)
{
    if(id < 0 || id >= room_len || !room[id])
        return enter_noexist;

    const char *secret_word = room[id]->GetSecretPass();
    CONSOLE_LOG("s: %s; pass: %s\n", secret_word, pass);
    if(secret_word != 0) {
        if(!pass)
            return enter_private;
        if(strcmp(secret_word, pass) != 0)
            return enter_uncorrect_pass;
    }

    cur_room->RemoveSession(s);
    room[id]->AddSession(s);
    s->SetRoom(room[id]);
    return enter_success;
}