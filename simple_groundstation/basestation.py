import logging
import logging.config
import base64
import serial
import struct
import sys

logger = logging.getLogger("basestation")

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
            "filename": "basestation.log",
            "formatter": "verbose"
        }
    },
    "loggers": {
        "basestation": {
            "level": "DEBUG",
            "handlers": ["console", "file"],
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


mission_states = {
    0: "Pad", 1: "Ignition", 2: "Powered Ascent", 3: "Burnout",
    4: "Free Ascent", 5: "Ignite", 6: "Wait Ignition", 7: "Separate",
    8: "Separated Ascent", 9: "Apogee", 10: "Drogue Deploy",
    11: "Drogue Descent", 12: "Main Deploy", 13: "Main Descent",
    14: "Touchdown", 15: "Landed"
}


def sys_init(data):
    return "System: Initialisation: {}".format(data)


def cal_tf(data):
    return "Calibration: Timestamp Frequency: {}Hz".format(data[0])


def cal_lga(data):
    return ("Calibration: Low-G Accel: axis={} 1g={}"
            .format(data[0], data[1]))


def cal_hga(data):
    return ("Calibration: High-G Accel: axis={} 1g={}"
            .format(data[0], data[1]))


def cal_baro_1(data):
    return ("Calibration: Barometer 1: d0={} c1={} c2={} c3={}"
            .format(*data))


def cal_baro_2(data):
    return ("Calibration: Barometer 2: c4={} c5={} c6={} d7={}"
            .format(*data))


def cal_pad_alt(data):
    return "Calibration: Pad Altitude: {}m ASL".format(data[0])


def imu_lga(data):
    return "IMU: Low-G Accel: x={} y={} z={}".format(data[0], data[1], data[2])


def imu_hga(data):
    return "IMU: Hi-G Accel: x={} y={} z={}".format(data[0], data[1], data[2])


def imu_baro(data):
    return "IMU: Baro: pressure={} temperature={}".format(data[0], data[1])


def imu_gyro(data):
    return "IMU: Gyro: x={} y={} z={}".format(data[0], data[1], data[2])


def imu_magno(data):
    return "IMU: Magno: x={} y={} z={}".format(data[0], data[1], data[2])


def ext_batt(data):
    return "External: Battery: {}mV".format(data[0])


def ext_sg(data):
    return "External: Strain Gauge: ch1={} ch2={} ch3={}".format(*data[:3])


def ext_tc(data):
    return "External: Thermocouple: ch1={} ch2={} ch3={}".format(*data[:3])


def state_mc(data):
    return ("State Machine: Mission Control: {}->{}"
            .format(mission_states[data[0]], mission_states[data[1]]))


def se_pred1(data):
    return "State Estimation: Predicton 1: dt={} height={}".format(*data)


def se_pred2(data):
    return "State Estimation: Predicton 2: velocity={} accel={}".format(*data)


def se_pressure(data):
    return "State Estimation: Pressure: sensor={} estimate={}".format(*data)


def se_accel(data):
    return "State Estimation: Accel: sensor={} estimate={}".format(*data)


def pyro_cont(data):
    return "Pyro: Continuity: ch1={} ch2={} ch3={} ch4={}".format(*data)


def pyro_fired(data):
    return "Pyro: Fired: ch1={} ch2={} ch3={} ch4={}".format(*data)


def loc_gps_time(data):
    return ("Location: GPS Time: {:02d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}"
            " (valid={})"
            .format(data[0] | (data[1] << 8), *data[2:]))


def loc_gps_pos(data):
    return "Location: GPS Position: lat={} lng={}".format(*data)


def loc_gps_alt(data):
    return "Location: GPS Altitude: height={} height_msl={}".format(*data)


def loc_gps_status(data):
    return ("Location: GPS Status: fix={} flags={} num_sv={}"
            .format(data[0], data[1], data[2]))


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
    last_timestamp = t_correction = 0
    ts_freq = 168E6
    while True:
        line = ser.readline().decode().strip("\n")
        logger.info(">>> %s", line)
        if len(line) != 32:
            continue
        pkt = base64.b16decode(line)
        ts, origin, mode, chan, chk, data = parse_packet(pkt)
        if chan == 0x00:
            logger.info("    Initialisation message seen, resetting time")
            t_correction = 0
        elif chan == 0x10:
            logger.info("    Timer frequency message seen, storing frequency")
            ts_freq = float(data[0])
        if ts < last_timestamp:
            t_correction += 0xFFFFFFFF
        last_timestamp = ts
        ts += t_correction
        time_s = ts / ts_freq
        our_check = checksum(pkt)
        chkv = str(our_check == chk)
        logger.info("    Time=%f Origin=%s Checksum=%s",
                    time_s, origins[origin], chkv)
        if chan in packet_functions:
            logger.info(packet_functions[chan](data))
        print()

if __name__ == "__main__":
    logging.config.dictConfig(logging_cfg)
    logger.info("Starting up")
    if len(sys.argv) != 2:
        print("Usage: {} <serial device>".format(sys.argv[0]))
    else:
        main(sys.argv[1])
