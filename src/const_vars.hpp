#ifndef CONSTVARS_H
#define CONSTVARS_H

/*
 * In chat messages look like that:
 * Knight385: Hello, guys!
 * 
*/

const int max_name_len = 18;
const int min_name_len = 3;
const int max_player_lenpass = 24;
const int min_player_lenpass = 4;

const int oneline_len = 57;
const int max_usermsg_len = oneline_len * 3 - max_name_len - 2; // ": " = 2
							// 57 * 3 - 18 - 2 = 151
const int max_msg_len = oneline_len * 3; // 57*3 = 171

const int usual_msg = 0; // no first char 
const int system_msg = 1;
const char system_char = '#';

#endif