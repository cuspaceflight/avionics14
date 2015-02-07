#pragma once
#include <Data/TelemetrySerialPort.h>
#include <FTCircularBuffer.h>
#include "DataSource.h"

class SerialDataSource : public DataSource
{
public:
	SerialDataSource(const char* port_name, int baud);
	~SerialDataSource();

	void update(uint64_t dt, state_estimate_t* estimate) override;

	void nextPacket(const telemetry_t& packet);

private:
	TelemetrySerialPort* serial_port_;
};

