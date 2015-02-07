#include "mag_rotation_estimator.h"
#include <math_utils.h>

float MO[3];
float MR[3];

void mag_rotation_estimator_set_reference_vector(float mag_reference[3]) {
	float norm = sqrtf(mag_reference[0] * mag_reference[0] + mag_reference[1] * mag_reference[1] + mag_reference[2] * mag_reference[2]);

	MR[0] = mag_reference[0] / norm;
	MR[1] = mag_reference[1] / norm;
	MR[2] = mag_reference[2] / norm;
}

void mag_rotation_estimator_new_mag(float mag[3]) {
	float norm = sqrtf(mag[0] * mag[0] + mag[1] * mag[1] + mag[2] * mag[2]);
	MO[0] = mag[0] / norm;
	MO[1] = mag[1] / norm;
	MO[2] = mag[2] / norm;
}

void mag_rotation_estimator_update(float euler_angles[3], float quaternion[4]) {
	float axis[3];
	float temp[3];
	vector_cross(MR, MO, axis);
	float angle = acosf(vector_dot(MR, MO));
	axis_angle_to_euler(axis, angle, euler_angles);
	axis_angle_to_quat(axis, angle, quaternion);
}