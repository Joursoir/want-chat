#include <stdio.h>

#include "UserInfo.hpp"

void UserInfo::Handle(bool r, bool w)
{
    if(!r)
        return;

    // this functions create for support any client, not one WantChat client:
    if(buf_used >= (int)sizeof(buffer)) {
        buf_used = 0;
        ignoring = true;
    }
    if(ignoring) {
        //CONSOLE_LOG("Ignore the message, it's so big\n");
        ReadAndIgnore();
    }
    else
        ReadAndCheck();
}

void UserInfo::ReadAndIgnore()
{
    int rc = read(GetFd(), buffer, sizeof(buffer));
    //CONSOLE_LOG("readI return %d bytes\n", rc);
    if(rc < 1) {
        the_master->CloseSession(this);
        return;
    }

    for(int i = 0; i < rc; i++)
        if(buffer[i] == '\n')
        { // stop ignoring!
            //CONSOLE_LOG("ReadAndIgnore: find \\n\n");
            int rest = rc - i - 1;
            if(rest > 0)
                memmove(buffer, buffer + i + 1, rest);
            buf_used = rest;
            ignoring = 0;
            CheckLines();
        }
}

void UserInfo::ReadAndCheck()
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

void UserInfo::CheckLines()
{
    if(buf_used <= 0)
        return;

    for(int i = 0; i < buf_used; i++) {
        if(buffer[i] == '\n') {
            //CONSOLE_LOG("[CheckLines] buffer[i] == \\n i = %d\n", i);
            buffer[i] = 0;
            if(i > 0 && buffer[i-1] == '\r')
                buffer[i-1] = 0;
            
            //CONSOLE_LOG("printed: %s\n", buffer);
            the_master->HandleMessage(this, buffer);

            int rest = buf_used - i - 1;
            memmove(buffer, buffer + i + 1, rest);
            buf_used = rest;
            //CONSOLE_LOG("[CheckLines] new buf_used = %d\n", buf_used);
            CheckLines();
            return;
        }
    }
}

void UserInfo::Send(const char *msg, const char spec_ch)
{
    char *tmp_msg = new char[1+strlen(msg)+2]; // for spec_symb + \n

    if(spec_ch == USUAL_CHAR)
        sprintf(tmp_msg, "%s\n", msg);
    else
        sprintf(tmp_msg, "%c%s\n", spec_ch, msg);

    CONSOLE_LOG("send: %s", tmp_msg);
    write(GetFd(), tmp_msg, strlen(tmp_msg));

    delete[] tmp_msg;
}

