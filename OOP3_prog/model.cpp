#include "vim_like.h"

//here model
VimModel::VimModel() {
	ChangeMode(Mode::NAVI);
	text_index = 0;
	cons_index = 0;
	strings_amount = 0;
	text_data;
	cons_data;
	filename_data;
	N_string_data;
	string_num = 0;
	text_data.clear();
	pressed_buttons.clear();
	opened_file_name.clear();
	scroll_flag = FALSE;
}
void VimModel::addObserver(Observer* observer)
{
	_observers.push_back(observer);
}
void VimModel::NotifyUpdateCom(MyString& content, const int symb, const int str_index,const unsigned char win)
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->UpdateCom(content, symb, str_index,win);
}

void VimModel::NotifyUpdateText(MyString& content, const int symb, const int str_index, const size_t offset, const size_t r_len, const size_t n_len, const size_t string_num)
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->UpdateText(content, symb, str_index, offset, r_len, n_len,string_num);
}
void VimModel::NotifyNewFile() {
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->NewFile();
}
void VimModel::NotifyBackToText() {
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->BackToText();
}
void VimModel::NotifySetNewStart(const size_t index) {
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->SetNewStart(index);
}
void VimModel::NotifyUpdateStat(MyString& filename,const size_t cur_str,const size_t str_am) {
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->UpdateStat(filename, cur_str, str_am);
}
void VimModel::NotifyUpdateMode(Mode& mode) {
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->UpdateMode(mode);
}
void VimModel::ChangeMode(Mode n_mode) {
	mode = n_mode;
}
bool  VimModel::RecogCom(MyString& command) {
	char ch;
	if (command.substr(0, 3) == ":o ") {
		std::ifstream file;
		opened_file_name.clear();
		opened_file_name.append(command.substr(3));
		file.open(opened_file_name.c_str());
		if (!file.is_open()) {
			return false;
		}
		is_modify = FALSE;
		NotifyNewFile();
		ChangeMode(Mode::NAVI);
		NotifyUpdateMode(mode);
		text_data.clear();
		offset = 0;
		MyString temp;
		do {
			file >> temp;
			text_data.append(temp);
			text_data.append(1,'\n');
			strings_amount++;
		} while (!file.eof());
		text_index = 0;
		file.close();
		return true;
	}
	else if (command==":x"|| command == ":w" || command == ":wq!") {
		std::ofstream file(opened_file_name.c_str());
		if (!file.is_open()) {
			return false;
		}
		file << text_data;
		file.close();
		if (command == ":x"|| command == ":wq!") {
			endwin();
			exit(0);
		}
		return true;
	}
	else if (command.substr(0, 3) == ":w ") {
		std::ofstream file(command.substr(3).c_str());
		if (!file.is_open()) {
			return false;
		}
		file << text_data;
		file.close();
		return true;
	}
	else if (command==":q") {
		if (is_modify == FALSE) {
			endwin();
			exit(0);
		}
		else return false;
	}
	else if (command == ":q!") {
			endwin();
			exit(0);
	}
	else if (command[1]<='9'&& command[1]>='0') {
		ChangeMode(Mode::NAVI);
		GotoStringByNum(command.substr(1));
		ChangeMode(Mode::COMMAND);
		return true;
	}
	else if (command == ":h") {
		std::ifstream file;
		file.open("help.txt");
		if (!file.is_open()) {
			return false;
		}
		buffer.clear();
		MyString temp;
		do {
			file >> temp;
			buffer.append(temp);
			buffer.append(1, '\n');
		} while (!file.eof());
		file.close();
		ChangeMode(Mode::HELP);
		NotifyUpdateMode(mode);
		NotifyUpdateText(buffer, 0, 0, 0, 0, 0, 0);
		return true;
	}
	return false;
}

size_t VimModel::GetStrLenR(const int str_index) {
	size_t j = 0;
	for (int i = str_index; text_data[i + 1] != '\0' && text_data[i + 1] != '\n' && text_data[i + 1] != EOF; i++, j++);
	//printf("   len = %d   ",j);
	return j;
}

size_t VimModel::GetStrLenL(const int str_index) {
	size_t j = 0;
	for (int i = str_index; text_data[i] != '\0' && text_data[i] != '\n' && i != 0; i--, j++);
	return j;
}

