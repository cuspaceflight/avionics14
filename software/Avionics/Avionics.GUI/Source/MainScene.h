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

class MainScene : public FTScene {
public:
	MainScene();
	~MainScene();

protected:
	uint32_t num_samples_;
	uint32_t simulation_index_;
	telemetry_t* data_input_;
	state_estimate_t state_estimate_;
	

	uint64_t simulation_time_;
	uint64_t packet_timestep_correction_;
	uint32_t last_packet_time_;
	int16_t accel_bias_measurements[3];
	float accel_scale_;

	StateDetailView* state_detail_view_;
	State3DRenderer* state_3d_renderer_;

	void update(float dt);
	void loadFromBinaryFile(const char* file);
	void adjustAccelValues(const int16_t data[4], float adjusted[3]);
};
