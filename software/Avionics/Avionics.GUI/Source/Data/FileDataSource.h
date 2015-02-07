#pragma once
#include <Data/DataSource.h>


class FileDataSource : public DataSource
{
public:
	FileDataSource(const char* filename);
	~FileDataSource();


	virtual void update(uint64_t dt, state_estimate_t* state) override;

protected:
	uint64_t simulation_time_;
	uint64_t packet_timestep_correction_;
	uint32_t last_packet_time_;
	uint32_t num_samples_;
	uint32_t simulation_index_;
	telemetry_t* data_input_;

	void loadFromBinaryFile(const char* filename);
};

