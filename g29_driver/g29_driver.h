#pragma once

#include <iostream>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

struct G29Messages {
    uint16_t m_gas;
    uint16_t m_brake;
    uint16_t m_clutch;

    uint16_t m_steeringAngle;
};

//struct G29InputCodes {
//    uint8_t STEERING_CODE = 0x00;
//    uint8_t GAS_CODE = 0X02;
//    uint8_t BRAKE_CODE = 0x05;
//    uint8_t CLUTCH_CODE = 0X01;
//    uint16_t GEAR_UP_CODE = 0x124;
//    uint16_t GEAR_DOWN_CODE = 0x125;
//
//};

class G29Driver {

    G29Messages g29Messages;

    int testBit(int bit, unsigned char *array);

    int m_device_handle;
    int m_axis_code = ABS_X;
    std::string m_device_name;
    struct ff_effect m_effect;
    struct input_event m_event;

public:
    int initDevice(std::string device_name);
    int readData();

};
