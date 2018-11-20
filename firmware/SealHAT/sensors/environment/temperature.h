/*
 * IncFile1.h
 *
 * Created: 17-Nov-18 15:03:04
 *  Author: Ethan
 */


#ifndef INCFILE1_H_
#define INCFILE1_H_

class si705x {
    // private copy constructor, and assignment operator
    si705x(const si705x &a);
    si705x& operator=(const si705x &a);

    public:
        si705x();
        ~si705x() {};

        int32_t read(uint16_t* val) = 0;
        int32_t

    protected:
        void i2c_init( bool(*f)(uint8_t, const void*, uint8_t, void*, uint8_t), const uint8_t I2C_ADDRESS ) {
            i2c_transfer = f;
            i2c_address  = I2C_ADDRESS;
        }

        int8_t writeReg8(const uint8_t regNo, const uint8_t val) {
            uint8_t buf[2] = {regNo, val};
            return i2c_transfer(i2c_address, buf, sizeof(buf), 0 , 0);
        }

        uint8_t readReg8(const uint8_t regNo) {
            uint8_t buf[2] = {regNo, 0x00};
            i2c_transfer( i2c_address, &buf[0], 1, &buf[1], 1 );
            return buf[1];
        }

        // function to make an I2C transaction: I2C address, pointer to tx buffer, tx size, pointer to rx buffer, rx size
        bool(*i2c_transfer)(uint8_t, void const*, uint8_t, void*, uint8_t);

        uint8_t  i2c_address;       // I2C Address
        uint16_t lastVal;           // last value read from the sensor
    };


#endif /* INCFILE1_H_ */