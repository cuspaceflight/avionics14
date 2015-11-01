import logging
import logging.config
import datetime
import base64
import serial
import struct
import sys
import urwid
import urwid.curses_display


logger = logging.getLogger("groundstation")

logging_cfg = {
    "version": 1,
    "disable_existing_loggers": True,
    "formatters": {
        "compact": {
            "format": "[%(asctime)s.%(msecs)03.0fZ] %(message)s",
            "datefmt": "%Y-%m-%dT%H:%M:%S"
        },
        "verbose": {
            "format": "[%(asctime)s.%(msecs)03.0fZ] %(levelname)s: "
                      "%(name)s: %(message)s",
            "datefmt": "%Y-%m-%dT%H:%M:%S"
        }
    },
    "handlers": {
        "console": {
            "level": "INFO",
            "class": "logging.StreamHandler",
            "formatter": "compact"
        },
        "file": {
            "level": "INFO",
            "class": "logging.handlers.WatchedFileHandler",
            "filename": "groundstation.log",
            "formatter": "verbose"
        }
    },
    "loggers": {
        "groundstation": {
            "level": "DEBUG",
            "handlers": ["file"],
        }
    }
}

mode_formats = {
    0: "8s", 1: "q", 2: "Q", 3: "ii", 4: "II", 5: "hhhh", 6: "HHHH",
    7: "bbbbbbbb", 8: "BBBBBBBB", 9: "ff", 10: "d"
}


origins = {
    0: "RESERVED", 1: "M2FC BODY", 2: "M2FC NOSE", 3: "M2R",
    4: "BADGER3-1", 5: "BADGER3-2"
}

axes = {0: "X", 1: "Y", 2: "Z"}

gps_locks = {
    0: "None", 1: "DR Only", 2: "2D", 3: "3D", 4: "GPS+DR", 5: "Time Only"}


mission_states = {
    0: "Pad", 1: "Ignition", 2: "Powered Ascent", 3: "Burnout",
    4: "Free Ascent", 5: "Ignite", 6: "Wait Ignition", 7: "Separate",
    8: "Separated Ascent", 9: "Apogee", 10: "Drogue Deploy",
    11: "Drogue Descent", 12: "Main Deploy", 13: "Main Descent",
    14: "Touchdown", 15: "Landed"
}


mission_state_wid = urwid.Text("", 'left', 'clip')
mission_time_wid = urwid.Text("", 'center', 'clip')
mission_utc_wid = urwid.Text("", 'right', 'clip')
cal_origin_wid = urwid.Text("", 'center', 'clip')
cal_tsf_wid = urwid.Text("", 'center', 'clip')
cal_lga_wid = urwid.Text("", 'center', 'clip')
cal_pad_wid = urwid.Text("", 'center', 'clip')
imu_lga_wid = urwid.Text("", 'center', 'clip')
imu_baro_wid = urwid.Text("", 'center', 'clip')
imu_gyro_wid = urwid.Text("", 'center', 'clip')
se_h_wid = urwid.Text("", 'center', 'clip')
se_v_wid = urwid.Text("", 'center', 'clip')
se_a_wid = urwid.Text("", 'center', 'clip')
pyro_cont_wid = urwid.Text("", 'center', 'clip')
pyro_fire_wid = urwid.Text("", 'center', 'clip')
loc_time_wid = urwid.Text("", 'center', 'clip')
loc_pos_wid = urwid.Text("", 'center', 'clip')
loc_alt_wid = urwid.Text("", 'center', 'clip')
loc_stat_wid = urwid.Text("", 'center', 'clip')
telem_pkts_wid = urwid.Text("", 'center', 'clip')
telem_chks_wid = urwid.Text("", 'center', 'clip')
telem_last_wid = urwid.Text("", 'center', 'clip')


imu_lga_scale = 1.0
imu_hga_scale = 1.0

last_times = {}