size_t VimModel::SerDownPos(MyString& text_data, const int str_index, const int word_len) {
	size_t j = str_index + GetStrLenR(str_index) + 1;
	GetStrLenR(j) > offset ? j += offset : j += GetStrLenR(j);
	if (string_num==0&&offset< GetStrLenR(j)) {
		j++;
	}
	else
	if (text_data[j] == '\n'&& text_data[j+1] != '\n') j++;
	//printf("  j %d   ", j);
	return j;
}

int VimModel::SerUpPos(MyString& text_data, const int str_index, const int word_len,const int offset) {
	if (string_num==0) {
		return -1;
	}
	size_t j = str_index - GetStrLenL(str_index)-1;
	j -= GetStrLenL(j);
	GetStrLenR(j) > offset ? j += offset+1 : j += GetStrLenR(j);
	if (string_num == 1)j--;
	return j;
}

void VimModel::SetScrlFlag(bool value) {
	scroll_flag = value;
}

bool VimModel::IsSpecSymb(const char symb) {
	if (symb == ';' || symb == ' '||
		symb == '%' || symb == '$'||
		symb == '#' || symb == '¹'||
		symb == '*' || symb == '-'|| 
		symb == '(' || symb == ')'|| 
		symb == '-' || symb == '=') {
		return TRUE;
	}
	return FALSE;
}

void VimModel::SendC(const int symb) {
	switch (mode) {
	case Mode::COMMAND:
		SendCom(symb);
		break;
	case Mode::NAVI:
		SendNav(symb);
		break;
	case Mode::SEARCH:
		SendSear(symb);
		break;
	case Mode::WRITE:
		SendWrite(symb);
		break;
	case Mode::HELP:
		SendHelp(symb);
		break;
	default:
		break;
	}
}

