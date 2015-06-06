import ctypes
import numpy as np
import matplotlib.pyplot as plt

libmission = ctypes.CDLL("./libmission.so")

state_names = {
    0: "PAD", 1: "IGNITION", 2: "POWERED_ASCENT", 3: "BURNOUT",
    4: "FREE_ASCENT", 5: "IGNITE", 6: "WAIT_IGNITION", 7: "SEPARATE",
    8: "SEPARATED_ASCENT", 9: "APOGEE", 10: "DROGUE_DEPLOY",
    11: "DROGUE_DESCENT", 12: "MAIN_DEPLOY", 13: "MAIN_DESCENT",
    14: "TOUCHDOWN", 15: "LANDED"
}


class StateEstimate(ctypes.Structure):
    _fields_ = [
        ("h", ctypes.c_float),
        ("v", ctypes.c_float),
        ("a", ctypes.c_float)
    ]


class InstanceData(ctypes.Structure):
    _fields_ = [
        ("t_launch", ctypes.c_int32),
        ("t_last_ignite_attempt", ctypes.c_int32),
        ("t_last_ignition", ctypes.c_int32),
        ("t_last_burnout", ctypes.c_int32),
        ("t_apogee", ctypes.c_int32),
        ("current_stage_position", ctypes.c_int),
        ("state", StateEstimate)
    ]

mm1 = 1.0
mm2 = 0.8
m_first_stage = 1.0
m_second_stage = 2.0

t_ignite = 15.0
t_burnout = 17.0
t_ignite_2 = 18.0
t_burnout_2 = 19.0

t = np.linspace(0, 60, 60000)
h = np.zeros(60000)
v = np.zeros(60000)
a = np.zeros(60000)
m = np.zeros(60000)
m[0] = m_first_stage + m_second_stage + mm1 + mm2
dt = t[1] - t[0]

drogue = np.zeros(60000)
main = np.zeros(60000)

istate = StateEstimate(0.0, 0.0, 0.0)
idata = InstanceData(0, 0, 0, 0, 0, 2, istate)
cur_state = 0
states = [(0.0, 0)]


for i in range(1, len(t)):
    if main[i]:
        cd = 0.4
        area = 0.3
    elif drogue[i]:
        cd = 0.3
        area = 0.1
    else:
        cd = 0.2
        area = 0.01

    m[i] = m[i-1]

    if h[i-1] <= 0:
        h[i-1] = 0.0
        v[i-1] = 0.0
        a[i] = 0.0
    else:
        a[i] = -9.81
        a[i] -= np.sign(v[i-1]) * (0.5 * 1.275 * v[i-1]**2 * cd * area) / m[i]
        a[i] += np.random.randn(1) * 0.1

    if t_ignite <= t[i] < t_burnout:
        a[i] += 170.0 / m[i]
        m[i] -= (mm1 / (t_burnout - t_ignite)) * dt

    if t[i] == t_burnout:
        m[i] -= m_first_stage

    if t_ignite_2 <= t[i] < t_burnout_2:
        a[i] += 130.0 / m[i]
        m[i] -= (mm2 / (t_burnout - t_ignite)) * dt

    v[i] = v[i-1] + (t[i] - t[i-1]) * a[i]
    h[i] = h[i-1] + (t[i] - t[i-1]) * v[i]

    libmission.chSetTimeNow(int(1000 * t[i]))

    state = StateEstimate(h[i], v[i], a[i])
    idata.state = state
    new_state = libmission.run_state(cur_state, ctypes.byref(idata))
    if new_state != cur_state:
        cur_state = new_state
        states.append((t[i], new_state))
        if state_names[new_state] == "DROGUE_DEPLOY":
            drogue[i:] = 1
        if state_names[new_state] == "MAIN_DEPLOY":
            main[i:] = 1


def plotstates():
    for ts, s in states:
        plt.axvline(ts)
        ydisp = plt.gca().transAxes.transform((0, 1-(s+0.8)/16.0))[1]
        ydata = plt.gca().transData.inverted().transform((0, ydisp))[1]
        plt.text(ts, ydata, " "+state_names[int(s)])


plt.subplot(311)
plt.plot(t, h)
plt.title("Altitude")
plotstates()

plt.subplot(312)
plt.plot(t, v)
plt.title("Velocity")
plotstates()

plt.subplot(313)
plt.plot(t, a)
plt.title("Acceleration")
plotstates()

plt.show()
