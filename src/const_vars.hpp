#ifndef WC_CONSTVARS_H
#define WC_CONSTVARS_H

// === Authorization info ==== //
const int max_name_len = 18;
const int min_name_len = 3;
const int max_player_lenpass = 24;
const int min_player_lenpass = 4;

// === UI info === //
const int max_msg_len = 171; // size of buffer for messages
const int max_usermsg_len = max_msg_len - max_name_len - 2; // ": " = 2

// === Message info === //
const char USUAL_CHAR = 0;
const char SYSTEM_CHAR = '#';
const char USERS_CHAR = ';';
const char GONLINE_CHAR = '!';
const char RONLINE_CHAR = '&';

#endif