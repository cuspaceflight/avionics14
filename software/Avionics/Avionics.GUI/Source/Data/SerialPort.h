#pragma once
#include <Windows.h>
#include <FTObject.h>
#include <FTString.h>

class SerialPort : public FTObject {
public:
	SerialPort(const char* port_name, int baud_rate);
	~SerialPort();

	virtual int read(uint8_t* buff, size_t size);

	virtual int write(const uint8_t* buff, size_t size);

	bool portIsOpen() {
		return port_handle_ != INVALID_HANDLE_VALUE;
	}

protected:
	HANDLE port_handle_;
	uint8_t* input_buffer_;
	
};

