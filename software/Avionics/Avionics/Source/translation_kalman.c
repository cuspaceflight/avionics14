/*
* Translational State estimation and sensor fusion
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*
* Pressure calibration, next state covariance matrix
* calculation and tuning constants taken from Martlet 2
* Written by Adam Greig, Cambridge University Spaceflight
*
*/

#include "translation_kalman.h"
#include <math.h>

// Current state - [x,y,z],[position,velocity,acceleration]
static float current_state[3][3];

//Covariance storage - [x,y,z],[row],[column]
static float current_covariance[3][3][3] = {
		{ { 250.0f, 0.0f, 0.0f },
		{ 0.0f, 0.1f, 0.0f },
		{ 0.0f, 0.0f, 0.1f } },

		{ { 250.0f, 0.0f, 0.0f },
		{ 0.0f, 0.1f, 0.0f },
		{ 0.0f, 0.0f, 0.1f } },

		{ { 250.0f, 0.0f, 0.0f },
		{ 0.0f, 0.1f, 0.0f },
		{ 0.0f, 0.0f, 0.1f } }
};

/* Constants from the US Standard Atmosphere 1976 */
const float Rs = 8.31432f;
const float g0 = 9.80665f;
const float M = 0.0289644f;
const float Lb[7] = { -0.0065f, 0.0f, 0.001f, 0.0028f, 0.0f, -0.0028f, -0.002f };
const float Pb[7] = { 101325.0f, 22632.10f, 5474.89f, 868.02f, 110.91f, 66.94f, 3.96f };
const float Tb[7] = { 288.15f, 216.65f, 216.65f, 228.65f, 270.65f, 270.65f, 214.65f };
const float Hb[7] = { 0.0f, 11000.0f, 20000.0f, 32000.0f, 47000.0f, 51000.0f, 71000.0f };
volatile uint8_t state_estimation_trust_barometer = 1;
/* Functions to convert pressures to altitudes via the
* US Standard Atmosphere
*/

float state_estimation_p2a_nonzero_lapse(float p, int b);
float state_estimation_p2a_zero_lapse(float p, int b);

/*
*
* Predicted State generated using SUVAT
*
* Next covariance matrix generated using method copied from Marlet 2
* Written by Adam Greig
*/
void translation_kalman_prediction_step(state_estimate_t* state, float dt) {
	//This will be computed in firmware but set at 1 ms for now for simplicity
	//float dt = 1;
	float dt2 = dt * dt;
	float dt3 = dt * dt2;
	float dt4 = dt * dt3;
	float dt5 = dt * dt4;
	float dt6 = dt * dt5;
	float dt2_2 = dt2 / 2.0f;

	/* TODO Determine this q-value */
	float q = 500.0;

	int i;
	for (i = 0; i < 3; i++) {
		float* x = current_state[i];
		float(*p)[3] = current_covariance[i];

		x[0] += dt * x[1] + dt2_2*x[2];
		x[1] += dt * x[2];

		/* Update covariance
		* P_{k|k-1} = F_k P_{k-1|k-1} F'_k + Q
		* Uses F.P.F' from above. We'll add Q later, this is just the FPF'.
		* Conveniently the form means we can update each element in-place.
		*/
		p[0][0] += (dt * p[1][0] + dt2_2 * p[2][0]
			+ dt * (p[0][1] + dt * p[1][1] * dt2_2 * p[2][1])
			+ dt2_2 * (p[0][2] + dt * p[1][2] + dt2_2 * p[2][2]));
		p[0][1] += (dt * p[1][1] + dt2_2 * p[2][1]
			+ dt * (p[0][2] + dt * p[1][2] + dt2_2 * p[2][2]));
		p[0][2] += (dt * p[1][2] + dt2_2 * p[2][2]);
		p[1][0] += (dt * p[2][0]
			+ dt * (p[1][1] + dt * p[2][1])
			+ dt2_2 * (p[1][2] + dt * p[2][2]));
		p[1][1] += (dt * p[2][1]
			+ dt * (p[1][2] + dt * p[2][2]));
		p[1][2] += (dt * p[2][2]);
		p[2][0] += (dt * p[2][1] + dt2_2 * p[2][2]);
		p[2][1] += (dt * p[2][2]);
		/* Add process noise to matrix above.
		* P_{k|k-1} += Q
		*/
		p[0][0] += q * dt6 / 36.0f;
		p[0][1] += q * dt5 / 12.0f;
		p[0][2] += q * dt4 / 6.0f;
		p[1][0] += q * dt5 / 12.0f;
		p[1][1] += q * dt4 / 4.0f;
		p[1][2] += q * dt3 / 2.0f;
		p[2][0] += q * dt4 / 6.0f;
		p[2][1] += q * dt3 / 2.0f;
		p[2][2] += q * dt2 / 1.0f;

		state->pos[i] = x[0];
		state->vel[i] = x[1];
		state->accel[i] = x[2];
	}
}


/*
*
* Pressure Update Equations
*
* Pressure only effects y axis so extract this first
*
* x = current_state[1]
*
* As the pressure reading gives us altitude H = [1, 0, 0]
*
* The covariance matrix P is a 3x3 matrix
*
* The residual covariance S is therefore [P[0][0] + R]
*
* Therefore K is the transpose of [P[0][0]/S,P[1][0]/S,P[2][0]/S]
*
* Working through the matrix multiplications:
*
*       [k[0] * P[0][0], k[0] * P[0][1], k[0] * P[0][2]]
* P -=  [k[1] * P[0][0], k[1] * P[0][1], k[1] * P[0][2]]
*       [k[2] * P[0][0], k[2] * P[0][1], k[2] * P[0][2]]
*
*       [k[0] * (Z - x[0])]
* x +=  [k[1] * (Z - x[0])]
*       [k[2] * (Z - x[0])]
*/

