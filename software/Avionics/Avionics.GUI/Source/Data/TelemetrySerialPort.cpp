#include "TelemetrySerialPort.h"


TelemetrySerialPort::TelemetrySerialPort(const char* port_name, int baud_rate) : SerialPort(port_name,baud_rate), message_buffer_index_(0) {
	if (portIsOpen())
		sync();
}


TelemetrySerialPort::~TelemetrySerialPort() {
	
}

// This method is needed as sometimes there will be a few random bytes when the port is first opened
// Without this sync routine this would cause all the packets to be offset slightly
void TelemetrySerialPort::sync() {
	int n = 0;
	const char characters[4] = { 'S', 'Y', 'N', 'C' };

	

	uint8_t read_byte;
	do {
		write((const uint8_t*)characters, 4);
		Sleep(10);
		if (read(&read_byte, 1) != 0) {
			if (read_byte == characters[n]) {
				n++;
			}
			else {
				n = 0;
			}
		}
		else {
			FTLOG("Waiting for SYNC...");
			Sleep(1000);
		}
	} while (n != 4);

	FTLOG("SYNC Successful");
}

bool TelemetrySerialPort::poll() {
	bool ret = false;
	uint8_t buffer[1024];
	uint8_t* message_buffer_as_bytes = (uint8_t*)&message_buffer_;

	int n = read(buffer, 1024);
	for (int i = 0; i < n; i++) {
		message_buffer_as_bytes[message_buffer_index_++] = buffer[i];
		if (message_buffer_index_ == sizeof(telemetry_t)) {
			message_buffer_index_ = 0;
			// TODO check checksum of packet

			packet_received_delegate_(message_buffer_);
			ret = true;
		}
	}
	return ret;
}
