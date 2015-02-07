#include "FileDataSource.h"
#include <fstream>
#include <stdio.h>

extern "C" {
#include "calibration.h"
}


FileDataSource::FileDataSource(const char* filename) : simulation_index_(0), data_input_(nullptr), simulation_time_(0), packet_timestep_correction_(0) {
	loadFromBinaryFile(filename);
}


FileDataSource::~FileDataSource() {
}

void FileDataSource::update(uint64_t dt, state_estimate_t* state) {
	if (simulation_index_ == num_samples_)
		return;
	//if (FTInputManager::getSharedInstance()->isKeyDown(KeyNameUp))
	//	return;
	simulation_time_ += dt;
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

		handlePacket(data);
	}

}

void FileDataSource::loadFromBinaryFile(const char* file) {
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

		simulation_index_ = 0;
		// Skip forward to the configuration values (as all prior data is meaningless)
		while (simulation_index_ != num_samples_) {
			if (data_input_[simulation_index_].channel_ == 0x11)
				break;
			++simulation_index_;
		}
		FTAssert(simulation_index_ != num_samples_, "No accelerometer config data found");

		// Find ignition state
		while (simulation_index_ != num_samples_) {
			/*if (data_input_[simulation_index_].channel_ == 0x20) {
				float cal[3];
				calibrate_accel(data_input_[simulation_index_].int16_data_, cal);

				FTLOG("%f %f %f", cal[0], cal[1], cal[2]);

			} else*/ if (data_input_[simulation_index_].channel_ == 0x40 && data_input_[simulation_index_].int32_data_[1] == 1) {
				break;
			}
			++simulation_index_;
		}
		FTAssert(simulation_index_ != num_samples_, "No ignition state found!");


		simulation_time_ = data_input_[simulation_index_].timestamp_;
		last_packet_time_ = data_input_[simulation_index_].timestamp_;
	}
	else {
		FTLogError("Couldn't open input file at path %s", file);
		return;
	}
}