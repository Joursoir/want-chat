#include "Client.hpp"

void Client::ShowMessage(const char *msg)
{
	chat->value(msg);
}