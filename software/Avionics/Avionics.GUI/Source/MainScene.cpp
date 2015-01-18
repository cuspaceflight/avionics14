#include "MainScene.h"
#include "StateDetailView.h"
#include "State3DRenderer.h"
#include <Util/FTInputManager.h>
#include <stdio.h>
#include <fstream>

extern "C" {
#include <telemetry.h>
#include <time_utils.h>
#include <mission.h>
#include <translation_kalman.h>
#include <quest_estimator.h>
#include <math_utils.h>
}

//#define RUN_SIMULATION

MainScene::MainScene() : simulation_index_(0), data_input_(nullptr), simulation_time_(0), packet_timestep_correction_(0) {
	state_3d_renderer_ = new State3DRenderer();
	addView(state_3d_renderer_);
	state_3d_renderer_->release();

	state_detail_view_ = new StateDetailView();
	addView(state_detail_view_);
	state_detail_view_->release();

	// Reference vectors rotated by (x,y,z) (35, -37, -180) degrees in that order
	float mag[3] = { 0.345186916322074, -0.819152044288992, -0.458078509796326 };
	float accel[3] = { 0.492978006498827, 0.573576436351046, -0.654203910697022 };

	quest_estimator_new_accel(accel);
	quest_estimator_new_mag(mag);
	float q[4];
	quest_estimator_update(q);

	float xyz[3];
	quat_to_euler(q, xyz);

	for (int i = 0; i < 3; i++)
		xyz[i] *= 180.0f / PI;

	FTLOG("Quest returned: (%f %f %f)", xyz[0], xyz[1], xyz[2]);

#ifdef RUN_SIMULATION
	loadFromBinaryFile("log_00021.bin");
	FTDirector::getSharedInstance()->getPreDrawEventHandler()->Connect(this, &MainScene::update);
#endif
}

MainScene::~MainScene() {
#ifdef RUN_SIMULATION
	FTDirector::getSharedInstance()->getPreDrawEventHandler()->Disconnect(this, &MainScene::update);
#endif
}

void MainScene::adjustAccelValues(const int16_t data[4], float adjusted[3]) {
	// Assume scale factors are the same for all axis
	//float gscale = abs(9.81f / accel_bias_measurements[1]);
	adjusted[0] = (data[0] - accel_bias_measurements[0]) * accel_scale_;
	adjusted[1] = (data[1] - accel_bias_measurements[1]) * accel_scale_;
	adjusted[2] = (data[2] - accel_bias_measurements[2]) * accel_scale_;
}

void MainScene::update(float actual_dt) {
	if (data_input_ == nullptr) {
		FTLogError("Data not loaded!");
		return;
	}

	if (simulation_index_ == num_samples_)
		return;
	//if (FTInputManager::getSharedInstance()->isKeyDown(KeyNameUp))
	//	return;


	const float prediction_update_rate = 1000.0f; // At what frequency to run the prediction step
	const int prediction_update_rate_clocks = (int)(CLOCK_FREQUENCY / prediction_update_rate);

	// This is needed because this method only gets called at 60Hz whereas the prediction functions need to be called at ~1kHz
	int num_cycles = (int)(actual_dt * prediction_update_rate);
	for (int i = 0; i < num_cycles; ++i) {
		//while (true) {
		if (simulation_index_ == num_samples_) {
			FTLog("Processed all samples");
			break;
		}

		simulation_time_ += prediction_update_rate_clocks;
		//FTLOG("Simulation Time %lu", simulation_time_);

		// Bring input data up to date
		while (true) {
			if (simulation_index_ == num_samples_)
				break;
			telemetry_t data = data_input_[simulation_index_];
			if (data.timestamp_ < last_packet_time_) {
				FTLog("uint32_t clock rollover");
				packet_timestep_correction_ += 0xFFFFFFFF;
			}
			last_packet_time_ = data.timestamp_;
			uint64_t packet_true_time = data.timestamp_ + packet_timestep_correction_;
			if (packet_true_time > simulation_time_)
				break;
			++simulation_index_;

			switch (data.channel_) {
				case 0x22: {
					//float altitude = state_estimation_pressure_to_altitude(data.int32_data_[0]);
					//FTLOG("New Altitude %f", altitude);
					translation_kalman_new_pressure((float)data.int32_data_[0]);
					break;
				}
				case 0x20: {
					//FTLOG("New Accel %i %i %i", data.int16_data_[0], data.int16_data_[1], data.int16_data_[2]);
					float adjusted[3];
					adjustAccelValues(data.int16_data_, adjusted);
					//FTLOG("Adjusted Accel %f %f %f", adjusted[0], adjusted[1], adjusted[2]);
					translation_kalman_new_accel(adjusted);
					break;
				}
				case 0x40: {
					print_state_transition(data.int32_data_[0], data.int32_data_[1]);
					break;
				}
				default:
					break;
			}
		}

		//FTLog("Running state estimators\n");
		translation_kalman_prediction_step(&state_estimate_, 1 / prediction_update_rate);
		//print_state_estimate(&state_estimate_);
		state_detail_view_->updateDisplay(state_estimate_);
		state_3d_renderer_->nextStateEstimate(state_estimate_);
	}

}

