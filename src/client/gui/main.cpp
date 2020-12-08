#include <cerrno>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>

#include "OO_FLTK.hpp"
#include "../../config.hpp"
#include "Client.hpp"

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

	BoxOutline ol_chat(spacing, spacing, chat_w+border*2, chat_h+border*2);
	ChatBaseOutput *chat = new ChatBaseOutput(
		spacing+border, spacing+border, chat_w, chat_h);

	// only for test, please not going to beat me :)
	/*char buffer[2048] = "";
	for(int i = 0; i < 17; i++)
		sprintf(buffer, "%s\n", buffer);
	sprintf(buffer, "%sWelcome to WantChat! What is your name?\n", buffer);
	chat->value(buffer);*/

	//

	int start_players_x = spacing + border + chat_w + border + spacing;
	BoxOutline ol_players(start_players_x, spacing, player_w+border*2, player_h+border*2);
	ChatBaseOutput *players = new ChatBaseOutput(
		start_players_x + border, spacing+border, player_w, player_h);

	// only for test, please not going to beat me :) (x2)
	/*char buffer1[1024] = "";
	for(int i = 0; i < 18; i++)
		sprintf(buffer1, "%sHackerspronickname\n", buffer1);
	players->value(buffer1);*/

	//

	int start_input_y = spacing + border + chat_h + border + spacing;
	BoxOutline ol_input(spacing, start_input_y, input_w+border*2, input_h+border*2);
	ChatInput *input = new ChatInput(spacing+border, 
		start_input_y+border, input_w, input_h);

	//

	win->end();
	win->show();

	Client *user = new Client(SERVER_IP, SERVER_PORT, chat);
	if(user->ConstuctorError()) {
		perror("server");
		fl_choice("Server error", "Exit", 0, 0);
		win->hide();
	}

	while(Fl::wait())
	{
		user->Run();
	}

	return 0;
}