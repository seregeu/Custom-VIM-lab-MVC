#include <iostream>
#include <fstream>
#include "curses.h"
#include "panel.h"
#include "MyString.h"
#include <string>
#include <vector>
#include <Windows.h>

#define BOTTOM_PANEL_H 2
#define MODE_L 20
#define NAME_L 105
#define NUMBER_L 20
#define F_ROWS 35
#define F_COLUMNS 145
#define CONS_H 2

#define CONS 1
#define TEXT 2
#define FILENAME 3
#define WSER 4
#define SEAR 5

#define BACKSPACE  8
#define DELETE 330
#define ENTER 10
#define ESC 27


//adapt
class Target
{
public:
	virtual int AdapterResizeTerm(int, int) = 0;
	virtual int ApapterStartColor() = 0;
	virtual int AdapterNoecho() = 0;
	virtual int AdapterInitPair(short, short, short) = 0;
	virtual int AdapterRaw() = 0;
	virtual WINDOW* ApapterInitscr() = 0;
	virtual int AdapterKeypad(WINDOW*, bool) = 0;
	virtual WINDOW* AdapterNewWin(int, int, int, int) = 0;
	virtual int AdapterWclear(WINDOW*) = 0;
	virtual int AdapterEndWin() = 0;
	virtual int AdapterMove(int, int) = 0;
	virtual int AdapterWrefresh(WINDOW*) = 0;
	virtual int AdapterWbkgd(WINDOW*, chtype) = 0;
};

class Adapter : public Target
{
public:
	virtual int AdapterMove(int _y, int _x) { return move(_y, _x); }
	virtual int ApapterStartColor() { return start_color(); }
	virtual int AdapterNoecho() { return noecho(); }
	virtual int AdapterResizeTerm(int height, int windth) { return resize_term(height, windth); };
	virtual int AdapterKeypad(WINDOW* window, bool value) { return keypad(window, value); }
	virtual int AdapterRaw() { return raw(); }
	virtual int AdapterWclear(WINDOW* window) { return wclear(window); }
	virtual WINDOW* AdapterNewWin(int nrows, int ncol, int _y, int _x) { return newwin(nrows, ncol, _y, _x); };
	virtual int AdapterWrefresh(WINDOW* window) { return wrefresh(window); }
	virtual int AdapterEndWin() { return endwin(); }
	virtual int AdapterInitPair(short pair, short f, short d) { return init_pair(pair, f, d); }
	virtual WINDOW* ApapterInitscr() { return initscr(); }
	virtual int AdapterWbkgd(WINDOW* window, chtype color) { return wbkgd(window, color); }
};

//here model
enum class Mode { NAVI, COMMAND, WRITE, SEARCH,HELP};

class Controller;
//observer
class Observer {
public:
	virtual ~Observer() {};
	virtual void UpdateCom(MyString& content, const int symb, const int com_index, const unsigned char win) = 0;
	virtual void UpdateText(MyString& content, const int symb, const int str_index, const size_t offset, const size_t r_len, const size_t n_len, const size_t string_num) = 0;
	virtual void NewFile()=0;
	virtual void BackToText()=0;
	virtual void SetNewStart(const size_t index)=0;
	virtual void UpdateStat(MyString& filename, const size_t cur_str, const size_t str_am)=0;
	virtual void UpdateMode(Mode& mode) = 0;

};
//Observable
class Observable {
protected:
	std::vector<Observer*> _observers;
public:
	virtual void addObserver(Observer* observer)=0;
	virtual void NotifyUpdateCom(MyString& content, const int symb, const int cons_index, const unsigned char win) =0;
	virtual void NotifyUpdateText(MyString& content, const int symb, const int text_index, const size_t offset, const size_t r_len, const size_t n_len, const size_t string_num) =0;
	virtual void NotifyNewFile() = 0;
	virtual void NotifyBackToText() = 0;
	virtual void NotifySetNewStart(const size_t index) = 0;
	virtual void NotifyUpdateStat(MyString& filename, const size_t cur_str, const size_t str_am)=0;
	virtual void NotifyUpdateMode(Mode& mode)=0;
};


class VimModel :public Observable {
public:
	VimModel();
	~VimModel() {};
	void addObserver(Observer* observer);
	void NotifyUpdateCom(MyString& content, const int symb, const int cons_index, const unsigned char win);
	void NotifyUpdateText(MyString& content, const int symb, const int text_index, const size_t offset, const size_t r_len, const size_t n_len,const size_t string_num);
	void NotifyUpdateStat(MyString& filename,size_t cur_str,size_t str_am);
	void NotifyUpdateMode(Mode& mode);
	void NotifyNewFile();
	void NotifyBackToText();
	void NotifySetNewStart(const size_t index);
	
