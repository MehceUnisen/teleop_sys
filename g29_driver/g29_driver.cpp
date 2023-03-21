#include "g29_driver.h"

int G29Driver::initDevice(std::string device_name) {
    m_device_name = device_name;
//    unsigned char key_bits[1+KEY_MAX/8/sizeof(unsigned char)];
    unsigned char abs_bits[1+ABS_MAX/8/sizeof(unsigned char)];
    unsigned char ff_bits[1+FF_MAX/8/sizeof(unsigned char)];
    struct input_event event;
    struct input_absinfo abs_info;

    m_device_handle = open(m_device_name.c_str(), O_RDWR|O_NONBLOCK);
    if (m_device_handle < 0) {
        std::cout << "ERROR: cannot open device : "<< m_device_name << std::endl;
        exit(1);

    } else {std::cout << "device opened" << std::endl;}

    // which axes has the device?
    std::memset(abs_bits, 0, sizeof(abs_bits));
    if (ioctl(m_device_handle, EVIOCGBIT(EV_ABS, sizeof(abs_bits)), abs_bits) < 0) {
        std::cout << "ERROR: cannot get abs bits" << std::endl;
        exit(1);
    }

    // get some information about force feedback
    std::memset(ff_bits, 0, sizeof(ff_bits));
    if (ioctl(m_device_handle, EVIOCGBIT(EV_FF, sizeof(ff_bits)), ff_bits) < 0) {
        std::cout << "ERROR: cannot get ff bits" << std::endl;
        exit(1);
    }

    // get axis value range
    if (ioctl(m_device_handle, EVIOCGABS(m_axis_code), &abs_info) < 0) {
        std::cout << "ERROR: cannot get axis range" << std::endl;
        exit(1);
    }
//    m_axis_max = abs_info.maximum;
//    m_axis_min = abs_info.minimum;
//    if (m_axis_min >= m_axis_max) {
//        std::cout << "ERROR: axis range has bad value" << std::endl;
//        exit(1);
//    }

    // check force feedback is supported?
    if(!testBit(FF_CONSTANT, ff_bits)) {
        std::cout << "ERROR: force feedback is not supported" << std::endl;
        exit(1);

    } else { std::cout << "force feedback supported" << std::endl; }

    // auto centering off
    memset(&event, 0, sizeof(event));
    event.type = EV_FF;
    event.code = FF_AUTOCENTER;
    event.value = 0;
    if (write(m_device_handle, &event, sizeof(event)) != sizeof(event)) {
        std::cout << "failed to disable auto centering" << std::endl;
        exit(1);
    }

    // init effect and get effect id
    memset(&m_effect, 0, sizeof(m_effect));
    m_effect.type = FF_CONSTANT;
    m_effect.id = -1; // initial value
    m_effect.trigger.button = 0;
    m_effect.trigger.interval = 0;
    m_effect.replay.length = 0xffff;  // longest value
    m_effect.replay.delay = 0; // delay from write(...)
    m_effect.u.constant.level = 0;
    m_effect.direction = 0xC000;
    m_effect.u.constant.envelope.attack_length = 0;
    m_effect.u.constant.envelope.attack_level = 0;
    m_effect.u.constant.envelope.fade_length = 0;
    m_effect.u.constant.envelope.fade_level = 0;

    if (ioctl(m_device_handle, EVIOCSFF, &m_effect) < 0) {
        std::cout << "failed to upload m_effect" << std::endl;
        exit(1);
    }

    // start m_effect
    memset(&event, 0, sizeof(event));
    event.type = EV_FF;
    event.code = m_effect.id;
    event.value = 1;
    if (write(m_device_handle, &event, sizeof(event)) != sizeof(event)) {
        std::cout << "failed to start event" << std::endl;
        exit(1);
    }
    return 0;
}

int G29Driver::testBit(int bit, unsigned char *array) {
    return ((array[bit / (sizeof(unsigned char) * 8)] >> (bit % (sizeof(unsigned char) * 8))) & 1);
}

int G29Driver::readData() {
    while(read(m_device_handle, &m_event, sizeof(m_event)) == sizeof(m_event)) {
        switch (m_event.code) {
            case 0x00: // steering
                if(m_event.type == EV_ABS)
                    g29Messages.m_steeringAngle = m_event.value;
                break;
            case 0x02: //gas
//                std::cout << "sevgilim bana gaz basiyor ," <<m_event.type << "\n";
                g29Messages.m_gas = m_event.value;
                break;
            case 0x05: //brake
//                std::cout << "hocam aykut uc yiyo\n";
                g29Messages.m_brake = m_event.value;
                break;
            case 0x01: // clutch
//                std::cout << "masallah hocam insallah hocam\n";
                g29Messages.m_clutch = m_event.value;
                break;
            case 0x124: //gear up
//                std::cout << "bu alemin krali da biziz madarasi da\n";
                break;
            case 0x125: //gear down
//                std::cout << "eyvallah abi eyvallah, buyuk adamsin vesselam\n";
                break;
        }
    }
    std::cout << "steering angle => " << g29Messages.m_steeringAngle << "\n";
    std::cout << "gas amount => " << g29Messages.m_gas << "\n";
    std::cout << "brake amount =>" << g29Messages.m_brake << "\n";
    std::cout << "clutch amount =>" << g29Messages.m_clutch << "\n";
    std::cout << "\n=================================================\n";
    usleep(500000);

    return 0;
}


