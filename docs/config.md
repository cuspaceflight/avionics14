# Badger 3 Config Files

Configuration settings are saved in a file named `config.txt` in the root of 
the SD card. Each line is one `key=value` pair, with values interpreted as 
either integers or floating point numbers. Bools should be specified as 0 or 1.

Configuration keys must be *in order* and exactly as follows:


Key              | Type  | Description
-----------------|-------|-----------------------------------------------------
stage            | Int   | 1 for bottom, 2 for next, etc
got_ignition     | Bool  | 1 if this flight computer fires its own motor 
got_separation   | Bool  | 1 if this flight computer fires separation charges (can include igniting next stage)
got_drogue       | Bool  | 1 if this flight computer fires a drogue parachute
got_main         | Bool  | 1 if this flight computer fires a main parachute
accel_axis       | Int   | Which accelerometer axis is 'up', 1=x 2=y 3=z 4=-x 5=-y 6=-z
pyro_firetime    | Int   | Time in milliseconds to fire pyros
pyro_1           | Int   | Pyro channel 1 usage: 0=unused 1=ignition 2=separation 3=drogue 4=main
pyro_2           | Int   | Pyro channel 2 usage: 0=unused 1=ignition 2=separation 3=drogue 4=main
pyro_3           | Int   | Pyro channel 3 usage: 0=unused 1=ignition 2=separation 3=drogue 4=main
pyro_4           | Int   | Pyro channel 4 usage: 0=unused 1=ignition 2=separation 3=drogue 4=main
ignition_accel   | Float | Acceleration threshold for detecting motor ignition, in m/s/s
burnout_time     | Float | Time limit on motor burn duration, in seconds
ignite_altitude  | Float | Altitude above which to fire this stage's motor, in metres ASL
ignite_time      | Float | Time since burnout at which to fire this stage's motor, in seconds
ignite_timeout   | Float | Time to spend trying to ignite before giving up, in seconds
apogee_time      | Float | Time since burnout after which apogee is considered to have been reached, in seconds
main_altitude    | Float | Altitude below which to release the main parachute, in metres ASL
main_time        | Float | Time since apogee at which to release the main parachute, in seconds
landing_time     | Float | Time since apogee at which to consider the rocket landed, in seconds
use_radio        | Bool  | 1 to use the radio, 0 to disable
use_magno        | Bool  | 1 to use the magnetometer, 0 to disable
use_gyro         | Bool  | 1 to use the gyroscope, 0 to disable
use_gps          | Bool  | 1 to use the GPS, 0 to disable
