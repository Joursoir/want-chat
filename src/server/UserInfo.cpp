#include "UserInfo.hpp"

void UserInfo::SetRoom(ChatRoom *new_master)
{
    this->the_master = new_master;
}

const char *UserInfo::GetName() const
{
    if(name[0])
        return name;
    return 0;
}

void UserInfo::SetName(const char *n_name)
{
    strcpy(name, n_name);
}

void UserInfo::SetStatus(const enum_status e_s)
{
    state = e_s;
}
