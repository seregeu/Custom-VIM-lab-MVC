#include "vim_like.h"

int main()
{
    initscr(); // Initialize ncurses
    start_color();
    VimModel model;
	Controller contoller(&model);
	Adapter adapter;
    View view(&model,&contoller,&adapter);
    //view.ChangeMode(View::Mode::COMMAND);
    view.Show(TEXT);
	view.Start();
    endwin(); //Finish work with ncurses
	return 0;
}