def time_since(what):
    if what not in last_times:
        last_times[what] = datetime.datetime.utcnow()
        return "???"
    delta = datetime.datetime.utcnow() - last_times[what]
    last_times[what] = datetime.datetime.utcnow()
    return "{:.1f}".format(delta.total_seconds())


def update_mission_state(state):
    mission_state_wid.set_text("State: {}".format(state))


def update_mission_time(t):
    mission_time_wid.set_text("T+{}s".format(t))
    mission_utc_wid.set_text("UTC: {}".format(
        datetime.datetime.utcnow().strftime("%H:%M:%S")))


def update_cal_origin(origin):
    cal_origin_wid.set_text([
        ('title', "Origin"), "\n{}".format(origin)])


def update_cal_tsf(tsf):
    cal_tsf_wid.set_text([
        ('title', "Timestamp Freq"), "\n{}MHz".format(tsf)])


def update_cal_lga(axis, scale):
    cal_lga_wid.set_text([
        ('title', "Low-G Accel"), "\n{}-axis, 1g={}LSB".format(axis, scale)])


def update_cal_pad(h_pad):
    cal_pad_wid.set_text([
        ('title', "Pad Altitude"), "\n{}m".format(h_pad)])


def update_imu_lga(x, y, z):
    last_t = time_since('imu_lga')
    imu_lga_wid.set_text([
        ('title', "Low-G Accel"),
        "\nx={} y={} z={} m/s/s\n({}s ago)".format(x, y, z, last_t)])


def update_imu_baro(pressure, temperature):
    last_t = time_since('imu_baro')
    imu_baro_wid.set_text([
        ('title', "Barometer"),
        "\n{}mbar, {}C\n({}s ago)".format(pressure, temperature, last_t)])


def update_imu_gyro(x, y, z):
    last_t = time_since('imu_gyro')
    imu_gyro_wid.set_text([
        ('title', "Gyroscope"),
        "\nx={} y={} z={} dps\n({}s ago)".format(x, y, z, last_t)])


def update_se_alt(alt):
    last_t = time_since('se_alt')
    se_h_wid.set_text([
        ('title', "Altitude"),
        "\n{}m\n({}s ago)".format(alt, last_t)])


def update_se_vel(vel):
    last_t = time_since('se_vel')
    se_v_wid.set_text([
        ('title', "Velocity"),
        "\n{}m/s\n({}s ago)".format(vel, last_t)])


def update_se_acc(acc):
    last_t = time_since('se_acc')
    se_a_wid.set_text([
        ('title', "Acceleration"),
        "\n{}m/s/s\n({}s ago)".format(acc, last_t)])


def update_pyro_cont(ch1, ch2, ch3, ch4):
    last_t = time_since('pyro_cont')
    pyro_cont_wid.set_text([
        ('title', "Continuity"),
        "\n{} {} {} {}\n({}s ago)".format(ch1, ch2, ch3, ch4, last_t)])


def update_pyro_fire(ch1, ch2, ch3, ch4):
    last_t = time_since('pyro_fire')
    pyro_fire_wid.set_text([
        ('title', "Firing"),
        "\n{} {} {} {}\n({}s ago)".format(ch1, ch2, ch3, ch4, last_t)])


def update_loc_time(hour, minute, sec):
    last_t = time_since('loc_time')
    loc_time_wid.set_text([
        ('title', "Time"),
        "\n{}:{}:{}\n({}s ago)".format(hour, minute, sec, last_t)])


def update_loc_pos(lat, lng):
    last_t = time_since('loc_pos')
    loc_pos_wid.set_text([
        ('title', "Location"),
        "\n{},{}\n({}s ago)".format(lat, lng, last_t)])


def update_loc_alt(alt):
    last_t = time_since('loc_alt')
    loc_alt_wid.set_text([
        ('title', "Altitude"),
        "\n{}\n({}s ago)".format(alt, last_t)])


