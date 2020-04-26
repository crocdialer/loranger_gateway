#ifndef _NODE_TYPES_H
#define _NODE_TYPES_H

#include <stdint.h>

#define STRUCT_TYPE_SMART_BULB 0x69
#define STRUCT_TYPE_ELEVATOR_CONTROL 0x13
#define STRUCT_TYPE_WEATHERMAN 0x77

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

typedef struct weather_t
{
    uint8_t stype = STRUCT_TYPE_WEATHERMAN;

    uint8_t battery = 0;

    //! temperature in rang [-100C .. 100C]
    uint16_t temperature = 0;

    //! pressure in hPa (mbar)
    uint16_t pressure = 0;

    // relative humidity in range [0..1]
    uint8_t humidity = 0;

} weather_t;
#endif
