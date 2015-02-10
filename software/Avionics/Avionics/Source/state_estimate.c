#include "state_estimate.h"
#include <quest_estimator.h>
#include <mag_rotation_estimator.h>
#include <translation_kalman.h>
#include <calibration.h>
#include <logging.h>
#include <stdbool.h>

#define LOCK_STATE_ESTIMATE
#define UNLOCK_STATE_ESTIMATE

#define CALIBRATION_COUNT 1000
#define USE_QUEST

volatile bool is_calibrated = false;

float calibration_mag_sum[3] = { 0, 0, 0 };
int calibration_mag_count = 0;
float calibration_accel_sum[3] = { 0, 0, 0 };
int calibration_accel_count = 0;
float accel_bias[3];


void print_state_estimate(const state_estimate_t* estimate) {
	PRINT("State estimate { Position: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->pos[i]);
	PRINT(" ] Velocity: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->vel[i]);
	PRINT(" ] Acceleration: [");
	for (int i = 0; i < 3; ++i)
		PRINT(" %f", estimate->accel[i]);
	PRINT(" ]}\n");
}

void calibrate() {
	float mag_reference[3];

	accel_bias[0] = calibration_accel_sum[0] / (float)calibration_accel_count;
	accel_bias[1] = calibration_accel_sum[1] / (float)calibration_accel_count;
	accel_bias[2] = calibration_accel_sum[2] / (float)calibration_accel_count;

	

	mag_reference[0] = calibration_mag_sum[0] / (float)calibration_mag_count;
	mag_reference[1] = calibration_mag_sum[1] / (float)calibration_mag_count;
	mag_reference[2] = calibration_mag_sum[2] / (float)calibration_mag_count;
#ifdef USE_QUEST
	quest_estimator_set_reference_vectors(accel_bias, mag_reference);
#else
	mag_rotation_estimator_set_reference_vector(mag_reference);
#endif
	is_calibrated = true;
	PRINT("Calibrated!\n");
}

void state_estimate_new_accel_raw(const int16_t raw_accel[3]) {
	LOCK_STATE_ESTIMATE;
	float accel[3];
	calibrate_accel(raw_accel, accel);

	accel[0] = accel[0];
	accel[1] = accel[1];
	accel[2] = accel[2];

	if (is_calibrated) {
		

		// TODO Call with (accel - previous_state_accel)
#ifdef USE_QUEST
		quest_estimator_new_accel(accel);
#endif
		//PRINT("Accel %f %f %f\n", accel[0], accel[1], accel[2]);
		accel[0] -= accel_bias[0];
		accel[1] -= accel_bias[1];
		accel[2] -= accel_bias[2];
		//accel[1] -= GRAVITY;
		//PRINT("Accel %f %f %f\n", accel[0], accel[1], accel[2]);



		translation_kalman_new_accel(accel);
	} else {
		calibration_accel_sum[0] += accel[0];
		calibration_accel_sum[1] += accel[1];
		calibration_accel_sum[2] += accel[2];
		calibration_accel_count++;

		if (calibration_mag_count > CALIBRATION_COUNT && calibration_accel_count > CALIBRATION_COUNT)
			calibrate();
	}
	
	
	
	UNLOCK_STATE_ESTIMATE;
}

void state_estimate_new_magnetometer_raw(const int16_t raw_mag[3]) {
	LOCK_STATE_ESTIMATE;

	float mag[3];
	//calibrate_mag(raw_mag, mag);

	mag[0] = -raw_mag[0];
	mag[1] = raw_mag[1];
	mag[2] = -raw_mag[2];


	if (is_calibrated) {
		//PRINT("Mag %f %f %f\n", mag[0], mag[1], mag[2]);
#ifdef USE_QUEST
		quest_estimator_new_mag(mag);
#else
		mag_rotation_estimator_new_mag(mag);
#endif
	} else {
		calibration_mag_sum[0] += mag[0];
		calibration_mag_sum[1] += mag[1];
		calibration_mag_sum[2] += mag[2];
		calibration_mag_count++;

		if (calibration_mag_count > CALIBRATION_COUNT && calibration_accel_count > CALIBRATION_COUNT)
			calibrate();
	}

	UNLOCK_STATE_ESTIMATE;
}

void state_estimate_new_pressure_raw(int pressure) {
	LOCK_STATE_ESTIMATE;
	translation_kalman_new_pressure_raw((float)pressure);
	UNLOCK_STATE_ESTIMATE;
}

void state_estimate_compute_next(state_estimate_t* next_estimate, float dt) {
	if (!is_calibrated)
		return;
	
	LOCK_STATE_ESTIMATE;

#ifdef USE_QUEST
	quest_estimator_update(next_estimate->orientation_q, next_estimate->orientation_euler);
	PRINT("Orientation %f %f %f\n", next_estimate->orientation_euler[0] * 57.2957795131f, next_estimate->orientation_euler[1] * 57.2957795131f, next_estimate->orientation_euler[2] * 57.2957795131f);
#else
	mag_rotation_estimator_update(next_estimate->orientation_euler,next_estimate->orientation_q);
	PRINT("Orientation %f %f %f\n", next_estimate->orientation_euler[0] * 57.2957795131f, next_estimate->orientation_euler[1] * 57.2957795131f, next_estimate->orientation_euler[2] * 57.2957795131f);
#endif
	
	translation_kalman_prediction_step(next_estimate, dt);
	UNLOCK_STATE_ESTIMATE;
}