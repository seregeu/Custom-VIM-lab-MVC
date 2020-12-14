#include "vim_like.h"
View::View(VimModel* c_model, Controller* c_controller, Target* adapter) {
	model = c_model;
	controller = c_controller;
	_adapter = adapter;
	adapter->AdapterResizeTerm(F_ROWS, F_COLUMNS);
	text_window = TextWindow(F_ROWS - CONS_H, F_COLUMNS, 0, 0, _adapter);
	cons_window = ConsWindow(CONS_H, F_COLUMNS, F_ROWS - CONS_H, 0, _adapter);
	statusbar = StatusBar(CONS_H, F_COLUMNS, F_ROWS - CONS_H, 0, _adapter);
	model->addObserver(this);
	c_posy = 0;
	c_posx = 0;
	t_posy = 0;
	t_posx = 0;
	scrolled_up = 0;
}
void View::Start() {
	int ch;
	noecho();
	keypad(stdscr, TRUE);
	while (true) {
		ch = getch();
		controller->SendLet(ch);
	}
}
void View::Show(const unsigned char window) {
	switch (window) {
	case CONS:
		cons_window.Show(_adapter);
		break;
	case TEXT:
		text_window.Show(_adapter);
		statusbar.Show(_adapter);
		break;
	case WSER:
		text_window.Show(_adapter);
		cons_window.Show(_adapter);
		break;
	case SEAR:
		statusbar.Show(_adapter);
	default:
		break;
	}
}
void View::MoveCR() {
	if (c_posx + 1 < F_COLUMNS) {
		c_posx++;
	}
	else if (c_posy < F_ROWS) {
		c_posy++;
		c_posx = 0;
	}
}
void View::MoveCL() {
	if (c_posx > 0) {
		c_posx--;
	}
	else if (c_posy > F_ROWS - CONS_H) {
		c_posy--;
		c_posx = F_COLUMNS - 1;
	}
}
void View::MoveCD() {
	if (c_posy < F_ROWS) {
		c_posy++;
	}
}
void View::MoveCU() {
	if (c_posy > F_ROWS - CONS_H) {
		c_posy--;
	}
}


int View::ScrollDown(MyString& content, const size_t n_len) {
	size_t ind = n_len / F_COLUMNS + 1;
	size_t sav = ind;
	size_t pr_ind = text_window.GetStart();
	size_t temp = 0;
	size_t new_ind = 0;
	for (new_ind = pr_ind; ind != 0; new_ind++) {
		temp++;
		if (content[new_ind] == '\n') {
			ind--;
			temp = 0;
		}
		if (temp % F_COLUMNS == 0 && temp != 0) {
			ind--;
		}
	}
	text_window.SetStart(new_ind);
	controller->UpScrollFlag(TRUE);
	return sav;
}
int View::ScrollUp(MyString& content, const size_t n_len, const size_t offset) {
	int ind = -t_posy;
	size_t sav = ind;
	size_t pr_ind = text_window.GetStart() - 1;
	if (n_len == 1) {
		text_window.SetStart(pr_ind);
		return 1;
	}
	size_t temp = 0;
	size_t new_ind = 0;
	for (new_ind = pr_ind; ind > 0; new_ind--) {
		temp++;
		if (content[new_ind] == '\n') {
			ind--;
			temp = 0;
		}
		if (temp % F_COLUMNS == 0 && temp != 0) {
			ind--;
		}
	}
	new_ind++;
	while (content[new_ind - 1] != '\n' && new_ind != 0) {
		new_ind--;
	}
	text_window.SetStart(new_ind);
	scrolled_up += sav;
	if (scrolled_up >= F_ROWS) {
		controller->UpScrollFlag(TRUE);
	}
	return sav;
}

void View::MoveTU(const int str_index, const size_t r_len, const size_t offset, const size_t n_len, const int str_n) {
	int h_n = 0;
	h_n += r_len / F_COLUMNS;  //begin of this word;
	h_n += n_len / F_COLUMNS;
	if (n_len % F_COLUMNS != 0) h_n++;//begin of next word
	int value = offset;
	if (value > n_len) value = n_len;
	h_n -= value / F_COLUMNS;
	if (n_len >= offset) {
		t_posx = (offset % F_COLUMNS);
	}
	else {
		t_posx = (n_len % F_COLUMNS) - 2;
		if (str_n == 0)t_posx++;
	}
	if (n_len == 1) t_posx = 0;
	t_posy -= h_n;
}

