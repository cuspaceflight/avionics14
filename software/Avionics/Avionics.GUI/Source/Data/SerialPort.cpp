#include "SerialPort.h"

SerialPort::SerialPort(const char* port_name, int baud_rate) {
	char port_settings_string[40];
	FTStringUtil<char>::formatString(port_settings_string, 40, "baud=%i data=8 parity=N stop=1", baud_rate);
	port_handle_ = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (port_handle_ == INVALID_HANDLE_VALUE) {
		FTLogError("Unable to open comport %s", port_name);
		return;
	}

	DCB port_settings;
	memset(&port_settings, 0, sizeof(port_settings));
	port_settings.DCBlength = sizeof(port_settings);

	if (!BuildCommDCBA(port_settings_string, &port_settings)) {
		FTLogError("Unable to open comport %s with options %s", port_name, port_settings_string);
		CloseHandle(port_handle_);
		port_handle_ = INVALID_HANDLE_VALUE;
		return;
	}

	if (!SetCommState(port_handle_, &port_settings)) {
		FTLogError("Unable to set comport %s settings with options %s", port_name, port_settings_string);
		CloseHandle(port_handle_);
		port_handle_ = INVALID_HANDLE_VALUE;
		return;
	}

	COMMTIMEOUTS port_timeouts;

	port_timeouts.ReadIntervalTimeout = MAXDWORD;
	port_timeouts.ReadTotalTimeoutMultiplier = 0;
	port_timeouts.ReadTotalTimeoutConstant = 0;
	port_timeouts.WriteTotalTimeoutMultiplier = 0;
	port_timeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(port_handle_, &port_timeouts)) {
		FTLogError("Unable to set comport timeouts for port %s", port_name);
		CloseHandle(port_handle_);
		port_handle_ = INVALID_HANDLE_VALUE;
		return;
	}
}


SerialPort::~SerialPort() {
	if (port_handle_ != INVALID_HANDLE_VALUE)
		CloseHandle(port_handle_);
}

int SerialPort::read(uint8_t* buff, size_t size) {
	FTAssert(portIsOpen(), "Trying to read from closed port");

	DWORD n;

	if (size > 4096)
		size = 4096;
	ReadFile(port_handle_, buff, size, &n, nullptr);
	return n;
}

int SerialPort::write(const uint8_t* buff, size_t size) {
	FTAssert(portIsOpen(), "Trying to write to closed port");
	DWORD n;
	WriteFile(port_handle_, buff, size, &n, nullptr);
	return (int)n;
}