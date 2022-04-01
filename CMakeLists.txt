# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(SODAQ_challenge)

target_sources(app PRIVATE src/main.c src/control_task.c src/leds.c src/btns.c)
include_directories(include)