void View::MoveTD(const int str_index, const size_t r_len, const size_t offset, const size_t n_len) {
	scrolled_up = 0;
	t_posy += (r_len + t_posx) / F_COLUMNS;
	if ((r_len + t_posx + 1) % F_COLUMNS >= 0) t_posy += 1;
	//c_posy += offset / F_COLUMNS;
	if (n_len > offset) {
		t_posx = (offset % F_COLUMNS);
		t_posy += offset / F_COLUMNS;
	}
	else {
		if (n_len > 0) {
			t_posx = (n_len % F_COLUMNS) - 1;
		}
		else {
			t_posx = (n_len % F_COLUMNS);
		}
		t_posy += n_len / F_COLUMNS;
	}

}
void View::MoveTR() {
	if (t_posx + 1 < F_COLUMNS) {
		t_posx++;
	}
	else if (t_posy < F_ROWS) {
		t_posy++;
		t_posx = 0;
	}
}
void View::MoveTL() {
	scrolled_up = 0;
	if (t_posx > 0) {
		t_posx--;
	}
	else if (t_posy > 0) {
		t_posy--;
		t_posx = F_COLUMNS - 1;
	}
}

void View::UpdateCom(MyString& content, const int symb, const int str_index, const unsigned char win) {
	if (content.length() == 1 && (symb == ':' || symb == '?' || symb == '/')) {
		c_posx = 0;
		c_posy = F_ROWS - CONS_H;
	}
	cons_window.PutData(content,_adapter);
	Show(win);
	switch (symb) {
	case KEY_LEFT:
		MoveCL();
		break;
	case KEY_RIGHT:
		MoveCR();
		break;
	case KEY_UP:
		MoveCU();
		break;
	case KEY_DOWN:
		MoveCD();
		break;
	case BACKSPACE:
		MoveCL();
		break;
	case DELETE:
		if (content.length() <= c_posx + F_COLUMNS * (c_posy - F_ROWS + CONS_H)) {
			MoveCL();
		}
		//else nothing to move
		break;
	case ENTER:
		if (win != WSER) {
			c_posy = 0;
			c_posx = 0;
		}
		break;
	default:
		if ((c_posx < F_COLUMNS && c_posy < F_ROWS) && (content.length() > 1)) {
			MoveCR();
		}
		break;
	}
	if (!(symb == ENTER && win == WSER))
		move(c_posy, c_posx);
	Show(TEXT);
}

void View::NewFile() {
	text_window.Refresh();
	t_posy = 0;
	t_posx = 0;
}
void View::BackToText() {
	move(t_posy, t_posx);
	Show(TEXT);
}
void View::SetNewStart(const size_t index) {
	t_posx = 0;
	t_posy = 0;
	move(t_posy, t_posx);
	text_window.SetStart(index);
	Show(TEXT);
	scrolled_up = 0;
	controller->UpScrollFlag(FALSE);
}

void View::UpdateText(MyString& content, const int symb, const int str_index, const size_t offset, const size_t r_len, const size_t n_len, const size_t string_num) {
	text_window.PutData(content, _adapter);
	Show(TEXT);
	switch (symb) {
	case KEY_LEFT:
		MoveTL();
		break;
	case KEY_RIGHT:
		MoveTR();
		break;
	case KEY_UP:
		MoveTU(str_index, r_len, offset, n_len, string_num);
		break;
	case KEY_DOWN:
		MoveTD(str_index, r_len, offset, n_len);
		break;
	case BACKSPACE:
		MoveTL();
		break;
	case DELETE:
		break;
	case ENTER:
		t_posy = 0;
		t_posx = 0;
		move(t_posy, t_posx);
		break;
	default:
		break;
	}
	if (t_posy >= F_ROWS - CONS_H) {
		t_posy -= ScrollDown(content, n_len);
		text_window.PutData(content, _adapter);
		Show(TEXT);
	}
	if (t_posy < 0) {
		t_posy += ScrollUp(content, n_len, offset);
		if (n_len / F_COLUMNS >= offset / F_COLUMNS) {
			t_posy += offset / F_COLUMNS;
		}
		else {
			t_posy += n_len / F_COLUMNS;
		}
		text_window.PutData(content, _adapter);
		Show(TEXT);
	}
	//moving cursor
	move(t_posy, t_posx);
}
Window::Window() {
	rows = 0;
	collumns = 0;
	window = nullptr;
}

void ConsWindow::PutData(MyString& content, Target* adapter) {
	adapter->AdapterWclear(window);
	mvwprintw(window, 0, 0, "%s", content.c_str());
}

