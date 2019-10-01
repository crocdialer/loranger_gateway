#ifndef _NODE_TYPES_H
#define _NODE_TYPES_H

#include <stdint.h>

#define STRUCT_TYPE_SMART_BULB 0x69

typedef struct smart_bulb_t
{
    uint8_t stype = STRUCT_TYPE_SMART_BULB;
    uint8_t light_sensor = 0;
    uint8_t acceleration = 0;
    uint8_t leds_enabled = 0;
    uint8_t battery = 0;
} smart_bulb_t;

#define STRUCT_TYPE_ELEVATOR_CONTROL 0x13

typedef struct elevator_t
{
    uint8_t stype = STRUCT_TYPE_ELEVATOR_CONTROL;
    uint8_t button = 0;
    uint8_t touch_status = 0;
    uint8_t velocity = 0;
    uint8_t intensity = 0;
} elevator_t;

#endif
