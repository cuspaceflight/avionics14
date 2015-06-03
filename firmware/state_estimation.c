/*
 * State estimation and sensor fusion
 * M2FC
 * 2014 Adam Greig, Cambridge University Spaceflight
 */

#include <math.h>
#include "state_estimation.h"
#include "microsd.h"
#include "time_utils.h"

/* Kalman filter state and covariance storage */
static float x[3]    = {1180.0f, 0.0f, 0.0f};
static float p[3][3] = {{250.0f, 0.0f, 0.0f},
                        {  0.0f, 0.1f, 0.0f},
                        {  0.0f, 0.0f, 0.1f}};
static uint32_t t_clk = 0;

/* Lock to protect the global shared Kalman state */
static BinarySemaphore kalman_lock;

/* Constants from the US Standard Atmosphere 1976 */
const float Rs = 8.31432f;
const float g0 = 9.80665f;
const float M = 0.0289644f;
const float Lb[7] = {
    -0.0065f, 0.0f, 0.001, 0.0028f, 0.0f, -0.0028f, -0.002f};
const float Pb[7] = {
    101325.0f, 22632.10f, 5474.89f, 868.02f, 110.91f, 66.94f, 3.96f};
const float Tb[7] = {
    288.15f, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65};
const float Hb[7] = {
    0.0f, 11000.0f, 20000.0f, 32000.0f, 47000.0f, 51000.0f, 71000.0f};

volatile uint8_t state_estimation_trust_barometer;

/* Functions to convert pressures to altitudes via the
 * US Standard Atmosphere
 */
float state_estimation_pressure_to_altitude(float pressure);
float state_estimation_p2a_nonzero_lapse(float p, int b);
float state_estimation_p2a_zero_lapse(float p, int b);

/* Generic accelerometer Kalman filter update, used by both the high-g and
 * log-g public functions.
 */
void state_estimation_update_accel(float accel, float r);


/*
 * Run the Kalman prediction step and return the latest state estimate.
 *
 * Our Kalman state is x_k = [x_0  x_1  x_2] = [x  dx/dt  d²x/dt²]
 * i.e. [position  velocity  acceleration].
 *
 * The state transition matrix F is therefore (by integration):
 * F = [ 1  dt  dt²/2 ]
 *     [ 0   1  dt    ]
 *     [ 0   0   1    ]
 *
 * We model the system as undergoing a jerk d³x/dt³ whose value
 * j ~ N(0, q) which realises a constant value over each integration
 * period dt. This leads to a process noise w_k:
 *
 * delta acceleration = j dt
 * delta velocity     = j dt²/2
 * delta position     = j dt³/6
 * 
 * w_k = [j.dt  j.dt²/2  j.dt³/6]'
 *
 * We then find Q as E[w_k . w_k']:
 * Q = [ q dt^6 /36    q dt^5 /12    q dt^4 /6 ]
 *     [ q dt^5 /12    q dt^4 /4     q dt^3 /2 ]
 *     [ q dt^4 /6     q dt^3 /2     q dt^2 /1 ]
 *
 * We do not model any control input (there is none) so B.u=0.
 *
 * F.P.F' is the final quantity of interest for Kalman prediction.
 *
 * F.P.F' = [ 1  dt  dt²/2 ][ P00 P01 P02 ][ 1      0   0 ]
 *          [ 0   1  dt    ][ P10 P11 P12 ][ dt     1   0 ]
 *          [ 0   0   1    ][ P20 P21 P22 ][ dt²/2  dt  1 ]
 *
 *        = [
 *           [P00 + dt P10 + dt²/2 P20,
 *            P01 + dt P11 + dt²/2 P21,
 *            P02 + dt P12 + dt²/2 P22
 *           ],
 *           [         P10 + dt    P20,
 *                     P11 + dt    P21,
 *                     P12 + dt    P22
 *           ],
 *           [                     P20,
 *                                 P21,
 *                                 P22
 *           ]
 *          ] . F'
 *
 *        = [
 *           [         P00 + dt P10 + dt²/2 P20
 *             +    dt(P01 + dt P11 + dt²/2 P21)
 *             + dt²/2(P02 + dt P12 + dt²/2 P22),
 *
 *                     P01 + dt P11 + dt²/2 P21
 *             +    dt(P02 + dt P12 + dt²/2 P22),
 *
 *                     P02 + dt P12 + dt²/2 P22
 *           ],
 *           [         P10 + dt P20
 *             +    dt(P11 + dt P21)
 *             + dt²/2(P12 + dt P22),
 *
 *                     P11 + dt P21
 *             +    dt(P12 + dt P22),
 *
 *                     P12 + dt P22
 *           ],
 *           [
 *                     P20 + dt P21 + dt²/2 P22,
 *
 *                              P21 + dt    P22,
 *
 *                                          P22
 *           ]
 *          ]
 *
 * It's not pretty but it is what it is.
 */
