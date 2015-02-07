#pragma once
#include <Rendering/Scene/FTScene.h>

extern "C" {
#include <state_estimate.h>
#include <telemetry.h>
}


class StateDetailView;
class State3DRenderer;
enum KeyName;
enum KeyState;
class DataSource;

class MainScene : public FTScene {
public:
	MainScene();
	~MainScene();

protected:
	state_estimate_t state_estimate_;
	DataSource* data_source_;

	
	

	StateDetailView* state_detail_view_;
	State3DRenderer* state_3d_renderer_;

	void update(float dt);
	//void loadFromBinaryFile(const char* file);
	
};
