#include "StateDetailView.h"
#include <Rendering/Camera/FTCamera2D.h>
#include <Rendering/Text/FTLabel.h>
#include <Rendering/FTDirector.h>
#include <FTString.h>
#include <sstream>

extern "C" {
#include <state_estimate.h>
#include <math_utils.h>
}

StateDetailView::StateDetailView() : FTView(new FTCamera2D(), FTRect<float>(0, 0, 1, 1)), children_(new FTArray<FTNode>()) {
	camera_->release();

	const int num_labels = 21;
	static const wchar_t* label_names[num_labels] = { L"Position X", L"Position Y", L"Position Z", L"Velocity X", L"Velocity Y", L"Velocity Z", L"Velocity Mag", L"Accel X", L"Accel Y", L"Accel Z", L"Accel Mag", L"Quat X", L"Quat Y", L"Quat Z", L"Quat W", L"Orientation Euler X", L"Orientation Euler Y", L"Orientation Euler Z", L"Angular Velocity X", L"Angular Velocity Y", L"Angular Velocity Z"};

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

	value_labels_[11]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.orientation_q[0]));
	value_labels_[12]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.orientation_q[1]));
	value_labels_[13]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.orientation_q[2]));
	value_labels_[14]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.orientation_q[3]));

	float euler[3];
	quat_to_euler(current_state.orientation_q,euler);

	value_labels_[15]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", euler[0] * 57.2957795131f));
	value_labels_[16]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", euler[1] * 57.2957795131f));
	value_labels_[17]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", euler[2] * 57.2957795131f));

	value_labels_[18]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.angular_velocity[0]));
	value_labels_[19]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.angular_velocity[1]));
	value_labels_[20]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.angular_velocity[2]));


	//value_labels_[3]->setString(FTStringUtil<wchar_t>::formatString(buff, 1024, L"%f", current_state.vel[0]));

}
