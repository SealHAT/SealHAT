#include "sealhat_device.h"
#include <QDir>
#include <QDebug>
#include <stdint.h>

SealHAT_device::SealHAT_device(QObject *parent) : QObject(parent)
{
    connect(&sealhat, &QSerialPort::readyRead, this, &SealHAT_device::handleReadyRead);
    connect(&sealhat, &QSerialPort::errorOccurred, this, &SealHAT_device::handleError);
    connect(&pollTimer, &QTimer::timeout, this, &SealHAT_device::handleTimeout);
}

SealHAT_device::~SealHAT_device()
{
    if(sealhat.isOpen()) {
        this->disconnectFromDevice();
    }
}

bool SealHAT_device::connectToDevice(QString portName)
{
    bool retval = false;
    QSerialPortInfo portInfo;

    // TODO: Gross. figure out how to pass in the port info, stored as QVariant in dropdown list
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if(serialPortInfo.portName() == portName) {
            portInfo = serialPortInfo;
        }
    }

    sealhat.setPort(portInfo);
    if(sealhat.open(QSerialPort::ReadWrite) ) {
        sealhat.setBaudRate(QSerialPort::Baud115200);
        sealhat.setDataBits(QSerialPort::Data8);
        sealhat.setParity(QSerialPort::NoParity);
        sealhat.setStopBits(QSerialPort::OneStop);
        sealhat.setDataTerminalReady(true);
        sealhat.setRequestToSend(true);

        pollTimer.start(5000);
        retval = true;
        emit(sealhat_connected());

        rawDataLog.setFileName(QString("rawOutput.txt"));
        rawDataLog.open(QIODevice::Truncate | QIODevice::WriteOnly);
    }
    else {
        qDebug() << "Error opening " << portName << " : " << sealhat.errorString();
    }

    return retval;
}

void SealHAT_device::disconnectFromDevice()
{
    if(sealhat.isOpen()) {
        sealhat.close();
        emit(sealhat_disconnected());
    }

    rawDataLog.close();
    pollTimer.stop();
    in_data.clear();
}

int SealHAT_device::sendData(QByteArray data)
{
    return sealhat.write(data);
}

