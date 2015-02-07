#include "SerialDataSource.h"

SerialDataSource::SerialDataSource(const char* port_name, int baud) : serial_port_(new TelemetrySerialPort(port_name,baud)) {
	serial_port_->getPacketReceivedDelegate()->Bind(this, &SerialDataSource::nextPacket);
}


SerialDataSource::~SerialDataSource() {
	serial_port_->release();
}

void SerialDataSource::update(uint64_t dt, state_estimate_t* estimate) {
	while (serial_port_->poll());
}

void SerialDataSource::nextPacket(const telemetry_t& packet) {
	handlePacket(packet);
}