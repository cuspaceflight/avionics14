#include "State3DRenderer.h"
#include <Rendering/Camera/FTCameraFPS.h>
#include "RocketRenderer.h"
#include "RocketPathRenderer.h"

extern "C" {
#include <state_estimate.h>
}


State3DRenderer::State3DRenderer() : FTView(new FTCameraFPS(), FTRect<float>(0,0,1.0f,1)), rocket_renderer_(new RocketRenderer()), rocket_path_renderer_(new RocketPathRenderer()) {
	camera_->release();

	((FTCameraFPS*)camera_)->setPosition(glm::vec3(0, 0, 50));
}

State3DRenderer::~State3DRenderer() {
	rocket_renderer_->release();
	rocket_path_renderer_->release();
}

void State3DRenderer::draw() {
	FTView::draw();
	rocket_renderer_->visit(camera_);
	rocket_path_renderer_->visit(camera_);
}

void State3DRenderer::nextStateEstimate(state_estimate_t& current_state) {
	rocket_renderer_->setPosition(glm::vec3(current_state.pos[0], current_state.pos[1], current_state.pos[2]));
	rocket_path_renderer_->nextStateEstimate(current_state);
}