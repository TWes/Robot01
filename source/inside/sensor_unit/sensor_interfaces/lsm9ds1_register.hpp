#ifndef LSM9DS1_REGISTER_HPP
#define LSM9DS1_REGISTER_HPP

#define MAGNETOMETER_ADDRESS 0x1e

#define CTRL_REG1_M 0x20
#define TEMP_COMP 7
#define OM 5
#define DO 2
#define FAST_ODR 1
#define ST 0

#define CTRL_REG2_M 0x21
#define FS 5
#define REBOOT 3
#define SOFT_RST 2

#define CTRL_REG3_M 0x22
#define I2C_DISABLE 7
#define LP 5
#define SIM 2
#define MD 0

#define CTRL_REG4_M 0x23
#define OMZ 2
#define BLE 1

#define CTRL_REG5_M 0x24
#define FAST_READ 7
#define BDU 6

#define STATUS_REG_M 0x27
#define ZYXOR 7
#define ZOR 6
#define YOR 5
#define XOR 4
#define ZYXDA 3
#define ZDA 2
#define YDA 1
#define XDA 0




#endif // LSM9DS1_REGISTER_HPP