void MainScene::loadFromBinaryFile(const char* file) {
	if (data_input_ != nullptr) {
		delete[] data_input_;
		data_input_ = nullptr;
	}


	std::ifstream file_stream(file, std::ios::in | std::ios::binary | std::ios::ate);
	if (file_stream.is_open()) {
		file_stream.seekg(0, std::ios::end);
		std::streamoff size = file_stream.tellg();

		if (size % sizeof(telemetry_t) != 0) {
			file_stream.close();
			FTLogError("File stream size not of a valid size - it is %i", size);
			return;
		}
		file_stream.seekg(0, std::ios::beg);

		// Useful for debugging where you only want a snippet of the data
		size_t max_samples = -1;
		num_samples_ = (size_t)min(max_samples, size / sizeof(telemetry_t));
		FTAssert(num_samples_ > 0, "No samples found!");
		data_input_ = new telemetry_t[num_samples_];
		FTLog("Loading Telemetry data");
		file_stream.read((char*)data_input_, num_samples_ * sizeof(telemetry_t));
		FTLog("Finished Loading Telemetry data");
		file_stream.close();

		int num_accel_samples = 50; // Number of accelerometer values to average
		int64_t totals[3];
		for (int i = 0; i < 3; i++)
			totals[i] = 0;

		simulation_index_ = 0;
		// Skip forward to the configuration values (as all prior data is meaningless)
		while (simulation_index_ != num_samples_) {
			if (data_input_[simulation_index_].channel_ == 0x11)
				break;
			++simulation_index_;
		}
		FTAssert(simulation_index_ != num_samples_, "No accelerometer config data found");

		// We have to manually calculate the static biases as only the y axis is provided in the data
		// Just calculate based on accel values up to ignition
		while (simulation_index_ != num_samples_) {
			if (data_input_[simulation_index_].channel_ == 0x20) {

				totals[0] += data_input_[simulation_index_].int16_data_[0];
				totals[1] += data_input_[simulation_index_].int16_data_[1];
				totals[2] += data_input_[simulation_index_].int16_data_[2];
				num_accel_samples++;
			} else if (data_input_[simulation_index_].channel_ == 0x40 && data_input_[simulation_index_].int32_data_[1] == 1) {
				break;
			}
			++simulation_index_;
		}
		FTAssert(simulation_index_ != num_samples_, "No ignition state found!");

		//simulation_index_ = 0;

		accel_bias_measurements[0] = (int16_t)(totals[0] / num_accel_samples);
		accel_bias_measurements[1] = (int16_t)(totals[1] / num_accel_samples);
		accel_bias_measurements[2] = (int16_t)(totals[2] / num_accel_samples);

		// Assume that the scale factor for all the axes is the same
		// This isn't really a valid assumption but is the best we can do without actual values
		accel_scale_ = 9.80665f / sqrtf((float)(accel_bias_measurements[0] * accel_bias_measurements[0] + accel_bias_measurements[1] * accel_bias_measurements[1] + accel_bias_measurements[2] * accel_bias_measurements[2]));


		simulation_time_ = data_input_[simulation_index_].timestamp_;
		last_packet_time_ = data_input_[simulation_index_].timestamp_;
		int32_t count = 0;
		uint64_t average = 0;

		// Determine clock Frequency
		/*uint32_t last_packet = data_input_[0].timestamp_;
		for (int i = 0; i < samples; ++i) {
			if (data_input_[i].channel_ != 0x50)
				continue;
			average += clocks_between(last_packet, data_input_[i].timestamp_);
			last_packet = data_input_[i].timestamp_;
			
			count++;
		}
		FTLOG("%i", average / count * 1000);*/

	} else {
		FTLogError("Couldn't open input file at path %s", file);
		return;
	}
}