def update_loc_stat(lock, svs):
    last_t = time_since('log_stat')
    loc_stat_wid.set_text([
        ('title', "Status"),
        "\nLock={} #SVs={}\n({}s ago)".format(lock, svs, last_t)])


def update_telem(pkts, chks, last):
    telem_pkts_wid.set_text([
        ('title', "Packets Received"),
        "\n{}".format(pkts)])
    telem_chks_wid.set_text([
        ('title', "Valid Checksums"),
        "\n{}".format(chks)])
    telem_last_wid.set_text([
        ('title', "Latest Packet"),
        "\n{}s ago".format(last)])


def sys_init(data):
    origin = data[0].decode()
    logger.info("System: Initialisation: {}".format(origin))
    update_cal_origin(origin)


def cal_tf(data):
    tsf = data[0]
    logger.info("Calibration: Timestamp Frequency: {}Hz".format(tsf))
    update_cal_tsf(tsf / 1E6)


def cal_lga(data):
    axis = data[0]
    scale = data[1]
    logger.info("Calibration: Low-G Accel: axis={} 1g={}".format(axis, scale))
    update_cal_lga(axes[axis], scale)
    global imu_lga_scale
    imu_lga_scale = scale


def cal_hga(data):
    axis = data[0]
    scale = data[1]
    logger.info("Calibration: High-G Accel: axis={} 1g={}"
                .format(axis, scale))
    global imu_hga_scale
    imu_hga_scale = scale


def cal_baro_1(data):
    logger.info("Calibration: Barometer 1: d0={} c1={} c2={} c3={}"
                .format(*data))


def cal_baro_2(data):
    logger.info("Calibration: Barometer 2: c4={} c5={} c6={} d7={}"
                .format(*data))


def cal_pad_alt(data):
    h_pad = data[0]
    logger.info("Calibration: Pad Altitude: {}m ASL".format(h_pad))
    update_cal_pad("{:.2f}".format(h_pad))


def imu_lga(data):
    x, y, z = data[:3]
    logger.info("IMU: Low-G Accel: x={} y={} z={}" .format(x, y, z))
    x /= imu_lga_scale
    y /= imu_lga_scale
    z /= imu_lga_scale
    x *= 9.81
    y *= 9.81
    z *= 9.81
    update_imu_lga("{:.2f}".format(x), "{:.2f}".format(y), "{:.2f}".format(z))


def imu_hga(data):
    x, y, z = data[:3]
    logger.info("IMU: Hi-G Accel: x={} y={} z={}".format(x, y, z))
    x /= imu_hga_scale
    y /= imu_hga_scale
    z /= imu_hga_scale
    x *= 9.81
    y *= 9.81
    z *= 9.81


def imu_baro(data):
    pres, temp = data
    logger.info("IMU: Baro: pressure={} temperature={}".format(pres, temp))
    update_imu_baro(int(pres/100.0), int(temp/100.0))


def imu_gyro(data):
    x, y, z = data[:3]
    logger.info("IMU: Gyro: x={} y={} z={}".format(x, y, z))
    x = int(x*17.5/1000.0)
    y = int(17.5/1000.0)
    z = int(17.5/1000.0)
    update_imu_gyro(x, y, z)


def imu_magno(data):
    logger.info("IMU: Magno: x={} y={} z={}".format(data[0], data[1], data[2]))


def ext_batt(data):
    logger.info("External: Battery: {}mV".format(data[0]))


def ext_sg(data):
    logger.info("External: Strain Gauge: ch1={} ch2={} ch3={}"
                .format(*data[:3]))


def ext_tc(data):
    logger.info("External: Thermocouple: ch1={} ch2={} ch3={}"
                .format(*data[:3]))


def state_mc(data):
    old, new = mission_states[data[0]], mission_states[data[1]]
    logger.info("State Machine: Mission Control: {}->{}" .format(old, new))
    update_mission_state(new)


def se_pred1(data):
    dt, h = data
    logger.info("State Estimation: Predicton 1: dt={} height={}".format(dt, h))
    update_se_alt("{:.2f}".format(h))


