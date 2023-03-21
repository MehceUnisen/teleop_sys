#include "g29_driver.h"

int main(){
    G29Driver driver;
    driver.initDevice(std::string("/dev/input/event18"));
    while(true) {
        driver.readData();
    }
	return 0;
}
