#include "MainScene.h"
#include "StateDetailView.h"
#include "State3DRenderer.h"
#include <Util/FTInputManager.h>
#include <Data/FileDataSource.h>
#include <Data/SerialDataSource.h>
#include <glm\gtx\quaternion.hpp>

extern "C" {
#include <telemetry.h>
#include <time_utils.h>
#include <mission.h>
#include <translation_kalman.h>
#include <quest_estimator.h>
#include <math_utils.h>
}

#define RUN_SIMULATION // Whether to run the estimators locally or render the on board values (not implemented yet)
#define SERIAL_DATA_SOURCE // Whether to read from a file or COM port

MainScene::MainScene() : data_source_(nullptr) {
	state_3d_renderer_ = new State3DRenderer();
	addView(state_3d_renderer_);
	state_3d_renderer_->release();

	state_detail_view_ = new StateDetailView();
	addView(state_detail_view_);
	state_detail_view_->release();
	
	/*float mag[3] = { -0.692562103971370, -0.297156898213635, -0.657309295527456 };
	float accel[3] = { 0.229187214724976, -0.752609925351439, -0.617293707135516 };

	quest_estimator_new_accel(accel);
	quest_estimator_new_mag(mag);
	float q[4];
	float xyz[3];
	quest_estimator_update(q,xyz);

	
	//quat_to_euler(q, xyz);

	for (int i = 0; i < 3; i++)
		xyz[i] *= 180.0f / PI;

	FTLOG("Quest returned: (%f %f %f)", xyz[0], xyz[1], xyz[2]);*/

#ifdef RUN_SIMULATION
#ifdef SERIAL_DATA_SOURCE
	data_source_ = new SerialDataSource("\\\\.\\COM3", 38400);
#else
	data_source_ = new FileDataSource("log_00021.bin");
#endif
	FTDirector::getSharedInstance()->getPreDrawEventHandler()->Connect(this, &MainScene::update);
#endif
}

MainScene::~MainScene() {
#ifdef RUN_SIMULATION
	FTDirector::getSharedInstance()->getPreDrawEventHandler()->Disconnect(this, &MainScene::update);
	data_source_->release();
#endif
}

void MainScene::update(float actual_dt) {
	if (data_source_ == nullptr) {
		FTLogError("Data not loaded!");
		return;
	}

	//if (FTInputManager::getSharedInstance()->isKeyDown(KeyNameUp))
	//	return;

	const float prediction_update_rate = 1000.0f; // At what frequency to run the prediction step
	const int prediction_update_rate_clocks = (int)(CLOCK_FREQUENCY / prediction_update_rate);
	// This is needed because this method only gets called at 60Hz whereas the prediction functions need to be called at ~1kHz
	int num_cycles = (int)(actual_dt * prediction_update_rate);

	for (int i = 0; i < num_cycles; ++i) {
		// Bring input data up to date
		
		data_source_->update(prediction_update_rate_clocks, &state_estimate_);
		
		
		//FTLog("Running state estimators\n");
		state_estimate_compute_next(&state_estimate_, 1 / prediction_update_rate);

		/*glm::quat quaternion;
		quaternion.x = state_estimate_.orientation_q[0];
		quaternion.y = state_estimate_.orientation_q[1];
		quaternion.z = state_estimate_.orientation_q[2];
		quaternion.w = state_estimate_.orientation_q[3];
		glm::vec3 euler = glm::eulerAngles(quaternion);
		//FTLOG("%f %f %f vs %f %f %f", euler.x, euler.y, euler.z, state_estimate_.orientation_euler[0], state_estimate_.orientation_euler[1], state_estimate_.orientation_euler[2]);
		state_estimate_.orientation_euler[0] = euler.x;
		state_estimate_.orientation_euler[1] = euler.y;
		state_estimate_.orientation_euler[2] = euler.z;


		state_estimate_.orientation_euler[0] = -state_estimate_.orientation_euler[0];
		state_estimate_.orientation_euler[1] = -state_estimate_.orientation_euler[1];*/

		//print_state_estimate(&state_estimate_);
		state_detail_view_->updateDisplay(state_estimate_);
		state_3d_renderer_->nextStateEstimate(state_estimate_);
	}

}


