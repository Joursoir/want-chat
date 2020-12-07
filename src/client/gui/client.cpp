#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Input.H>

#include "OO_FLTK.hpp"

const int spacing = 20;
const int border = 2;

const int chat_w = 695;
const int chat_h = 440;
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
	BoxOutline *ol_chat = new BoxOutline(
		spacing, spacing, chat_w+border*2, chat_h+border*2);
	ChatBaseOutput *chat = new ChatBaseOutput(
		spacing+border, spacing+border, chat_w, chat_h);

	// only for test, please not going to beat me :)
	char buffer[2048] = "";
	for(int i = 0; i < 18; i++)
		sprintf(buffer, "%sNickname: some text some text some text some text some te\n", buffer);
	chat->value(buffer);

	//

	int start_players_x = spacing + border + chat_w + border + spacing;
	BoxOutline *ol_players = new BoxOutline(
		start_players_x, spacing, player_w+border*2, player_h+border*2);
	ChatBaseOutput *players = new ChatBaseOutput(
		start_players_x + border, spacing+border, player_w, player_h);

	// only for test, please not going to beat me :) (x2)
	char buffer1[1024] = "";
	for(int i = 0; i < 18; i++)
		sprintf(buffer1, "%sHackerspronickname\n", buffer1);
	players->value(buffer1);

	//

	int start_input_y = spacing + border + chat_h + border + spacing;
	BoxOutline *outline_input = new BoxOutline(
		spacing, start_input_y, input_w+border*2, input_h+border*2);
	ChatInput *input = new ChatInput(spacing+border, 
		start_input_y+border, input_w, input_h);
	//

	win->end();
	win->show();
	return Fl::run();
}

// 59 - 1 - 20

/* I need this widgets:
- Button
- Inputbox
- Box */