def se_pred2(data):
    v, a = data
    logger.info("State Estimation: Predicton 2: vel={} accel={}".format(v, a))
    update_se_vel("{:.2f}".format(v))
    update_se_acc("{:.2f}".format(a))


def se_pressure(data):
    logger.info("State Estimation: Pressure: sensor={} estimate={}"
                .format(*data))


def se_accel(data):
    logger.info("State Estimation: Accel: sensor={} estimate={}".format(*data))


def pyro_cont(data):
    ch1 = "ch1" if data[0] else " "
    ch2 = "ch2" if data[1] else " "
    ch3 = "ch3" if data[2] else " "
    ch4 = "ch4" if data[3] else " "
    logger.info("Pyro: Continuity: ch1={} ch2={} ch3={} ch4={}".format(*data))
    update_pyro_cont(ch1, ch2, ch3, ch4)


def pyro_fired(data):
    ch1 = "ch1" if data[0] else " "
    ch2 = "ch2" if data[1] else " "
    ch3 = "ch3" if data[2] else " "
    ch4 = "ch4" if data[3] else " "
    logger.info("Pyro: Fired: ch1={} ch2={} ch3={} ch4={}".format(*data))
    update_pyro_fire(ch1, ch2, ch3, ch4)


def loc_gps_time(data):
    hr = "{:02d}".format(data[4])
    mn = "{:02d}".format(data[5])
    sc = "{:02d}".format(data[6])
    logger.info("Location: GPS Time: {:02d}-{:02d}-{:02d} {}:{}:{}"
                " (valid={})"
                .format(data[0] | (data[1] << 8), data[2], data[3],
                        hr, mn, sc, data[7]))
    update_loc_time(hr, mn, sc)


def loc_gps_pos(data):
    lat = "{:8.5f}".format(data[0])
    lng = "{:8.5f}".format(data[1])
    logger.info("Location: GPS Position: lat={} lng={}".format(lat, lng))
    update_loc_pos(lat, lng)


def loc_gps_alt(data):
    logger.info("Location: GPS Altitude: height={} height_msl={}"
                .format(*data))
    update_loc_alt(int(data[0]))


def loc_gps_status(data):
    fix = gps_locks[data[0]]
    flags = data[1]
    svs = data[2]
    logger.info("Location: GPS Status: fix={} flags={} num_sv={}"
                .format(fix, flags, svs))
    update_loc_stat(fix, svs)


packet_functions = {
    0x00: sys_init,
    0x10: cal_tf, 0x11: cal_lga, 0x12: cal_hga,
    0x13: cal_baro_1, 0x14: cal_baro_2, 0x15: cal_pad_alt,
    0x20: imu_lga, 0x21: imu_hga, 0x22: imu_baro,
    0x23: imu_gyro, 0x24: imu_magno,
    0x30: ext_batt, 0x31: ext_sg, 0x32: ext_tc,
    0x40: state_mc,
    0x50: se_pred1, 0x51: se_pred2, 0x52: se_pressure, 0x53: se_accel,
    0x60: pyro_cont, 0x61: pyro_fired,
    0x70: loc_gps_time, 0x71: loc_gps_pos, 0x72: loc_gps_alt,
    0x73: loc_gps_status
}


mission_line = urwid.AttrMap(urwid.LineBox(urwid.Columns(
    (mission_state_wid, mission_time_wid, mission_utc_wid), 1), "MISSION"),
    'green')
cal_line = urwid.LineBox(urwid.Columns(
    (cal_origin_wid, cal_tsf_wid, cal_lga_wid, cal_pad_wid), 1), "CALIBRATION")
imu_line = urwid.LineBox(urwid.Columns(
    (imu_lga_wid, imu_baro_wid, imu_gyro_wid), 1), "INERTIAL MEASUREMENT")
