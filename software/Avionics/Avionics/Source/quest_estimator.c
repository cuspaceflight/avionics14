#include "quest_estimator.h"
#include <math.h>
#include <logging.h>
#include "math_utils.h"
#include "state_estimate.h"

// Stores the last orientation output in euler angles
float current_orientation[3] = {0,0,0};
float MO[3] = { 1, 0, 0 };
float AO[3] = { 0, 1, 0 };


float MR[3] = { 0.948683298050514f, 0.316227766016838f, 0 };
float AR[3] = { 0, 0.986393923832144f, -0.164398987305357f };

// Tuning constants
const float AccelA = 0.5f;
const float MagA = 0.5f;


// We need to apply sequential rotations to avoid singularities
// 1 - rotation pi about x axis
// 2 - rotaiton pi about y axis
// 3 - rotation pi about z axis
int sequential_rotations = 0;

void quest_estimator_set_reference_vectors(float accel_reference[3], float mag_reference[3]) {
	float normAccel = sqrtf(accel_reference[0] * accel_reference[0] + accel_reference[1] * accel_reference[1] + accel_reference[2] * accel_reference[2]);
	float normMag = sqrtf(mag_reference[0] * mag_reference[0] + mag_reference[1] * mag_reference[1] + mag_reference[2] * mag_reference[2]);
	MR[0] = mag_reference[0] / normMag;
	MR[1] = mag_reference[1] / normMag;
	MR[2] = mag_reference[2] / normMag;

	AR[0] = accel_reference[0] / normAccel;
	AR[1] = accel_reference[1] / normAccel;
	AR[2] = accel_reference[2] / normAccel;

	PRINT("Accel Reference: %f %f %f\n",AR[0],AR[1],AR[2]);
	PRINT("Mag Reference: %f %f %f\n",MR[0],MR[1],MR[2]);
}

void quest_estimator_new_accel(const float accel[3]) {

	float normAccel = sqrtf(accel[0] * accel[0] + accel[1] * accel[1] + accel[2] * accel[2]);
	AO[0] = accel[0] / normAccel;
	AO[1] = accel[1] / normAccel;
	AO[2] = accel[2] / normAccel;

	switch (sequential_rotations) {
		case 1: {
			AO[1] = -AO[1];
			AO[2] = -AO[2];
			break;
		}
		case 2: {
			AO[0] = -AO[0];
			AO[2] = -AO[2];
			break;
		}
		case 3: {
			AO[1] = -AO[1];
			AO[0] = -AO[0];
			break;
		}
		default:
			break;
	}
}

void quest_estimator_new_mag(const float mag[3]) {
	float normMag = sqrtf(mag[0] * mag[0] + mag[1] * mag[1] + mag[2] * mag[2]);
	MO[0] = mag[0] / normMag;
	MO[1] = mag[1] / normMag;
	MO[2] = mag[2] / normMag;

	switch (sequential_rotations) {
		case 1: {
			MO[1] = -MO[1];
			MO[2] = -MO[2];
			break;
		}
		case 2: {
			MO[0] = -MO[0];
			MO[2] = -MO[2];
			break;
		}
		case 3: {
			MO[1] = -MO[1];
			MO[0] = -MO[0];
			break;
		}
		default:
			break;
	}
}

void rotate_measurement_data() {
	switch (sequential_rotations) {
		case 1: {
			MO[1] = -MO[1];
			MO[2] = -MO[2];
			AO[1] = -AO[1];
			AO[2] = -AO[2];
			break;
		}
		case 2: {
			MO[0] = -MO[0];
			MO[2] = -MO[2];
			AO[0] = -AO[0];
			AO[2] = -AO[2];
			break;
		}
		case 3: {
			MO[1] = -MO[1];
			MO[0] = -MO[0];
			AO[1] = -AO[1];
			AO[0] = -AO[0];
			break;
		}
		default:
			break;
	}
}

// Rotates the quaternion to match the sequential rotation selected
void rotate_quaternion(float q[4]) {
	switch (sequential_rotations) {
		case 0: {
			// Not totally sure why this is needed but the quaternion produced seems to have the y and z axis flipped
			q[1] = -q[1];
			q[2] = -q[2];
			return;
		}
		case 1: {
			float q0 = q[0];
			float q1 = q[1];

			q[0] = q[3];
			q[1] = -q[2];
			q[2] = q1;
			q[3] = -q0;

			return;
		}
		case 2: {
			float q0 = q[0];
			float q1 = q[1];
			q[0] = -q[2];
			q[1] = -q[3];
			q[2] = -q0;
			q[3] = -q1;

			return;
		}
		case 3: {
			float q2 = q[2];
			float q0 = q[0];
			q[0] = q[1];
			q[1] = q0;
			q[2] = -q[3];
			q[3] = -q2;

			return;
		}
		default:
			break;
	}
}