void VimModel:: SendCom(const int symb){
	switch (symb) {
	case KEY_LEFT:
		if (cons_index > 0) {
			cons_index--;
		}
		else {
			return;
		}
		break;
	case KEY_RIGHT:
		if (cons_index < cons_data.length()) {
			cons_index++;
		}
		else {
			return;
		}
		break;
	case KEY_UP:
		if ((cons_index > max_com_len / 2) && (cons_data.length() > max_com_len / 2)) {
			cons_index -= max_com_len / 2;
		}
		else {
			return;
		}
		break;
	case KEY_DOWN:
		if ((cons_data.length() > max_com_len / 2) && (cons_data.length() < max_com_len) && (cons_index <= cons_data.length() % (max_com_len / 2))) {
			cons_index += max_com_len / 2;
		}
		else {
			return;
		}
		break;
	case BACKSPACE:
		if (cons_data.length() > 0) {
			if (cons_index == 1 && cons_data.length() > 1) {
				return;
			}
			if (cons_index > 0) {
				cons_data.erase(cons_index - 1, 1);
				cons_index--;
			}
		}
		if (cons_data.length() == 0) {
			ChangeMode(Mode::NAVI);
			NotifyUpdateMode(mode);
			cons_data.clear();
			NotifyBackToText();
			return;
		}
		break;
	case DELETE:
		if (cons_data.length() > 1 && cons_index < cons_data.length()) {
			cons_data.erase(cons_index, 1);
		}
		else {
			cons_data.erase(cons_index - 1, 1);
		}
		if (cons_index > cons_data.length()) {
			cons_index--;
		}
		break;
	case ENTER:
		if (cons_data.length() <= 1) return;
		if (RecogCom(cons_data) == true) {
			if (cons_data.substr(0, 3) == ":o ") {
				NotifyUpdateCom(cons_data, symb, cons_index, CONS);
				ChangeMode(Mode::NAVI);
				NotifyUpdateMode(mode);
				NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
				NotifyUpdateText(text_data, symb, text_index, 0, 0, 0, string_num);
				return;
			}
			else if (cons_data == ":h") {
				return;
			}
			else {
				cons_data.clear();
				ChangeMode(Mode::NAVI);
				NotifyUpdateMode(mode);
				NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
				NotifyBackToText();
				return;
			}
		}
		else {
			if (cons_data != ":q") {
				cons_data.clear();
				cons_data.append("Oops, command was not executed!");
				NotifyUpdateCom(cons_data, symb, 1, CONS);
				getch();
			}
			else {
				cons_data.clear();
				cons_data.append("File was modifyed, use q!");
				NotifyUpdateCom(cons_data, symb, 1, CONS);
				getch();
			}
			cons_data.clear();
			ChangeMode(Mode::NAVI);
			NotifyUpdateMode(mode);
			NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
			NotifyBackToText();
			return;
		}
		break;
	default:
		if (cons_data.length() < max_com_len) {
			cons_data.insert(cons_index, 1, symb);
			cons_index++;
		}
		break;
	}
	NotifyUpdateCom(cons_data, symb, cons_index, CONS);
	return;
}
void VimModel:: SendNav(const int symb){
	int sav = 0;
	int up_len = 0;
	if (pressed_buttons.length() > 0) {
		if (pressed_buttons[pressed_buttons.length() - 1] >= '0' && pressed_buttons[pressed_buttons.length() - 1] <= '9') {
			if (symb >= '0' && symb <= '9') {
				pressed_buttons.append(1, symb);
				return;
			}
			else if (symb == 'G') {
				GotoStringByNum(pressed_buttons);
				pressed_buttons.clear();
				return;
			}
			else {
				pressed_buttons.clear();
			}
		}
		else if (pressed_buttons == "r") {
			text_data.erase(text_index, 1);
			text_data.insert(text_index, 1, symb);
			NotifyUpdateMode(mode);
			NotifyUpdateText(text_data, 0, text_index, offset, GetStrLenR(text_index), 0, string_num);
			pressed_buttons.clear();
			return;
		}
		else
			switch (symb) {
			case 'g':
				if (pressed_buttons[pressed_buttons.length() - 1] == 'g') {
					GoToFileStart();
					pressed_buttons.clear();
					NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
					NotifyUpdateText(text_data, symb, text_index, offset, GetStrLenR(text_index), 0, string_num);
					return;
				}
				else {
					pressed_buttons.clear();
				}
				break;
			case 'i':
				if (pressed_buttons[pressed_buttons.length() - 1] == 'd') {
					pressed_buttons.append(1, symb);
					return;
				}
				else {
					pressed_buttons.clear();
				}
				break;
			case 'w':
				if (pressed_buttons == "di") {
					DeleteWordHere();
					NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
					NotifyUpdateText(text_data, symb, text_index, offset, GetStrLenR(text_index), 0, string_num);
					pressed_buttons.clear();
					is_modify = TRUE;
					return;
				}
				else if (pressed_buttons == "y") {
					CopyCurWord();
					pressed_buttons.clear();
					return;
				}
				else {
					pressed_buttons.clear();
				}
				break;
			case 'd':
				if (pressed_buttons == "d") {
					DeleteThisString();

					NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
					NotifyUpdateText(text_data, symb, text_index, offset, GetStrLenR(text_index), 0, string_num);
					pressed_buttons.clear();
					is_modify = TRUE;
					return;
				}
				else {
					pressed_buttons.clear();
				}
				break;
			case 'y':
				if (pressed_buttons == "y") {
					CopyCurString();
					pressed_buttons.clear();
					return;
				}
				else {
					pressed_buttons.clear();
				}
				break;
			default:
				pressed_buttons.clear();
				break;
			}
	}
	up_len = 0;
	switch (symb) {
	case ':':
		ChangeMode(Mode::COMMAND);
		NotifyUpdateMode(mode);
		cons_index = 1;
		cons_data.clear();
		cons_data.append(":");
		NotifyUpdateCom(cons_data, symb, cons_index, CONS);
		return;
		break;
	case '/':
		ChangeMode(Mode::SEARCH);
		NotifyUpdateMode(mode);
		search_index = 1;
		search_data.clear();
		search_data.append("/");
		NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
		NotifyUpdateCom(search_data, symb, search_index, WSER);
		return;
		break;
	case '?':
		ChangeMode(Mode::SEARCH);
		NotifyUpdateMode(mode);
		search_index = 1;
		search_data.clear();
		search_data.append("?");
		NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
		NotifyUpdateCom(search_data, symb, search_index, WSER);
		return;
		break;
	case KEY_LEFT:
		if (GetStrLenL(text_index) > 1 || (string_num == 0 && text_index > 0)) {
			text_index--;
			offset--;
		}
		else {
			return;
		}
		break;
	case KEY_RIGHT:
		if (GetStrLenR(text_index) > 0) {
			text_index++;
			offset++;
		}
		else {
			return;
		}
		break;
	case KEY_UP:
		sav = GetStrLenL(text_index);
		if (SerUpPos(text_data, text_index, sav, offset) < 0) {
			return;
		}
		text_index = SerUpPos(text_data, text_index, sav, offset);
		up_len = GetStrLenR(text_index) + GetStrLenL(text_index);
		string_num--;
		if ((up_len > 0 || text_index != SerUpPos(text_data, text_index, text_index, offset)) && text_index >= 0) {
			NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
			NotifyUpdateText(text_data, symb, text_index, offset, sav, up_len + 1, string_num);
		}
		return;
		break;
	case KEY_DOWN:
		sav = GetStrLenR(text_index);
		if (SerDownPos(text_data, text_index, sav) >= text_data.length()) {
			//printf("  1  %d  2  %d", SerDownPos(text_data, text_index, sav), text_data.length());
			scroll_flag = TRUE;
			return;
		}
		text_index = SerDownPos(text_data, text_index, sav);
		string_num++;
		//printf("   %c ",text_data[text_index]);
		NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
		NotifyUpdateText(text_data, symb, text_index, offset, sav, GetStrLenR(text_index) + GetStrLenL(text_index), string_num);
		return;
		break;
	case BACKSPACE:
		return;
		break;
	case DELETE:
		return;
		break;
	case ENTER:
		return;
		break;
	case '0':
		MovToStringStart();
		break;
	case '^':
		MovToStringStart();
		break;
	case 'w':
		if (text_index == text_data.length() - 1) {
			return;
		}
		MovToNextWordEnd();
		break;
	case '$':
		MovToStringEnd();
		break;
	case 'b':
		MovToPrevWordStart();
		break;
	case 'x':
		if (text_data[text_index] != '\0') {
			text_data.erase(text_index, 1);
		}
		break;
	case 'G':
		GoToFileEnd();
		break;
	case KEY_NPAGE:
		PageDown();
		break;
	case KEY_PPAGE:
		if (string_num == 0) return;
		PageUP();
		break;
	case 'p':
		if (buffer.length() > 0) {
			if (text_data[text_index] != '\n') {
				text_data.insert(text_index, buffer.c_str());
			}
			else {
				text_data.insert(text_index + 1, buffer.c_str());
			}
		}
		break;
	case 'i':
		ChangeMode(Mode::WRITE);
		NotifyUpdateMode(mode);
		is_modify = TRUE;
		break;
	case 'I':
		MovToStringStart();
		ChangeMode(Mode::WRITE);
		NotifyUpdateMode(mode);
		is_modify = TRUE;
		break;
	case 'A':
		MovToStringEnd();
		ChangeMode(Mode::WRITE);
		NotifyUpdateMode(mode);
		is_modify = TRUE;
		break;
	case 'S':
		DeleteThisString();
		ChangeMode(Mode::WRITE);
		NotifyUpdateMode(mode);
		is_modify = TRUE;
		break;
	case 'r':
		ChangeMode(Mode::WRITE);
		NotifyUpdateMode(mode);
		if (pressed_buttons.length() == 0) {
			pressed_buttons.append(1, symb);
		}
		ChangeMode(Mode::NAVI);
		is_modify = TRUE;
		break;
	default:
		if (pressed_buttons.length() == 0) {
			pressed_buttons.append(1, symb);
		}
		break;
	}
	NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
	NotifyUpdateText(text_data, symb, text_index, offset, GetStrLenR(text_index), 0, string_num);
}
void VimModel::SendSear(const int symb) {
	if (search_data.length() > 0) {
		switch (symb) {
		case KEY_LEFT:
			if (search_index > 0) {
				search_index--;
			}
			else {
				return;
			}
			break;
		case KEY_RIGHT:
			if (search_index < search_data.length()) {
				search_index++;
			}
			else {
				return;
			}
			break;
		case KEY_UP:
			if ((search_index > max_com_len / 2) && (search_data.length() > max_com_len / 2)) {
				search_index -= max_com_len / 2;
			}
			else {
				return;
			}
			break;
		case KEY_DOWN:
			if ((search_data.length() > max_com_len / 2) && (search_data.length() < max_com_len) && (search_index <= search_data.length() % max_com_len / 2)) {
				cons_index += max_com_len / 2;
			}
			else {
				return;
			}
			break;
		case BACKSPACE:
			if (search_data.length() > 0) {
				if (search_index == 1 && search_data.length() > 1) {
					return;
				}
				if (search_index > 0) {
					search_data.erase(search_index - 1, 1);
					search_index--;
				}
			}
			if (search_data.length() == 0) {
				ChangeMode(Mode::NAVI);
				search_data.clear();
				NotifyBackToText();
				return;
			}
			break;
		case DELETE:
			if (search_data.length() > 1 && search_index < search_data.length()) {
				search_data.erase(search_index, 1);
			}
			else {
				search_data.erase(search_index - 1, 1);
			}
			if (search_index > search_data.length()) {
				search_index--;
			}
			break;
		case ESC:
			ChangeMode(Mode::NAVI);
			NotifyUpdateMode(mode);
			NotifyBackToText();
			return;
			break;
		case ENTER:
			if (search_data.length() <= 1) return;
			switch (search_data[0]) {
			case '/':
				SearchToEnd(search_data);
				prsearch_data.clear();
				prsearch_data.append(search_data);
				search_data.clear();
				NotifyUpdateMode(mode);
				break;
			case '?':
				SearchToStart(search_data);
				prsearch_data.clear();
				prsearch_data.append(search_data);
				search_data.clear();
				NotifyUpdateMode(mode);
				break;
			}
			break;
		default:
			if (search_data.length() < max_com_len) {
				search_data.insert(search_index, 1, symb);
				search_index++;
			}
			break;
		}
		NotifyUpdateCom(search_data, symb, search_index, WSER);
		return;
	}
	else {
		switch (symb) {
		case 'n':
			if (prev_search == FALSE) {
				SearchToEnd(prsearch_data);
			}
			else {
				SearchToStart(prsearch_data);
			}
			break;
		case 'N':
			if (prev_search == TRUE) {
				SearchToEnd(prsearch_data);
			}
			else {
				SearchToStart(prsearch_data);
			}
			break;
		case ESC:
			ChangeMode(Mode::NAVI);
			NotifyBackToText();
			NotifyUpdateMode(mode);
			return;
			break;
		}
	}
}
void VimModel::SendHelp(const int symb) {
	switch (symb) {
	case ESC:
		ChangeMode(Mode::NAVI);
		NotifyUpdateMode(mode);
		NotifyBackToText();
		SendC(0);
		return;
		break;
	default:
		break;
	}
}
void VimModel::SendWrite(const int symb) {
	int sav = 0;
	int up_len = 0;
	switch (symb) {
	case KEY_LEFT:
		if (GetStrLenL(text_index) > 1 || (string_num == 0 && text_index > 0)) {
			text_index--;
			offset--;
		}
		else {
			return;
		}
		break;
	case KEY_RIGHT:
		if (GetStrLenR(text_index) > 0) {
			text_index++;
			offset++;
		}
		else {
			return;
		}
		break;
	case KEY_UP:
		sav = GetStrLenL(text_index);
		if (SerUpPos(text_data, text_index, sav, offset) < 0) {
			return;
		}
		text_index = SerUpPos(text_data, text_index, sav, offset);
		up_len = GetStrLenR(text_index) + GetStrLenL(text_index);
		string_num--;
		if ((up_len > 0 || text_index != SerUpPos(text_data, text_index, text_index, offset)) && text_index >= 0) {
			NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
			NotifyUpdateText(text_data, symb, text_index, offset, sav, up_len + 1, string_num);
		}
		return;
		break;
	case KEY_DOWN:
		sav = GetStrLenR(text_index);
		if (SerDownPos(text_data, text_index, sav) >= text_data.length()) {
			scroll_flag = TRUE;
			return;
		}
		text_index = SerDownPos(text_data, text_index, sav);
		string_num++;
		NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
		NotifyUpdateText(text_data, symb, text_index, offset, sav, GetStrLenR(text_index) + GetStrLenL(text_index), string_num);
		return;
		break;
	case BACKSPACE:
		if (text_data.length() > 0) {
			if (text_data[text_index] == '\n') {
				strings_amount--;
				SendC(KEY_UP);
				MovToStringEnd();
				text_data.erase(text_index + 1, 1);
				NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
				NotifyUpdateText(text_data, 0, text_index, offset, GetStrLenR(text_index), 0, string_num);
				return;
			}
			else if (text_data[text_index - 1] == '\n') {
				SendC(KEY_UP);
				MovToStringEnd();
				text_data.erase(text_index + 1, 1);
				strings_amount--;
				NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
				NotifyUpdateText(text_data, 0, text_index, offset, GetStrLenR(text_index), 0, string_num);
				return;
			}
			else {
				text_data.erase(text_index, 1);
				text_index--;
			}
		}

		break;
	case DELETE:
		if (text_data.length() >= 0) {
			if (GetStrLenL(text_index) >= 0 && text_data[text_index + 1] != '\0') {
				text_data.erase(text_index + 1, 1);
				if (text_data[text_index] == '\n') text_index++;
			}
			else {
				SendC(BACKSPACE);
			}
		}
		break;
	case ENTER:
		text_data.insert(text_index, 1, '\n');
		text_index--;
		SendC(KEY_DOWN);
		MovToStringStart();
		strings_amount++;
		return;
		break;
	case KEY_NPAGE:
		PageDown();
		break;
	case KEY_PPAGE:
		if (string_num == 0) return;
		PageUP();
		break;
	case ESC:
		ChangeMode(Mode::NAVI);
		NotifyUpdateMode(mode);
		NotifyBackToText();
		return;
		break;
	default:
		text_data.insert(text_index + 1, 1, symb);
		SendC(KEY_RIGHT);
		return;
		break;
	}
	NotifyUpdateStat(opened_file_name, string_num + 1, strings_amount);
	NotifyUpdateText(text_data, symb, text_index, offset, GetStrLenR(text_index), 0, string_num);
	return;
}


