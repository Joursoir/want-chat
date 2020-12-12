#include <cerrno>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>

#include "../../config.hpp"
#include "../dimensions_client.hpp"
#include "OO_FLTK.hpp"
#include "Client.hpp"

const int spacing = 20;
const int border = 2;
const int indent = 5;

const int chat_w = 695;
const int chat_h = 440;
const int chat_line_h = chat_h / CHAT_LINES;

const int players_w = 225;
const int players_h = chat_line_h*PLAYERS_LINES+8;

const int tips_w = players_w;
const int tips_h = chat_line_h*TIPS_LINES+8;

const int input_w = chat_w + border + spacing + border + players_w;
const int input_h = 40;

const int win_w = spacing + border + (input_w) + border + spacing;
const int win_h = spacing + border + chat_h + border +
	spacing + border + input_h + border + spacing;

int main(int argc, char **argv)
{
	Fl_Window *win = new Fl_Window(win_w, win_h, "WantChat");
	win->color(FL_BLACK);

	// Draw chat:
	BoxBackground ol_chat(spacing, spacing,
		chat_w + border*2, chat_h + border*2);
	BoxBackground ol_chat_black(spacing + border, spacing + border,
		chat_w, chat_h, 0, FL_BLACK);

	ChatBaseOutput **chat = new ChatBaseOutput*[CHAT_LINES];
	for(int i = 0, y = 0; i < CHAT_LINES; i++, y += chat_line_h) {
		chat[i] = new ChatBaseOutput(spacing + border + indent, 
			spacing + border + y, chat_w - indent, chat_line_h);
	}
	// End draw chat

	// Draw players:
	int start_players_x = spacing + border + chat_w + border + spacing;
	BoxBackground ol_players(start_players_x, spacing, 
		players_w + border*2, players_h + border*2);
	BoxBackground ol_players_black(start_players_x + border,
		spacing + border, players_w, players_h, 0, FL_BLACK);

	ChatBaseOutput **players = new ChatBaseOutput*[PLAYERS_LINES];
	for(int i = 0, y = 0; i < PLAYERS_LINES; i++, y+= chat_line_h) {
		players[i] = new ChatBaseOutput(start_players_x + border + indent, 
			spacing + border + y, players_w - indent, chat_line_h);
	}
	players[PLAYERS_LINES-1]->value("<- /prev  /next ->");
	// End draw players

	// Draw tips:
	int start_tips_y = spacing + border + players_h + border + spacing + 16; // please, don't ask where i took 16
	BoxBackground ol_tips(start_players_x, start_tips_y,
		tips_w + border*2, tips_h + border*2);
	BoxBackground ol_tips_black(start_players_x + border,
		start_tips_y + border, tips_w, tips_h, 0, FL_BLACK);

	ChatBaseOutput **tips = new ChatBaseOutput*[TIPS_LINES];
	for(int i = 0, y = 0; i < TIPS_LINES; i++, y += chat_line_h) {
		tips[i] = new ChatBaseOutput(start_players_x + border + indent, 
			start_tips_y + border + y, tips_w - indent, chat_line_h);
	}
	tips[TIPS_LINES-6]->value("       Tips");
	tips[TIPS_LINE_GONLINE-1]->value("Online: ");
	tips[TIPS_LINE_RONLINE-1]->value("Online room: ");
	tips[TIPS_LINES-3]->value("");
	tips[TIPS_LINES-2]->value("ESC - exit");
	tips[TIPS_LINES-1]->value("ENTER - send msg");
	// End draw tips

	Client *user = new Client(SERVER_IP, SERVER_PORT, chat, players, tips);

	// Draw input:
	int start_input_y = spacing + border + chat_h + border + spacing;
	BoxBackground ol_input(spacing, start_input_y,
		input_w + border*2, input_h + border*2);
	ChatInput input(spacing + border, start_input_y + border,
		input_w, input_h, 0, user);
	// End draw input

	win->end();
	win->show();

	if(user->ConstuctorError()) {
		fl_choice("Application cannot connect to WantChat server\n\
			Sorry, try later :(", "Exit", 0, 0);
		win->hide();
	}

	while(user->Run())
	{
		Fl::wait(0);
		if(!win->visible_r())
			break;
	}
	return 0;
}