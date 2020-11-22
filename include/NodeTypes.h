#ifndef _NODE_TYPES_H
#define _NODE_TYPES_H

#include <stdint.h>

#define STRUCT_TYPE_EMPTY_DEVICE 0x00
#define STRUCT_TYPE_SMART_BULB 0x69
#define STRUCT_TYPE_ELEVATOR_CONTROL 0x13
#define STRUCT_TYPE_TEMPERATUREMAN 0x76
#define STRUCT_TYPE_WEATHERMAN 0x77
#define STRUCT_TYPE_GASMAN 0xF0
#define STRUCT_TYPE_TRACKERMAN 0x66

typedef struct empty_device_t
{
    uint8_t stype = STRUCT_TYPE_EMPTY_DEVICE;
    uint8_t battery = 0;
} empty_device_t;

typedef struct smart_bulb_t
{
    uint8_t stype = STRUCT_TYPE_SMART_BULB;
    uint8_t light_sensor = 0;
    uint8_t acceleration = 0;
    uint8_t leds_enabled = 0;
    uint8_t battery = 0;
} smart_bulb_t;

typedef struct elevator_t
{
    uint8_t stype = STRUCT_TYPE_ELEVATOR_CONTROL;
    uint8_t button = 0;
    uint8_t touch_status = 0;
    uint8_t velocity = 0;
    uint8_t intensity = 0;
} elevator_t;

typedef struct temperature_t
{
    uint8_t stype = STRUCT_TYPE_TEMPERATUREMAN;

    uint8_t battery = 0;

    //! temperature in range [-50C .. 100C]
    uint16_t temperature = 0;
} temperature_t;

typedef struct weather_t
{
    uint8_t stype = STRUCT_TYPE_WEATHERMAN;

    uint8_t battery = 0;

    //! temperature in range [-50C .. 100C]
    uint16_t temperature = 0;

    //! pressure in range [500hPa .. 1500hPa]
    uint16_t pressure = 0;

    // relative humidity in range [0..1]
    uint8_t humidity = 0;

} weather_t;

typedef struct gasman_t
{
    uint8_t stype = STRUCT_TYPE_GASMAN;

    // battery in range [0 .. 1]
    uint8_t battery = 0;

    // eCO2 (equivalent calculated carbon-dioxide) concentration in parts per million (ppm)
    uint16_t eco2 = 0;

    // TVOC (total volatile organic compound) concentration in parts per billion (ppb)
    uint16_t tvoc = 0;

} gasman_t;

typedef struct trackerman_t
{
    uint8_t stype = STRUCT_TYPE_TRACKERMAN;

    //! battery [0..1]
    uint8_t battery = 0;

    //! fixed point latitude in decimal degrees. divide by 10000000.0 to get a double
    int32_t latitude_fixed = 0;

    //! fixed point longitude in decimal degrees. divide by 10000000.0 to get a double
    int32_t longitude_fixed = 0;

    //! number of satellites, 0 -> no fix
    uint8_t num_satellites = 0;

} trackerman_t;

#endif