void  VimModel::MovToStringStart() {
	do{
		SendC(KEY_LEFT);
	} while (GetStrLenL(text_index) > 1);
	if (!string_num) SendC(KEY_LEFT);
}

void  VimModel::MovToStringEnd() {
	while(GetStrLenR(text_index) > 0) {
		SendC(KEY_RIGHT);
	}
}

void  VimModel::MovToNextWordEnd() {
	do{
		if (GetStrLenR(text_index) == 0) {
			SendC(KEY_DOWN);
			MovToStringStart();
		}
		SendC(KEY_RIGHT);
	} while (!(!(IsSpecSymb(text_data[text_index])) && (text_data[text_index + 1] == '\n' || IsSpecSymb(text_data[text_index+1]) || text_data[text_index + 1] == '\0')));
}

void  VimModel::MovToPrevWordStart() {
	if (text_data[text_index - 1] == '\n') {
		SendC(KEY_UP);
		MovToStringEnd();
		if (GetStrLenL(text_index) <= 1) return;
	}
	do{
		if (string_num > 0&&GetStrLenL(text_index) <= 1) {
			SendC(KEY_UP);
			MovToStringEnd();
		}
		SendC(KEY_LEFT);
		if (text_index == 0) return;
		if (text_data[text_index] == ' ' && text_data[text_index-1] == '\n') {
			SendC(KEY_UP);
			MovToStringEnd();
			break;
		}
	}while (!(!(IsSpecSymb(text_data[text_index])) && (text_data[text_index - 1] == '\n' || IsSpecSymb(text_data[text_index-1]))));
}

