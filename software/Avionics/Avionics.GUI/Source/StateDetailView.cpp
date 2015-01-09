#include "StateDetailView.h"
#include <Rendering/Camera/FTCamera2D.h>
#include <Rendering/Text/FTLabel.h>
#include <Rendering/FTDirector.h>
#include <FTString.h>
#include <sstream>

extern "C" {
#include <state_estimate.h>
}

StateDetailView::StateDetailView() : FTView(new FTCamera2D(), FTRect<float>(0, 0, 1, 1)), children_(new FTArray<FTNode>()) {
	camera_->release();

	const int num_labels = 11;
	static const wchar_t* label_names[num_labels] = {L"Position X", L"Position Y", L"Position Z", L"Velocity X", L"Velocity Y", L"Velocity Z", L"Velocity Mag", L"Accel X", L"Accel Y", L"Accel Z", L"Accel Mag"};

	glm::vec2 screensize = FTDirector::getSharedInstance()->getWindowSize();
	float y = screensize.y - 30;

	value_labels_ = new FTLabel*[num_labels];
	const float y_padding = 25.0f;
	for (int i = 0; i < num_labels; i++) {
		FTLabel* label = new FTLabel("Fonts/Vera.ttf", label_names[i], 14);
		children_->add(label);
		label->setPosition(glm::vec2(30, y));
		label->setAnchorPoint(glm::vec2(0, 0.5f));
		label->release();

		label = new FTLabel("Fonts/Vera.ttf", L"0", 14, true);
		children_->add(label);
		label->setPosition(glm::vec2(250, y));
		label->setAnchorPoint(glm::vec2(1, 0.5f));
		label->release();

		value_labels_[i] = label;

		y -= y_padding;
	}

	value_labels_[0]->setString(L"-0.03");
	value_labels_[0]->setString(L"-0.04");
}

StateDetailView::~StateDetailView() {
	FTLog("State Detail View Destroyed");
	children_->release();
}

void StateDetailView::draw() {
	FTView::draw();

	for (auto it = children_->begin(); it != children_->end(); ++it) {
		(*it)->visit(camera_);
	}
}

void StateDetailView::updateDisplay(state_estimate_t& current_state) {
	static wchar_t buff[1024];
	value_labels_[0]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.pos[0]));
	value_labels_[1]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.pos[1]));
	value_labels_[2]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.pos[2]));

	value_labels_[3]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.vel[0]));
	value_labels_[4]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.vel[1]));
	value_labels_[5]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.vel[2]));

	float mag = sqrtf(current_state.vel[0] * current_state.vel[0] + current_state.vel[1] * current_state.vel[1] + current_state.vel[2] * current_state.vel[2]);
	value_labels_[6]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", mag));


	value_labels_[7]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.accel[0]));
	value_labels_[8]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.accel[1]));
	value_labels_[9]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.accel[2]));

	mag = sqrtf(current_state.accel[0] * current_state.accel[0] + current_state.accel[1] * current_state.accel[1] + current_state.accel[2] * current_state.accel[2]);
	value_labels_[10]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", mag));

	//value_labels_[3]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.vel[0]));

}
