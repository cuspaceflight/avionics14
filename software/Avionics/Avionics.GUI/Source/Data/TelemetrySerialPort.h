#pragma once
#include "SerialPort.h"
#include <ThirdParty/Signals/Delegate.h>

extern "C" {
#include <telemetry.h>
}

class TelemetrySerialPort : public SerialPort {
public:
	TelemetrySerialPort(const char* port_name, int baud_rate);
	~TelemetrySerialPort();

	Gallant::Delegate1<const telemetry_t&>* getPacketReceivedDelegate() {
		return &packet_received_delegate_;
	}
	
	bool poll();

protected:
	virtual int read(uint8_t* buff, size_t size) override {
		return SerialPort::read(buff, size);
	}

	void sync();

	Gallant::Delegate1<const telemetry_t&> packet_received_delegate_;
	telemetry_t message_buffer_;
	uint8_t message_buffer_index_;
};

