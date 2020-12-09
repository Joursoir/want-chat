#include <cerrno>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>

#include "OO_FLTK.hpp"
#include "../../config.hpp"
#include "Client.hpp"

const int spacing = 20;
const int border = 2;
const int indent = 5;

const int chat_w = 695;
const int chat_h = 440;
const int chat_line_h = 440 / lines_in_chat;
const int player_w = 225;
const int player_h = chat_h;
const int input_w = chat_w + border + spacing + border + player_w;
const int input_h = 40; // 55

const int win_w = spacing + border + (input_w) + border + spacing;
const int win_h = spacing + border + chat_h + border +
	spacing + border + input_h + border + spacing;


int main(int argc, char **argv)
{
	Fl_Window *win = new Fl_Window(win_w, win_h, "WantChat");
	win->color(FL_BLACK);

	//

	BoxBackground ol_chat(spacing, spacing,
		chat_w+border*2, chat_h+border*2);
	BoxBackground ol_black(spacing+border, spacing+border,
		chat_w, chat_h, 0, FL_BLACK);

	ChatBaseOutput **chat = new ChatBaseOutput*[18];
	int y = 0;
	for(int i = 0 ; i < 18; i++) {
		chat[i] = new ChatBaseOutput(spacing+border+indent, 
			spacing+border+y, chat_w-indent, chat_line_h);
		y += chat_line_h;
	}

	//

	int start_players_x = spacing + border + chat_w + border + spacing;
	BoxBackground ol_players(start_players_x, spacing, player_w+border*2, player_h+border*2);
	ChatBaseOutput *players = new ChatBaseOutput(
		start_players_x + border, spacing+border, player_w, player_h);

	//

	Client *user = new Client(SERVER_IP, SERVER_PORT, chat);

	int start_input_y = spacing + border + chat_h + border + spacing;
	BoxBackground ol_input(spacing, start_input_y, input_w+border*2, input_h+border*2);
	ChatInput input(spacing+border, start_input_y+border, input_w, input_h, 0, user);

	//

	win->end();
	win->show();

	if(user->ConstuctorError()) {
		fl_choice("Server error", "Exit", 0, 0);
		win->hide();
	}

	while(user->Run())
	{
		Fl::wait(0);
	}

	return 0;
}