void translation_kalman_new_pressure(float pressure) {
	/* Around 6.5Pa resolution on the barometer. */
	float baro_res = 6.5f;
	float h, hd;
	/* Discard data when mission control believes we are transonic. */
	if (!state_estimation_trust_barometer)
		return;
	/* Convert pressure reading into an altitude.
	* Run the same conversion for pressure + sensor resolution to get an idea
	* of the current noise variance in altitude terms for the filter.
	*/
	h = state_estimation_pressure_to_altitude(pressure);
	hd = state_estimation_pressure_to_altitude(pressure + baro_res);
	/* If there was an error (couldn't find suitable altitude band) for this
	* pressure, just don't use it. It's probably wrong. */
	if (h == -9999.0f || hd == -9999.0f)
		return;
	/* TODO: validate choice of r */
	float r = (h - hd) * (h - hd);

	float* x = current_state[1];
	float (*P)[3] = current_covariance[1];


	float s_inverse = 1 / (P[0][0] + r);

	float k[3];
	k[0] = P[0][0] * s_inverse;
	k[1] = P[1][0] * s_inverse;
	k[2] = P[2][0] * s_inverse;

	P[0][0] -= k[0] * P[0][0];
	P[0][1] -= k[0] * P[0][1];
	P[0][2] -= k[0] * P[0][2];
	P[1][0] -= k[1] * P[0][0];
	P[1][1] -= k[1] * P[0][1];
	P[1][2] -= k[1] * P[0][2];
	P[2][0] -= k[2] * P[0][0];
	P[2][1] -= k[2] * P[0][1];
	P[2][2] -= k[2] * P[0][2];

	float error = h - x[0];

	x[0] += k[0] * error;
	x[1] += k[1] * error;
	x[2] += k[2] * error;
}


/*
*
* Accelerometer Update Equations
*
* Each axis is independent so we handle them separately
*
* x = current_state[i]
* Z = accel[i]
* for i = 0,1,2
*
* As the accel reading gives us acceleration H = [0, 0, 1]
*
* The covariance matrix P is a 3x3 matrix
*
* The residual covariance S is therefore [P[2][2] + R]
*
* Therefore K is the transpose of [P[0][2]/S,P[1][2]/S,P[2][2]/S]
*
* Working through the matrix multiplications:
*
*       [k[0] * P[2][0], k[0] * P[2][1], k[0] * P[2][2]]
* P -=  [k[1] * P[2][0], k[1] * P[2][1], k[1] * P[2][2]]
*       [k[2] * P[2][0], k[2] * P[2][1], k[2] * P[2][2]]
*
*       [k[0] * (Z - x[0])]
* x +=  [k[1] * (Z - x[0])]
*       [k[2] * (Z - x[0])]
*/

void translation_kalman_new_accel(const float* accel) {
	const float r = 0.2365f; //low-g value (high-g value is 7.6951f)

	int i;
	for (i = 0; i < 3; i++) {
		float* x = current_state[i];
		float(*P)[3] = current_covariance[i];

		float s_inverse = 1 / (P[2][2] + r);

		float k[3];
		k[0] = P[0][2] * s_inverse;
		k[1] = P[1][2] * s_inverse;
		k[2] = P[2][2] * s_inverse;

		P[0][0] -= k[0] * P[2][0];
		P[0][1] -= k[0] * P[2][1];
		P[0][2] -= k[0] * P[2][2];
		P[1][0] -= k[1] * P[2][0];
		P[1][1] -= k[1] * P[2][1];
		P[1][2] -= k[1] * P[2][2];
		P[2][0] -= k[2] * P[2][0];
		P[2][1] -= k[2] * P[2][1];
		P[2][2] -= k[2] * P[2][2];

		float error = accel[i] - x[2];

		x[0] += k[0] * error;
		x[1] += k[1] * error;
		x[2] += k[2] * error;
	}
}

float state_estimation_pressure_to_altitude(float pressure)
{
	int b;
	/* For each level of the US Standard Atmosphere 1976, check if the pressure
	* is inside that level, and use the appropriate conversion based on lapse
	* rate at that level.
	*/
	for (b = 0; b < 6; b++) {
		if (pressure <= Pb[b] && pressure > Pb[b + 1]) {
			if (Lb[b] == 0.0f) {
				return state_estimation_p2a_zero_lapse(pressure, b);
			}
			else {
				return state_estimation_p2a_nonzero_lapse(pressure, b);
			}
		}
	}
	/* If no levels matched, something is wrong, returning -9999f will cause
	* this pressure value to be ignored.
	*/
	return -9999.0f;
}
/*
* Convert a pressure and an atmospheric level b into an altitude.
* Reverses the standard equation for non-zero lapse regions,
* P = Pb (Tb / (Tb + Lb(h - hb)))^(M g0 / R* Lb)
*/
float state_estimation_p2a_nonzero_lapse(float pressure, int b)
{
	float lb = Lb[b];
	float hb = Hb[b];
	float pb = Pb[b];
	float tb = Tb[b];
	return hb + tb / lb * (powf(pressure / pb, (-Rs*lb) / (g0*M)) - 1.0f);
}
/* Convert a pressure and an atmospheric level b into an altitude.
* Reverses the standard equation for zero-lapse regions,
* P = Pb exp( -g0 M (h-hb) / R* Tb)
*/
float state_estimation_p2a_zero_lapse(float pressure, int b)
{
	float hb = Hb[b];
	float pb = Pb[b];
	float tb = Tb[b];
	return hb + (Rs * tb) / (g0 * M) * (logf(pressure) - logf(pb));
}