se_line = urwid.LineBox(urwid.Columns(
    (se_h_wid, se_v_wid, se_a_wid), 1), "STATE ESTIMATION")
pyro_line = urwid.LineBox(urwid.Columns(
    (pyro_cont_wid, pyro_fire_wid), 1), "PYROTECHNICS")
loc_line = urwid.LineBox(urwid.Columns(
    (loc_time_wid, loc_pos_wid, loc_alt_wid, loc_stat_wid), 1), "GPS LOCATION")
telem_line = urwid.LineBox(urwid.Columns(
    (telem_pkts_wid, telem_chks_wid, telem_last_wid), 1), "TELEMETRY")

line_pile = urwid.Pile((mission_line, cal_line, loc_line,
                        imu_line, se_line, telem_line, pyro_line))
pile_fill = urwid.Filler(line_pile, 'top')

palette = [
    ('green', 'dark green', 'black'),
    ('title', 'default,bold', 'black'),
]


def parse_packet(pkt):
    ts, meta, chan, chk = struct.unpack("IBBH", pkt[0:8])
    origin, mode = meta >> 4, meta & 0xF
    data = struct.unpack(mode_formats[mode], pkt[8:])
    return ts, origin, mode, chan, chk, data


def checksum(pkt):
    crc = 0
    for i in range(16):
        if i not in (6, 7):
            crc ^= pkt[i] << 8
        for j in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc


def main(port):
    ser = serial.Serial(port, 115200)
    last_ts = t_corr = 0
    tsf = 168E6
    n_pkts = 0
    n_chks = 0

    update_mission_state("???")
    update_mission_time("???")
    update_cal_origin("???")
    update_cal_tsf("???")
    update_cal_lga("???", "???")
    update_cal_pad("???")
    update_imu_lga("?", "?", "?")
    update_imu_baro("?", "?")
    update_imu_gyro("?", "?", "?")
    update_se_alt("?")
    update_se_vel("?")
    update_se_acc("?")
    update_pyro_cont("?", "?", "?", "?")
    update_pyro_fire("?", "?", "?", "?")
    update_loc_time("??", "??", "??")
    update_loc_pos("???", "???")
    update_loc_alt("???")
    update_loc_stat("?", "?")
    update_telem(0, 0, "?")

    mainloop = urwid.MainLoop(pile_fill, palette=palette)

    logger.info("Starting main loop...")
    mainloop.start()
    mainloop.draw_screen()

    # def update(mainloop, arg):
    while True:
        line = ser.readline().decode().strip("\n")
        logger.info(">>> %s", line)
        if len(line) != 32:
            continue
        n_pkts += 1
        pkt = base64.b16decode(line)
        ts, origin, mode, chan, chk, data = parse_packet(pkt)
        if chan == 0x00:
            logger.info("    Saw init msg, resetting t_corr")
            t_corr = 0
        elif chan == 0x10:
            tsf = float(data[0])
            logger.info("    Saw timer freq, using new value %f", tsf)
        if ts < last_ts:
            t_corr += 0xFFFFFFFF
        last_ts = ts
        ts += t_corr
        time_s = "{:.03f}".format(ts / tsf)
        update_mission_time(time_s)
        update_telem(n_pkts, n_chks, time_since('telem'))
        our_chk = checksum(pkt)
        chkv = str(our_chk == chk)
        logger.info("    Time=%s Origin=%s Check=%s", time_s, origins[origin],
                    chkv)
        if our_chk == chk:
            n_chks += 1
        else:
            continue
        if chan in packet_functions:
            packet_functions[chan](data)
        # mainloop.set_alarm_in(0.1, update)
        mainloop.draw_screen()

    # mainloop.set_alarm_in(0.1, update)
    # mainloop.run()


if __name__ == "__main__":
    logging.config.dictConfig(logging_cfg)
    logger.info("Starting up")
    if len(sys.argv) != 2:
        print("Usage: {} <serial device>".format(sys.argv[0]))
    else:
        main(sys.argv[1])