state_estimate_t state_estimation_get_state()
{
    float q, dt, dt2, dt3, dt4, dt5, dt6, dt2_2;
    static uint8_t sbp_counter = 0;
    state_estimate_t x_out;

    /* TODO Determine this q-value */
    q = 500.0;

    /* Acquire lock */
    chBSemWait(&kalman_lock);

    /* Find elapsed time */
    dt = time_seconds_since(&t_clk);
    dt2 = dt * dt;
    dt3 = dt * dt2;
    dt4 = dt * dt3;
    dt5 = dt * dt4;
    dt6 = dt * dt5;
    dt2_2 = dt2 / 2.0f;

    /* Update state
     * x_{k|k-1} = F_k x_{k-1|k-1}
     *           = [x_0 + dt x_1 + dt²/2 x_2]
     *             [         x_1 + dt    x_2]
     *             [                     x_2]
     */
    x[0] = x[0] + dt * x[1] + dt * dt * x[2] / 2.0f;
    x[1] = x[1] + dt * x[2];
    x[2] = x[2];

    /* Update covariance
     * P_{k|k-1} = F_k P_{k-1|k-1} F'_k + Q
     * Uses F.P.F' from above. We'll add Q later, this is just the FPF'.
     * Conveniently the form means we can update each element in-place.
     */
    p[0][0] += (   dt    *  p[1][0] + dt2_2 * p[2][0]
                 + dt    * (p[0][1] + dt    * p[1][1] * dt2_2 * p[2][1])
                 + dt2_2 * (p[0][2] + dt    * p[1][2] + dt2_2 * p[2][2]));
    p[0][1] += (   dt    *  p[1][1] + dt2_2 * p[2][1]
                 + dt    * (p[0][2] + dt    * p[1][2] + dt2_2 * p[2][2]));
    p[0][2] += (   dt    *  p[1][2] + dt2_2 * p[2][2]);

    p[1][0] += (   dt    *  p[2][0]
                 + dt    * (p[1][1] + dt    * p[2][1])
                 + dt2_2 * (p[1][2] + dt    * p[2][2]));
    p[1][1] += (   dt    *  p[2][1]
                 + dt    * (p[1][2] + dt    * p[2][2]));
    p[1][2] += (   dt    *  p[2][2]);

    p[2][0] += (   dt    *  p[2][1] + dt2_2 * p[2][2]);
    p[2][1] += (   dt    *  p[2][2]);

    /* Add process noise to matrix above.
     * P_{k|k-1} += Q
     */
    p[0][0] += q * dt6 / 36.0f;
    p[0][1] += q * dt5 / 12.0f;
    p[0][2] += q * dt4 /  6.0f;
    p[1][0] += q * dt5 / 12.0f;
    p[1][1] += q * dt4 /  4.0f;
    p[1][2] += q * dt3 /  2.0f;
    p[2][0] += q * dt4 /  6.0f;
    p[2][1] += q * dt3 /  2.0f;
    p[2][2] += q * dt2 /  1.0f;

    /* Copy state to return struct */
    x_out.h = x[0];
    x_out.v = x[1];
    x_out.a = x[2];

    /* Release lock */
    chBSemSignal(&kalman_lock);

    /* Log the newly predicted state */
    microsd_log_f(CHAN_SE_P1, dt, x_out.h);
    microsd_log_f(CHAN_SE_P2, x_out.v, x_out.a);

    sbp_counter++;
    if(sbp_counter == 100) {
        SBP_SEND(0x22, x_out);
        sbp_counter = 0;
    }

    return x_out;
}

/*
 * Initialises the state estimation's shared variables.
 */
void state_estimation_init()
{
    state_estimation_trust_barometer = 0;
    chBSemInit(&kalman_lock, FALSE);
}