WINDOW* Window::CreateNewWin(const int height, const int width, const int starty, const int startx, Target* adapter)
{
	WINDOW* local_win;
	local_win = adapter->AdapterNewWin(height, width, starty, startx);
	adapter->AdapterInitPair(1, COLOR_BLACK, COLOR_WHITE);
	adapter->AdapterWbkgd(local_win, COLOR_PAIR(1) | A_BLINK);
	return local_win;
}

void TextWindow::SetStart(const size_t new_start_ind) {
	text_out_strt = new_start_ind;
}

size_t TextWindow::GetStart() {
	return text_out_strt;
}

void TextWindow::Show(Target* adapter) {
	adapter->AdapterWrefresh(window);
}

void TextWindow::Refresh() {
	start_ind = 0;
	c_y = 0;
	c_x = 0;
	text_out_strt = 0;
}

void TextWindow::PutData(MyString& content,Target* adapter) {
	adapter->AdapterWclear(window);
	c_y = 0;
	c_x = 0;
	size_t i = text_out_strt;
	while ((c_y < F_ROWS - CONS_H || c_x < F_COLUMNS) && (content[i] != '\0')) {
		mvwprintw(window, c_y, c_x, "%c", content[i]);
		if (content[i] == '\n') {
			c_y++;
			c_x = 0;
		}
		else c_x++;
		i++;
		if (c_x == F_COLUMNS) {
			c_y++;
			c_x = 0;
		}
	}
}

TextWindow::TextWindow(const int height, const int width, const int starty, const int startx, Target* adapter) {
	c_x = 0;
	c_y = 0;
	start_ind = 0;
	text_out_strt = 0;
	window = CreateNewWin(height, width, starty, startx, adapter);
	wrefresh(window);
}

TextWindow::TextWindow() {
	c_x = 0;
	c_y = 0;
	start_ind = 0;
	text_out_strt = 0;
}

ConsWindow::ConsWindow(const int height, const int width, const int starty, const int startx, Target* adapter) {
	window = CreateNewWin(height, width, starty, startx, adapter);
	wrefresh(window);
}

ConsWindow::ConsWindow() {
}

void ConsWindow::Show(Target* adapter) {
	adapter->AdapterWrefresh(window);
}

StatusBar::StatusBar() {
	window = nullptr;
	filename = nullptr;
	N_string = nullptr;
}

StatusBar::StatusBar(const int height, const int width, const int starty, const int startx, Target* adapter) {
	window = CreateNewWin(BOTTOM_PANEL_H, MODE_L, starty, startx, adapter);
	wrefresh(window);
	filename = CreateNewWin(BOTTOM_PANEL_H, NAME_L, starty, startx + MODE_L, adapter);
	wrefresh(filename);
	N_string = CreateNewWin(BOTTOM_PANEL_H, NUMBER_L, starty, startx + MODE_L + NAME_L, adapter);
	wrefresh(N_string);
}

void View::UpdateStat(MyString& filename, size_t cur_str, size_t str_am) {
	statusbar.PutData(filename, cur_str, str_am,_adapter);
}

void View::UpdateMode(Mode& mode) {
	statusbar.PutMode(mode, _adapter);
}

void StatusBar::Show(Target* adapter) {
	adapter->AdapterWrefresh(window);
	adapter->AdapterWrefresh(filename);
	adapter->AdapterWrefresh(N_string);
}
void StatusBar::PutData(MyString& file_name, const size_t cur_str, const size_t str_am, Target* adapter) {
	adapter->AdapterWclear(N_string);
	adapter->AdapterWclear(filename);
	mvwprintw(N_string, 0, 0, " %d / %d ", cur_str, str_am);
	mvwprintw(filename, 0, 0, " %s ", file_name.c_str());
}
void StatusBar::PutMode(Mode& mode,Target* adapter) {
	adapter->AdapterWclear(window);
	switch (mode) {
	case Mode::NAVI:
		mvwprintw(window, 0, 0, " MODE: NAVIGATION ");
		break;
	case Mode::COMMAND:
		mvwprintw(window, 0, 0, " MODE: COMMAND ");
		break;
	case Mode::SEARCH:
		mvwprintw(window, 0, 0, " MODE: SEARCH ");
		break;
	case Mode::WRITE:
		mvwprintw(window, 0, 0, " MODE: WRITE ");
		break;
	case Mode::HELP:
		adapter->AdapterWclear(N_string);
		adapter->AdapterWclear(filename);
		break;
	}
}