void SealHAT_device::deserializeDataPackets()
{
    quint16       startSym, size;   // size and startSym from data header
    quint8        devID, seqNum;    // device ID and sequence number from header
    quint32       timestamp;        // timestamp from header

    int index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
    while((index >= 0) && ((unsigned int)clean_data.size() > sizeof(DATA_HEADER_t))) {
        // remove any partial packets (should only happen in first read of a stream)
        clean_data.remove(0, index);

        // Attach the data stream to the clean_data array. Set to little endian.
        QDataStream stream(&clean_data, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream >> startSym >> devID >> seqNum >> timestamp >> size;

        if((unsigned int)clean_data.size() > (sizeof(DATA_HEADER_t) + size)) {
            parseSensorPackets(stream, devID, timestamp, seqNum, size);

            clean_data.remove(0, (size + sizeof(DATA_HEADER_t)));
            index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
        }
        else {
            qDebug() << "incomplete packet, leaving till next time";
            index = -1;
        }
    }

}

void SealHAT_device::parseSensorPackets(QDataStream& stream, quint8 device, quint32 time, quint8 seqNum, quint16 length)
{
    switch(device) {
        case DEVICE_ID_LIGHT          : parse_max44009(stream, time, seqNum, length); break;
        case DEVICE_ID_TEMPERATURE    : parse_si7051(stream, time, seqNum, length); break;
        case DEVICE_ID_ACCELEROMETER  : parse_lsm303agr_acc(stream, time, seqNum, length); break;
        case DEVICE_ID_MAGNETIC_FIELD : parse_lsm303agr_mag(stream, time, seqNum, length); break;
        case DEVICE_ID_GPS            : parse_samm8q(stream, time, seqNum, length); break;
        case DEVICE_ID_EKG            : parse_max30003(stream, time, seqNum, length); break;
        case DEVICE_ID_GYROSCOPE      : qDebug() << "Err: no gyro sensor available"; break;
        case DEVICE_ID_PRESSURE       : qDebug() << "Err: no pressure sensor available"; break;
        case DEVICE_ID_DEPTH          : qDebug() << "Err: no depth sensor available"; break;
        case DEVICE_ID_SYSTEM         : qDebug() << "Err: no SYS code yet"; break;
        default : qDebug() << QString("Err: invalid sensor (%1) with packet size %2").arg(device).arg(length);
    };

}

void SealHAT_device::parse_si7051(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    const float TEMP_STEP_SIZE   = 175.72;
    const float TEMP_ZERO_OFFSET = 46.85;
    const int   TEMP_RANGE       = 65536;
    const int   MS_PERIOD        = 1000;
    const int   SAMPLE_SIZE      = sizeof(uint16_t);

    QDateTime     timestamp;
    int           sampleCount = length / SAMPLE_SIZE;
    quint16       tempRAW;
    SENSOR_DATA_t valueSI;

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(time);
    timestamp = timestamp.addMSecs((-MS_PERIOD)*sampleCount);

    while(sampleCount > 0) {
        stream >> tempRAW;
        valueSI.temperature = ((TEMP_STEP_SIZE*tempRAW) / TEMP_RANGE) - TEMP_ZERO_OFFSET;
        data_q.enqueue(SensorSample(DEVICE_ID_TEMPERATURE, timestamp, seqNum, valueSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(MS_PERIOD);
    }
}

void SealHAT_device::parse_max44009(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    const float FULL_ACCURACY_CONSTANT = 0.045;
    const int   MS_PERIOD              = 1000;
    const int   SAMPLE_SIZE            = sizeof(uint16_t);

    QDateTime     timestamp;
    int           sampleCount = length / SAMPLE_SIZE;
    quint8        luxMantissa, luxExponent;
    SENSOR_DATA_t valueSI;

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(time);
    timestamp = timestamp.addMSecs((-MS_PERIOD)*sampleCount);

    while(sampleCount > 0) {
        stream >> luxMantissa >> luxExponent;
        valueSI.light = (1<<luxExponent) * (float)luxMantissa * FULL_ACCURACY_CONSTANT;
        data_q.enqueue(SensorSample(DEVICE_ID_LIGHT, timestamp, seqNum, valueSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(MS_PERIOD);
    }
}

void SealHAT_device::parse_lsm303agr_acc(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    // different Scales, there are 3 modes and 4 full scale settings. these are mg/LSB values from data sheet page 13. these are rounded ints from the STM API
    static const uint8_t   intScale[3][4] = {{1,   2,  4,  12}, {4,   8, 16,  48}, {16, 32, 64, 192}};
    const ACC_OPMODE_t     CURRENT_MODE  = ACC_HR_50_HZ;
    const ACC_FULL_SCALE_t CURRENT_SCALE = ACC_SCALE_2G;
    const int              SAMPLE_SIZE   = sizeof(AxesRaw_t);

    QDateTime     timestamp;
    unsigned int  i, j, shift, msPeriod;
    int           sampleCount = length / SAMPLE_SIZE;
    qint16        accX, accY, accZ;
    SENSOR_DATA_t valSI;

    switch(CURRENT_MODE & ACC_POWER_MODE_MASK) {
        case ACC_NORMAL_POWER : i = 1; shift = 6; break;
        case ACC_LOW_POWER : i = 2; shift = 8; break;
        case ACC_HIGH_RESOLUTION :
        default :           i = 0; shift = 4;
    };

    switch(CURRENT_MODE & ACC_RATE_MASK) {
        case ACC_1_HZ   : msPeriod = 1000; break;
        case ACC_10_HZ  : msPeriod = 100; break;
        case ACC_25_HZ  : msPeriod = 40; break;
        case ACC_50_HZ  : msPeriod = 20; break;
        case ACC_100_HZ : msPeriod = 10; break;
        case ACC_200_HZ : msPeriod = 5; break;
        case ACC_400_HZ : msPeriod = 2; break;
        default    : msPeriod = 20;
    };

    switch(CURRENT_SCALE) {
        case ACC_SCALE_4G:  j = 1; break;
        case ACC_SCALE_8G:  j = 2; break;
        case ACC_SCALE_16G: j = 3; break;
        case ACC_SCALE_2G:
        default:         j = 0;
    };

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(time);
    timestamp = timestamp.addMSecs((-msPeriod)*sampleCount);

    while(sampleCount > 0) {
        stream >> accX >> accY >> accZ;
        valSI.acceleration.xAxis = ( (accX >> shift) * intScale[i][j] / 1000.0);
        valSI.acceleration.yAxis = ( (accY >> shift) * intScale[i][j] / 1000.0);
        valSI.acceleration.zAxis = ( (accZ >> shift) * intScale[i][j] / 1000.0);
        data_q.enqueue(SensorSample(DEVICE_ID_ACCELEROMETER, timestamp, seqNum, valSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(msPeriod);
    }
}

void SealHAT_device::parse_lsm303agr_mag(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    const MAG_OPMODE_t MAG_RATE    = MAG_LP_50_HZ;
    const float        MAG_CONST   = 1.5;
    const int          SAMPLE_SIZE = sizeof(AxesRaw_t);

    QDateTime     timestamp;
    unsigned int  msPeriod;
    int           sampleCount = length / SAMPLE_SIZE;
    qint16        magX, magY, magZ;
    SENSOR_DATA_t valSI;

    switch(MAG_RATE & MAG_RATE_MASK) {
        case MAG_10_HZ  : msPeriod = 100; break;
        case MAG_20_HZ  : msPeriod = 50; break;
        case MAG_50_HZ  : msPeriod = 20; break;
        case MAG_100_HZ : msPeriod = 10; break;
        default: msPeriod = 50;
    };

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(time);
    timestamp = timestamp.addMSecs((-msPeriod)*sampleCount);

    while(sampleCount > 0) {
        stream >> magX >> magY >> magZ;
        valSI.magnetic.xAxis = ((magX * MAG_CONST) / 1000.0);
        valSI.magnetic.yAxis = ((magY * MAG_CONST) / 1000.0);
        valSI.magnetic.zAxis = ((magZ * MAG_CONST) / 1000.0);
        data_q.enqueue(SensorSample(DEVICE_ID_MAGNETIC_FIELD, timestamp, seqNum, valSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(msPeriod);
    }
}

void SealHAT_device::parse_samm8q(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    qDebug() << "GPS??? What GPS??????";
}

void SealHAT_device::parse_max30003(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    qDebug() << "EKG??? What EKG??????";
}

void SealHAT_device::handleReadyRead()
{
    static const int dataAndCRC = (PAGE_SIZE_LESS + sizeof(int32_t));
    in_data.append(sealhat.readAll());

    if(in_data.contains("(c)onfigure, (r)etrieve data, (s)tream data")
            || in_data.contains("stream (o)nly, (l)og to flash only, (b)oth") ) {
        qDebug() << "recieved menu: " << in_data.data();
        in_data.clear();
    }
    else {
        int index = in_data.indexOf(QByteArray(USB_PACKET_START_SYM_STR));
        if(index >= 0) {
            in_data.remove(0, index+4);
        }
        else {
            qDebug() << "Pattern Not Found: \"CAFED00D\"";
        }

        if(in_data.size() >= dataAndCRC) {
            QByteArray tempArray = in_data;
            tempArray.chop(tempArray.size()-dataAndCRC);

            // TODO: perform CRC32, Qt doesnt have an implementation??
            tempArray.chop(4);

            // send the verefied data to the clean array, and chop it off the in_data
            clean_data.append(tempArray);
            in_data.remove(0, dataAndCRC);
        }
        else {
            qDebug() << "Not a full USB packet, saving data for next round.";
        }
    }

    // write raw data to the logging file
    rawDataLog.write(clean_data.data(), clean_data.size());
    rawDataLog.flush();

    // call the deserialize function
    this->deserializeDataPackets();
    emit(samplesReady(&data_q));
    pollTimer.start(5000);
}

void SealHAT_device::handleTimeout()
{
    qDebug() << "No data from port " << sealhat.portName()
             << " For 5 seconds...";
}

void SealHAT_device::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (!serialPortError) {
        qDebug() << "Serial Port Error: " << sealhat.errorString();
    }
}

QDataStream& operator>>(QDataStream& stream, DATA_HEADER_t& data_header) {
    quint32 temp_timestamp;
    quint16 temp_startSym, temp_id, temp_msTime, temp_size;

    stream >> temp_startSym
           >> temp_id
           >> temp_timestamp
           >> temp_msTime
           >> temp_size;

    data_header.startSym  = (uint16_t)temp_startSym;
    data_header.id        = (uint16_t)temp_id;
    data_header.timestamp = (uint32_t)temp_timestamp;
    //data_header.msTime    = (uint16_t)temp_msTime;
    data_header.size    = (uint16_t)temp_size;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, DATA_TRANSMISSION_t& txData) {
    quint32 temp_startSymbol;
    quint32 temp_crc;

    stream >> temp_startSymbol;

    for(int i=0;i<PAGE_SIZE_EXTRA; i++){
        stream >> txData.data[i];
    }

    stream >> temp_crc;

    txData.startSymbol = (uint32_t)temp_startSymbol;
    txData.crc = (uint32_t)temp_crc;

    return stream;
}
