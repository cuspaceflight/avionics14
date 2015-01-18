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

static float vector_cross_mag(float a[3], float b[3]) {
	float x = a[1] * b[2] - a[2] * b[1];
	float y = a[2] * b[0] - a[0] * b[2];
	float z = a[0] * b[1] - a[1] * b[0];

	return sqrtf(x*x + y*y + z*z);
}

// Warning: This code does not handle gimbal lock and so should not be used for anything crucial
// It will return incorrect data y = +-pi/2
// It is meant as a function to help debug quaternions
// It uses xyz ordering (ie x rotation followed by y rotation followed by z rotation)
static void quat_to_euler(float q[4], float xyz[3]) {
	xyz[0] = -atan2f(2 * (q[1] * q[2] - q[0] * q[3]), 1 - 2 * (q[0]*q[0] + q[1]*q[1]));
	xyz[1] = asinf(2 * (q[0] * q[2] + q[1] * q[3]));
	xyz[2] = -atan2f(2 * (q[0] * q[1] - q[2] * q[3]), 1 - 2 * (q[1]*q[1] + q[2]*q[2]));
}
#endif /* MATH_UTILS_H */