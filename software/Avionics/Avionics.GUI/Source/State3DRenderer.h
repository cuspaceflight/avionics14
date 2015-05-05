#pragma once
#include <Rendering/Scene/FTView.h>

class RocketRenderer;
class RocketPathRenderer;
struct state_estimate_t;

class State3DRenderer : public FTView {
public:
	State3DRenderer();
	~State3DRenderer();

	virtual void draw() override;

	void nextStateEstimate(state_estimate_t& current_state);

protected:
	RocketRenderer* rocket_renderer_;
	RocketPathRenderer* rocket_path_renderer_;
};