/* We run a Kalman update step with a new pressure reading.
 * The pressure is converted to an altitude (since that's what's in our state
 * and what is useful to reason about) using the US standard atmosphere via the
 * `state_estimation_pressure_to_altitude` function. We also use this function
 * to estimate the current sensor noise in altitude terms.
 *
 * We thus derive R, the sensor noise variance, as the altitude error band at
 * the current altitude squared, (alt(pressure) - alt(pressure+error))².
 *
 * Then the Kalman update is run, with:
 * z = [altitude]
 * H = [1 0 0]
 * y = z - Hx = [altitude - x_0]
 * s = H P H' + R = P00 + R
 * K = P H' s^-1 = [P00 * s^-1]
 *                 [P10 * s^-1]
 *                 [P20 * s^-1]
 * x_{k|k} = x_{k|k-1} + K . y
 *         = [x_0 + (P00 * 1/(P00+R)) * y]
 *           [x_1 + (P10 * 1/(P00+R)) * y]
 *           [x_2 + (P20 * 1/(P00+R)) * y]
 * P_{k|k} = (I - K H) P_{k|k-1}
 *         = [1-K0  0  0][P00 P01 P02]
 *           [ -K1  1  0][P10 P11 P12]
 *           [ -K2  0  1][P20 P21 P22]
 *         = [P00 - K0 P00    P01 - K0 P01    P02 - K0 P02]
 *           [P10 - K1 P00    P11 - K1 P01    P12 - K1 P02]
 *           [P20 - K2 P00    P21 - K2 P01    P22 - K2 P02]
 */
void state_estimation_new_pressure(float pressure)
{
    /* Around 6.5Pa resolution on the barometer. */
    float baro_res = 6.5f;

    float y, r, s_inv, k[3];
    float h, hd;

    /* Discard data when mission control believes we are transonic. */
    if(!state_estimation_trust_barometer)
        return;

    /* Convert pressure reading into an altitude.
     * Run the same conversion for pressure + sensor resolution to get an idea
     * of the current noise variance in altitude terms for the filter.
     */
    h = state_estimation_pressure_to_altitude(pressure);
    hd = state_estimation_pressure_to_altitude(pressure + baro_res);

    /* If there was an error (couldn't find suitable altitude band) for this
     * pressure, just don't use it. It's probably wrong. */
    if(h == -9999.0f || hd == -9999.0f)
        return;

    /* TODO: validate choice of r */
    r = (h - hd) * (h - hd);

    /* Acquire lock */
    chBSemWait(&kalman_lock);

    /* Measurement residual */
    y = h - x[0];

    /* Precision */
    s_inv = 1.0f / (p[0][0] + r);

    /* Compute optimal Kalman gains */
    k[0] = p[0][0] * s_inv;
    k[1] = p[1][0] * s_inv;
    k[2] = p[2][0] * s_inv;

    /* New state after measurement */
    x[0] += k[0] * y;
    x[1] += k[1] * y;
    x[2] += k[2] * y;

    /* Update P matrix post-measurement */
    p[0][0] -= k[0] * p[0][0];
    p[0][1] -= k[0] * p[0][1];
    p[0][2] -= k[0] * p[0][2];
    p[1][0] -= k[1] * p[0][0];
    p[1][1] -= k[1] * p[0][1];
    p[1][2] -= k[1] * p[0][2];
    p[2][0] -= k[2] * p[0][0];
    p[2][1] -= k[2] * p[0][1];
    p[2][2] -= k[2] * p[0][2];

    /* Log new pressure reading and the consequent new state altitude */
    microsd_log_f(CHAN_SE_U_P, pressure, x[0]);

    /* Release lock */
    chBSemSignal(&kalman_lock);
}