	void ChangeMode(Mode n_mode);
	void SendC(const int symb);
	void SendCom(const int symb);
	void SendNav(const int symb);
	void SendSear(const int symb);
	void SendHelp(const int symb);
	void SendWrite(const int symb);
	void SetScrlFlag(bool value);
	bool RecogCom(MyString& command);
	size_t GetStrLenR(const int text_index);
	size_t GetStrLenL(const int text_index);
	size_t SerDownPos(MyString& text_data, const int text_index, const int word_len);
	int SerUpPos(MyString& text_data, const int text_index, const int word_len, const int offset);
	bool IsSpecSymb(const char symb);
	void MovToStringStart();
	void MovToStringEnd();
	void MovToNextWordEnd();
	void MovToPrevWordStart();
	void GoToFileStart();
	void GoToFileEnd();
	void GotoStringByNum(const MyString& pressed_buttons);
	void DeleteWordHere();
	void PageDown();
	void PageUP();
	void DeleteThisString();
	void CopyCurString();
	void CopyCurWord();

	void SearchToEnd(MyString& aim);
	void SearchToStart(MyString& aim);
private:
	const int max_com_len = 290;
	bool scroll_flag;
	bool prev_search; //false - forward, true - back 
	bool is_modify; //false - was not, true - was
	MyString text_data;
	MyString cons_data;
	MyString search_data;
	MyString prsearch_data;
	MyString filename_data;
	MyString N_string_data;
	MyString pressed_buttons;
	MyString buffer;
	MyString opened_file_name;
	Mode mode;
	size_t cons_index;
	size_t search_index;
	int text_index;
	size_t offset;
	size_t string_num;
	size_t page_num;
	size_t strings_amount;
};
 //here view
class Window {
protected:
	unsigned char rows;
	unsigned char collumns;
	WINDOW* window;
public:
	Window();
	~Window() {};
	virtual void Show(Target* adapter)=0;
	WINDOW* CreateNewWin(const int height, const int width, const int starty, const int startx,Target* adapter);
};

class TextWindow : public Window {
private:
	size_t start_ind;
	size_t c_y;
	size_t c_x;
	size_t text_out_strt;
public:
	TextWindow();
	TextWindow(const int height, const int width, const int starty, const int startx, Target* adapter);
	~TextWindow() {};
	void Show(Target* adapter);
	void PutData(MyString& content, Target* adapter);
	size_t GetStart();
	void SetStart(const size_t new_start_ind);
	void Refresh();
};

class ConsWindow : public Window {
private:

public:
	ConsWindow();
	ConsWindow(const int height, const int width, const int starty, const int startx, Target* adapter);
	~ConsWindow() {};
	void PutData(MyString& content, Target* adapter);
	void Show(Target* adapter);
};

class StatusBar : public Window {
private:
	WINDOW* filename;
	WINDOW* N_string;
public:
	StatusBar();
	StatusBar(const int height, const int width, const int starty, const int startx, Target* adapter);
	~StatusBar(){};
	void Show(Target* adapter);
	void PutData(MyString& filename, const size_t cur_str, const size_t str_am,Target* adapter);
	void PutMode(Mode& mode,Target* adapter);
};

class View :public Observer {
public:
	View(VimModel* c_model, Controller* c_controller, Target* adapter);
	~View() {};
	void Show(const unsigned char window);
	void UpdateCom(MyString& content, const int symb, const int cons_index, const unsigned char win);
	void UpdateText(MyString& content, const int symb, const int str_index, const size_t offset, const size_t r_len, const size_t n_len, const size_t string_num);
	void NewFile();
	void BackToText();
	void SetNewStart(const size_t index);
	void UpdateStat(MyString& filename, const size_t cur_str, const size_t str_am);
	void UpdateMode(Mode& mode);

	void Start();
	void MoveCR(); 
	void MoveCL();
	void MoveCD();
	void MoveCU();

	void MoveTR();
	void MoveTL();
	void MoveTD(const int str_index, const size_t r_len, const size_t n_len, const size_t offset);
	void MoveTU(const int str_index, const size_t r_len, const size_t offset, const size_t n_len, const int str_n);

	int ScrollDown(MyString& content,const size_t n_len);
	int ScrollUp(MyString& content, const size_t n_len,const size_t offset);
private:
	TextWindow text_window;
	ConsWindow cons_window;
	StatusBar statusbar;
	VimModel* model;
	Controller *controller;
	int c_posy;
	int c_posx;	
	int t_posy;
	int t_posx;
	size_t scrolled_up;
	Target* _adapter;
};

//here controller
class Controller {
private:
	VimModel* model;
public:
	Controller(VimModel* c_model);
	void SendLet(const int let);
	void UpScrollFlag(bool value);
};




