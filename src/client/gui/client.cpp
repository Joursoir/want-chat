#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Multiline_Input.H>

const int spacing = 20;
const int border = 2;

const int chat_w = 695;
const int chat_h = 440;
const int player_w = 225;
const int player_h = chat_h;
const int input_w = chat_w + border + spacing + border + player_w;
const int input_h = 55;

const int win_w = spacing + border + (input_w) + border + spacing;
const int win_h = spacing + border + chat_h + border +
	spacing + border + input_h + border + spacing;

int main(int argc, char **argv)
{
	Fl_Window *win = new Fl_Window(win_w, win_h, "WantChat");
	win->color(FL_BLACK);

	//

	Fl_Box *outline_chat = new Fl_Box(
		spacing, spacing, chat_w+border*2, chat_h+border*2);
	outline_chat->box(FL_FLAT_BOX);
	outline_chat->color(FL_WHITE);


	Fl_Multiline_Output *box_chat = new Fl_Multiline_Output(
		spacing+border, spacing+border, chat_w, chat_h);
	box_chat->box(FL_FLAT_BOX);
	box_chat->color(FL_BLACK);

	box_chat->textfont(FL_COURIER);
	box_chat->textsize(20);
	box_chat->textcolor(FL_WHITE);

	// only for test, please not going to beat me :)
	char buffer[2048] = "";
	for(int i = 0; i < 18; i++)
	{
		sprintf(buffer, "%sNickname: some text some text some text some text some te\n", buffer);
	}
	box_chat->value(buffer);

	//

	int start_players_x = spacing + border + chat_w + border + spacing;
	Fl_Box *outline_players = new Fl_Box(
		start_players_x, spacing, player_w+border*2, player_h+border*2);
	outline_players->box(FL_FLAT_BOX);
	outline_players->color(FL_WHITE);


	Fl_Multiline_Output *box_players = new Fl_Multiline_Output(
		start_players_x + border, spacing+border, player_w, player_h);
	box_players->box(FL_FLAT_BOX);
	box_players->color(FL_BLACK);

	box_players->textfont(FL_COURIER);
	box_players->textsize(20);
	box_players->textcolor(FL_WHITE);

	// only for test, please not going to beat me :) (x2)
	char buffer1[1024] = "";
	for(int i = 0; i < 18; i++)
	{
		sprintf(buffer1, "%sHackerspronickname\n", buffer1);
	}
	box_players->value(buffer1);

	//

	int start_input_y = spacing + border + chat_h + border + spacing;
	Fl_Box *outline_input = new Fl_Box(
		spacing, start_input_y, input_w+border*2, input_h+border*2);
	outline_input->box(FL_FLAT_BOX);
	outline_input->color(FL_WHITE);


	Fl_Multiline_Input *box_input = new Fl_Multiline_Input(spacing+border, 
		start_input_y+border, input_w, input_h);

	box_input->box(FL_FLAT_BOX);
	box_input->color(FL_BLACK);

	box_input->cursor_color(FL_WHITE);
	box_input->textfont(FL_COURIER);
	box_input->textsize(20);
	box_input->textcolor(FL_WHITE);

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