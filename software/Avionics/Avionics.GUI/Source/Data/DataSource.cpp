#include "DataSource.h"
extern "C" {
#include <state_estimate.h>
#include <mission.h>
}

DataSource::DataSource() {

}

DataSource::~DataSource() {

}

void DataSource::handlePacket(const telemetry_t& data) {
	switch (data.channel_) {
	case PACKET_PRESSURE_RAW: {
		//float altitude = state_estimation_pressure_to_altitude(data.int32_data_[0]);
		//FTLOG("New Altitude %f", altitude);
		
		state_estimate_new_pressure_raw(data.int32_data_[0]);
		break;
	}
	case PACKET_ACCEL_RAW: {
		//FTLOG("New Accel %i %i %i", data.int16_data_[0], data.int16_data_[1], data.int16_data_[2]);
		//FTLOG("Adjusted Accel %f %f %f", adjusted[0], adjusted[1], adjusted[2]);
		//translation_kalman_new_accel(adjusted);
		state_estimate_new_accel_raw(data.int16_data_);
		break;
	}
	case PACKET_GYRO_RAW: {
		break;
	}
	case PACKET_MAG_RAW: {
		state_estimate_new_magnetometer_raw(data.int16_data_);
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