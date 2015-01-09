#pragma once
#include <Rendering/Scene/FTView.h>
#include <Rendering/Scene/FTNode.h>
#include <FTArray.h>

class FTLabel;
struct state_estimate_t;

class StateDetailView : public FTView {
public:
	StateDetailView();
	~StateDetailView();

	virtual void draw() override;

	void updateDisplay(state_estimate_t& current_state);

protected:
	FTArray<FTNode>* children_;
	FTLabel** value_labels_;
};
