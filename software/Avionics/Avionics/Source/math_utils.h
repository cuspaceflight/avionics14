#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <float.h>
#include <math.h>

#define PI 3.14159265358979323846f

static float mat3x3_det(float mat[3][3]) {
	return mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2])
		- mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
		+ mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
}

static float vector_dot(float a[3], float b[3]) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static void vector_cross(float a[3], float b[3], float out[3]) {
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

static float vector_cross_mag(float a[3], float b[3]) {
	float x = a[1] * b[2] - a[2] * b[1];
	float y = a[2] * b[0] - a[0] * b[2];
	float z = a[0] * b[1] - a[1] * b[0];

	return sqrtf(x*x + y*y + z*z);
}

static void axis_angle_to_euler(float axis[3], float angle, float out[3]) {
	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1 - c;

	float x = axis[0];
	float y = axis[1];
	float z = axis[2];

	float singularity_test = (x * y * t - z * s);

	if (singularity_test > 0.998) {
		return;
	}
	if (singularity_test < -0.998) {
		return;
	}

	out[0] = atan2f(y * s - x * z * t, 1 - (y*y + z*z) * t);
	out[1] = asinf(x * y * t + z * s);
	out[2] = atan2f(x * s - y * z * t, 1 - (x*x + z*z) * t);
}

// Warning: This code does not handle gimbal lock and so should not be used for anything crucial
// It will return incorrect data y = +-pi/2
// It is meant as a function to help debug quaternions
// It uses xyz ordering (ie x rotation followed by y rotation followed by z rotation)
static void quat_to_euler(float q[4], float xyz[3]) {
	xyz[0] = -atan2f(2 * (q[1] * q[2] - q[0] * q[3]), 1 - 2 * (q[0]*q[0] + q[1]*q[1]));
	xyz[1] = asinf(2 * (q[0] * q[2] + q[1] * q[3]));
	xyz[2] = -atan2f(2 * (q[0] * q[1] - q[2] * q[3]), 1 - 2 * (q[1]*q[1] + q[2]*q[2]));
	
	
	/*xyz[0] = atan2f(2.0f * (q[1] * q[2] + q[3] * q[0]), q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2]);
	xyz[1] = asinf(-2.0f * (q[0] * q[2] - q[3] * q[1]));
	xyz[2] = atan2f(2.0f * (q[0] * q[1] + q[3] * q[2]), q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2]);*/
}

static void axis_angle_to_quat(float axis[3], float angle, float q[4]) {
	float s = sinf(angle / 2.0f);
	float x = axis[0] * s;
	float y = axis[1] * s;
	float z = axis[2] * s;
	float w = cosf(angle / 2.0f);

	float norm = x*x + y*y + z*z + w*w;
	q[0] = x / norm;
	q[1] = y / norm;
	q[2] = z / norm;
	q[3] = w / norm;
}
#endif /* MATH_UTILS_H */