void VimModel::GoToFileStart() {
	text_index = 0;
	NotifySetNewStart(text_index);
}

void VimModel::GoToFileEnd() {
	int prev = 0;
	do {
		prev = string_num;
		SendC(KEY_DOWN);
	} while (string_num != prev);
	MovToStringEnd();
}

void VimModel::GotoStringByNum(const MyString & pressed_buttons) {
	unsigned num = std::atoi(pressed_buttons.c_str());
	if (num>string_num) {
		size_t prev = string_num+1;
		while (string_num+1< num&&string_num!=prev) {
			prev = string_num;
			SendC(KEY_DOWN);
		}
	}
	else {
		size_t prev = string_num + 1;
		while (string_num+1 > num && string_num != prev) {
			prev = string_num;
			SendC(KEY_UP);
		}
	}
	MovToStringStart();
}

void VimModel::DeleteWordHere() {
	while (text_index!=0&&text_data[text_index -1] != ' '&& text_data[text_index - 1] != '\n') {
		text_index--;
	}
	size_t i = text_index;
	while (!(text_data[i] == ' ' || text_data[i] == '\n')){
		i++;
	}
	if (text_data[i] != '\n') {
		i++;
	}
	text_data.erase(text_index, i - text_index);
	if (text_data[text_index] == '\n') {
		text_index--;
	}
}