static void mat3x3_inv_transpose(float mat[3][3], float det, float res[3][3]) {
	float invdet = 1 / det;
	res[0][0] = (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) * invdet;
	res[1][0] = -(mat[0][1] * mat[2][2] - mat[0][2] * mat[2][1]) * invdet;
	res[2][0] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * invdet;
	res[0][1] = -(mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) * invdet;
	res[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * invdet;
	res[2][1] = -(mat[0][0] * mat[1][2] - mat[1][0] * mat[0][2]) * invdet;
	res[0][2] = (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) * invdet;
	res[1][2] = -(mat[0][0] * mat[2][1] - mat[2][0] * mat[0][1]) * invdet;
	res[2][2] = (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) * invdet;
}


void quest_estimator_update(float q[4], float euler_angles[3]) {
	
	// Make best guess for which sequential rotation to use
	//sequential_rotations = 0;
	if (current_orientation[0] > PI / 2.0f && sequential_rotations != 1) {
		// Rotation around x axis
		rotate_measurement_data();
		sequential_rotations = 1;
		rotate_measurement_data();
	} else if (current_orientation[1] > PI / 2.0f && sequential_rotations != 2) {
		// Rotation around y axis
		rotate_measurement_data();
		sequential_rotations = 2;
		rotate_measurement_data();
	} else if (current_orientation[2] > PI / 2.0f && sequential_rotations != 3) {
		// Rotation around z axis
		rotate_measurement_data();
		sequential_rotations = 3;
		rotate_measurement_data();
	} else if (sequential_rotations != 0) {
		// No rotation
		rotate_measurement_data();
		sequential_rotations = 0;
		rotate_measurement_data();
	}

	for (int attempt = 0; attempt < 4; attempt++) {
		float B[3][3];
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				B[row][col] = AO[row] * AR[col] * AccelA + MO[row] * MR[col] * MagA;
			}
		}

		float minus_s[3][3];
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				minus_s[row][col] = -B[row][col] - B[col][row];
			}
		}

		float sigma = MagA * (MR[0] * MO[0] + MR[1] * MO[1] + MR[2] * MO[2]) + AccelA * (AR[0] * AO[0] + AR[1] * AO[1] + AR[2] * AO[2]);

		float Z[3];

		Z[0] = B[1][2] - B[2][1];
		Z[1] = B[2][0] - B[0][2];
		Z[2] = B[0][1] - B[1][0];

		float deltaCos = vector_dot(MO, AO) * vector_dot(MR, AR) + vector_cross_mag(MO, AO) * vector_cross_mag(MR, AR);

		float lambda = sqrtf(MagA * MagA + 2 * MagA * AccelA * deltaCos + AccelA * AccelA);

		float lamda_plus_sig = lambda + sigma;

		minus_s[0][0] += lamda_plus_sig;
		minus_s[1][1] += lamda_plus_sig;
		minus_s[2][2] += lamda_plus_sig;

		float ymat[3][3];
		float det = mat3x3_det(minus_s);

		if (det < 1e-5f) {
			// We made the wrong sequential rotation assumption - try a different one
			// This shouldn't ever happen unless we are spinning really really fast
			rotate_measurement_data();
			sequential_rotations++;
			if (sequential_rotations > 3)
				sequential_rotations = 0;
			rotate_measurement_data();
			PRINT("Made incorrect first guess!");
			continue;
		}

		mat3x3_inv_transpose(minus_s, det, ymat);

		float a = B[0][1] - B[1][0];
		float b = B[0][2] - B[2][0];
		float c = B[1][2] - B[2][1];

		for (int i = 0; i < 3; i++) {
			q[i] = ymat[2][i] * a - ymat[1][i] * b + ymat[0][i] * c;
		}

		float qNorm = sqrtf(1.0f + q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
		q[3] = 1.0f / qNorm;

		for (int i = 0; i < 3; i++)
			q[i] /= qNorm;

		rotate_quaternion(q);

		quat_to_euler(q, current_orientation);

		euler_angles[0] = current_orientation[0];
		euler_angles[1] = current_orientation[1];
		euler_angles[2] = current_orientation[2];

		break;
	}
}