float state_estimation_pressure_to_altitude(float pressure)
{
    int b;
    /* For each level of the US Standard Atmosphere 1976, check if the pressure
     * is inside that level, and use the appropriate conversion based on lapse
     * rate at that level.
     */
    for(b = 0; b < 6; b++) {
        if(pressure <= Pb[b] && pressure > Pb[b+1]) {
            if(Lb[b] == 0.0f) {
                return state_estimation_p2a_zero_lapse(pressure, b);
            } else {
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

    return hb + tb/lb * (powf(pressure/pb, (-Rs*lb)/(g0*M)) - 1.0f);
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

    return hb + (Rs * tb)/(g0 * M) * (logf(pressure) - logf(pb));
}

/* Update the state estimate with a new low-g accel reading.
 * Readings near clipping (16g) are ignored.
 * The sensor noise is based on the datasheet RMS value at our sampling rate.
 *
 * Base noise value at 100Hz ODR is 1.1 LSB rms and increases by sqrt(2) every
 * doubling of the data rate.
 * Therefore at 800Hz we see 3.1 LSB and by 3200Hz we see 6.2 LSB noise.
 * Scale factor is 3.9 mg/LSB so our noise figures translate to
 * 12.09mg and 24.12mg noise, which is 0.1186 m/s/s and 0.2365 m/s/s
 * acceleration rms respectively.
 *
 * In theory these should be squared to find a variance, but in practice...
 *
 */
void state_estimation_new_lg_accel(float lg_accel)
{
    if(fabsf(lg_accel) > 150.0f) {
        /* The low-g accelerometer is limited to +-16g, so
         * we'll just discard anything above 150m/s/s (15.3g).
         */
        return;
    }

    state_estimation_update_accel(lg_accel, 0.2365f);
}

/* Update the state estimate with a new high-g accel reading.
 * The sensor noise is based on the datasheet RMS value at our sampling rate.
 *
 * Base noise is 5mg per sqrt(Hz).
 * At  800Hz ODR, noise rms is therefore 141mg, 1.383m/s/s
 * At 3200Hz ODR, noise rms is therefore 283mg, 2.774m/s/s
 *
 * Square RMS noise to get variance, 1.9127 or 7.6951.
 */
void state_estimation_new_hg_accel(float hg_accel)
{
    state_estimation_update_accel(hg_accel, 7.6951f);
}

/* Run the Kalman update for a single acceleration value.
 * Called internally from the hg and lg functions after
 * preprocessing.
 *
 * z = [accel]
 * H = [0 0 1]
 * y = z - Hx = [accel - x_2]
 * s = H P H' + R = P22 + R
 * K = P H' s^-1 = [P02 * s^-1]
 *                 [P12 * s^-1]
 *                 [P22 * s^-1]
 * x_{k|k} = x_{k|k-1} + K . y
 *         = [x_0 + (P02 * 1/(P22+R)) * y]
 *           [x_1 + (P12 * 1/(P22+R)) * y]
 *           [x_2 + (P22 * 1/(P22+R)) * y]
 * P_{k|k} = (I - K H) P_{k|k-1}
 *         = [1  0   -K0][P00 P01 P02]
 *           [0  1   -K1][P10 P11 P12]
 *           [0  0  1-K2][P20 P21 P22]
 *         = [P00 - K0 P20    P01 - K0 P21    P02 - K0 P22]
 *           [P10 - K1 P20    P11 - K1 P21    P12 - K1 P22]
 *           [P20 - K2 P20    P21 - K2 P21    P22 - K2 P22]
 */
void state_estimation_update_accel(float a, float r)
{
    float y, s_inv, k[3];

    /* Acquire lock */
    chBSemWait(&kalman_lock);

    /* Measurement residual */
    y = a - x[2];

    /* Precision */
    s_inv = 1.0f / (p[2][2] + r);

    /* Compute optimal Kalman gains */
    k[0] = p[0][2] * s_inv;
    k[1] = p[1][2] * s_inv;
    k[2] = p[2][2] * s_inv;

    /* Update state */
    x[0] += k[0] * y;
    x[1] += k[1] * y;
    x[2] += k[2] * y;

    /* Update covariance */
    p[0][0] -= k[0] * p[2][0];
    p[0][1] -= k[0] * p[2][1];
    p[0][2] -= k[0] * p[2][2];
    p[1][0] -= k[1] * p[2][0];
    p[1][1] -= k[1] * p[2][1];
    p[1][2] -= k[1] * p[2][2];
    p[2][0] -= k[2] * p[2][0];
    p[2][1] -= k[2] * p[2][1];
    p[2][2] -= k[2] * p[2][2];

    /* Log new acceleration value the consequent new state acceleration */
    microsd_log_f(CHAN_SE_U_A, a, x[2]);

    /* Release lock */
    chBSemSignal(&kalman_lock);
}