void VimModel::PageDown() {
	scroll_flag = FALSE;
	while (scroll_flag != TRUE) {
		SendC(KEY_DOWN);
	}
	NotifySetNewStart(text_index);
}

void VimModel::SearchToStart(MyString& aim) {
	prev_search = TRUE;
	bool searchd = FALSE;
	if (text_index == 0) return;
	int index = text_index-1;
	while (index-- > 0) {
		if (text_data[index] == aim[1]) {
			if (std::equal(&text_data.c_str()[index], &text_data.c_str()[index + aim.length() - 1], aim.substr(1).c_str())) {
				searchd = TRUE;
				break;
			}
		}
	}
	if (searchd == FALSE) {
		ChangeMode(Mode::NAVI);
		NotifyBackToText();
		ChangeMode(Mode::SEARCH);
		return;
	}
	else {
		ChangeMode(Mode::NAVI);
		MovToStringStart();
		while (text_index > index) SendC(KEY_UP);
		while (text_index < index) SendC(KEY_RIGHT);
		ChangeMode(Mode::SEARCH);
	}
}

void VimModel::PageUP() {
	scroll_flag = FALSE;
	MovToStringStart();
	while (scroll_flag != TRUE&&string_num>0) {
		SendC(KEY_UP);
	}
	if (string_num != 0) {
		SendC(KEY_DOWN);
	}else SendC(KEY_LEFT);
	NotifySetNewStart(text_index);
}

