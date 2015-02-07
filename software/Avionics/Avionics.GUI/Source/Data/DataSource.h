#pragma once
#include <FTObject.h>
#include <stdint.h>
extern "C" {
#include <state_estimate.h>
#include <telemetry.h>
}

class DataSource : public FTObject {
public:
	virtual void update(uint64_t dt, state_estimate_t* estimate) = 0;

protected:
	DataSource();
	~DataSource();

	void handlePacket(const telemetry_t& packet);
};