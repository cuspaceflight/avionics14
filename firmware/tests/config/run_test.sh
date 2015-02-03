#!/bin/sh

# This script simply copies in the config source files and tests them against
# a mocked up SD card access layer (that actually uses the local file system).
#
# I haven't figured out if there's a way to compile the files with a direct
# reference to config.c in the source directory, such that it will include the
# microsd header from in here instead of the microsd header in its own dir.

cp ../../config.c config.c
cp ../../config.h config.h

gcc -std=c99 -Wall --pedantic microsd.c config.c config_test.c
./a.out
rm a.out
rm config.c config.h