void VimModel::DeleteThisString() {
	MovToStringStart();
	if (text_data[text_index] == '\n'&& text_data[text_index-1] == '\n') {
		SendC('x');
		return;
	}
	if (string_num == 0&& text_data[text_index + GetStrLenR(text_index) + 2] != '\0') {
		text_data.erase(text_index, GetStrLenR(text_index) + GetStrLenL(text_index) + 2);
	}
	else {
		if (text_data[text_index + GetStrLenR(text_index)+1] != '\0') {
			text_data.erase(text_index, GetStrLenR(text_index) + GetStrLenL(text_index)+1);
		}
		else {
			MovToStringStart();
			text_data.erase(text_index, GetStrLenR(text_index) + GetStrLenL(text_index));
			//text_index--;
			if (string_num != 0) {
				MovToPrevWordStart();
			}else{
				text_data.erase(text_index, 1);
			}
		}
	}
	strings_amount--;
}

void VimModel::CopyCurString() {
	buffer.clear();
	MovToStringStart();
	buffer.append(text_data.substr(text_index, GetStrLenR(text_index) + 1));
}

void VimModel::CopyCurWord() {
	if (text_data[text_index] == '\n'||IsSpecSymb(text_data[text_index])) return; //if we are on separator
	int left = text_index;
	while ((text_index > 0)&&!(IsSpecSymb(text_data[left])|| text_data[left] == '\n')){
		left--;
	}
	if (left!=0)left++;
	int right = text_index;
	while (!(IsSpecSymb(text_data[right]) || text_data[right] == '\n'|| text_data[right] == '\0')) {
		right++;
	}
	right--;
	buffer.clear();
	buffer.append(text_data.substr(left, right-left+1));
}

void VimModel::SearchToEnd(MyString& aim) {
	prev_search = FALSE;
	ChangeMode(Mode::NAVI);
	size_t index = text_data.find(aim.substr(1).c_str(),text_index+1);
	if (index == -1) {
		return;
	}
	MovToStringStart();
	while (text_index < index) SendC(KEY_DOWN);
	if(text_index != index) SendC(KEY_UP);
	while (text_index < index) SendC(KEY_RIGHT);
	ChangeMode(Mode::SEARCH);
	NotifyUpdateStat(opened_file_name,string_num, strings_amount);
}
