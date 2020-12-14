#include "vim_like.h"

//here controller
Controller::Controller(VimModel* c_model) {
	model = c_model;
}

void Controller::SendLet(const int letter) {
	model->SendC(letter);
}

void Controller::UpScrollFlag(bool value) {
	model->SetScrlFlag(value);
}
