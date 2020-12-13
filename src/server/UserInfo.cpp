#include "UserInfo.hpp"

void UserInfo::SetRoom(ChatRoom *new_master)
{
    the_master = new_master;
}

const char *UserInfo::GetName() const
{
    return name;
}

void UserInfo::SetName(const char *n_name)
{
    strcpy(name, n_name);
}

const char *UserInfo::GetPassword() const
{
	return password;
}

void UserInfo::SetPassword(const char *pass)
{
	strcpy(password, pass);
}

void UserInfo::SetStatus(const enum_status n_status)
{
    state = n_status;
}

void UserInfo::SetUserList(int list)
{
    u_list = list;
}
