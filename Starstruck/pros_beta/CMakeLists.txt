cmake_minimum_required(VERSION 3.6)
project(pros_beta)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    firmware/cortex.ld
    firmware/libpros.a
    firmware/STM32F10x.ld
    firmware/uniflash.jar
    include/2616E.h
    include/API.h
    include/main.h
    include/ports.h
    src/2616E_lib/autonswitch.c
    src/2616E_lib/drive.c
    src/2616E_lib/lcd.c
    src/auto.c
    src/init.c
    src/opcontrol.c
    common.mk
    Makefile
    project.pros)

add_executable(pros_beta ${SOURCE_FILES})