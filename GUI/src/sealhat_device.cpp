#include "sealhat_device.h"
#include <QDir>
#include <QDebug>
#include <stdint.h>

SealHAT_device::SealHAT_device(QObject *parent) : QObject(parent)
{
    connect(&sealhat, &QSerialPort::errorOccurred, this, &SealHAT_device::SerialError_cb);
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
        sealhat.setDataTerminalReady(false);
        sealhat.setRequestToSend(false);

        pollTimer.start(5000);
        retval = true;
        emit(sealhat_connected());

        rawDataLog.setFileName(QString("rawOutput.txt"));
        rawDataLog.open(QIODevice::Truncate | QIODevice::WriteOnly);

        preCRCfile.setFileName(QString("preCRCOutput.txt"));
        preCRCfile.open(QIODevice::Truncate | QIODevice::WriteOnly);
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
    preCRCfile.close();
    pollTimer.stop();
    in_data.clear();
}

int SealHAT_device::sendData(QByteArray data)
{
    return sealhat.write(data);
}

bool SealHAT_device::startStream()
{
    const char streamCmd = STREAM_DATA;
    QByteArray data;            // for reading out temporary data
    bool       retval;          // return value

    // set default return value
    retval = false;

    // get the current device configuration, so data is interpreted correctly
    getConfig();

    sealhat.setDataTerminalReady(true);
    if(sealhat.waitForReadyRead(800)) {
        data += sealhat.readAll();
        if(data.contains(USB_TEXT_ADVENTURE_MENU)) {
            qDebug() << "received menu, starting stream";
            sealhat.write(&streamCmd);
            connect(&pollTimer, &QTimer::timeout, this, &SealHAT_device::SerialTimerTimout_cb);
            connect(&sealhat, &QSerialPort::readyRead, this, &SealHAT_device::StreamingReadyRead_cb);
            retval = true;
        }
        else {
            qDebug() << "No Menu Received";
        }
    }
    else {
        qDebug() << "No data received from " << sealhat.portName();
    }

    return retval;
}

void SealHAT_device::stopStream() {
    const char streamCmd = STREAM_DATA;

    if(sealhat.isOpen()) {
        sealhat.write(&streamCmd);
        disconnect(&pollTimer, &QTimer::timeout, this, &SealHAT_device::SerialTimerTimout_cb);
        disconnect(&sealhat, &QSerialPort::readyRead, this, &SealHAT_device::StreamingReadyRead_cb);
    }
    in_data.clear();
    clean_data.clear();
    sealhat.clear();
    sealhat.setDataTerminalReady(false);
}

bool SealHAT_device::download()
{
    const char downloadCmd = DOWNLOAD_DATA;
    bool       retval;     // return value

    // set default return value
    retval = false;

    // get the current device configuration, so data is interpreted correctly
    getConfig();

    sealhat.setDataTerminalReady(true);
    if(sealhat.waitForReadyRead(800)) {
        data += sealhat.readAll();
        if(data.contains(USB_TEXT_ADVENTURE_MENU)) {
            qDebug() << "received menu, starting download";
            sealhat.write(&downloadCmd);
            connect(&pollTimer, &QTimer::timeout, this, &SealHAT_device::SerialTimerTimout_cb);
            connect(&sealhat, &QSerialPort::readyRead, this, &SealHAT_device::StreamingReadyRead_cb);
            retval = true;
        }
        else {
            qDebug() << "No Menu Received";
        }
    }
    else {
        qDebug() << "No data received from " << sealhat.portName();
    }

    return retval;
}

bool SealHAT_device::getConfig()
{
    const char      streamCmd = VERIFY_CONFIG;
    QByteArray      data;       // for reading bytes out of serial
    SYSTEM_CONFIG_t config;     // hold the device config packet once received
    bool            retval;     // return value

    // set default return value
    retval = false;

    sealhat.setDataTerminalReady(true);
    if(sealhat.waitForReadyRead(800)) {
        data += sealhat.readAll();
        if(data.contains(USB_TEXT_ADVENTURE_MENU)) {
            qDebug() << "received menu, getting config";

            // clear data and send the verify settings command
            data.clear();
            sealhat.write(&streamCmd);
            sealhat.waitForReadyRead(1000);
            data = sealhat.readAll();

            // Attach the data stream to the clean_data array. Set to little endian.
            QDataStream stream(&data, QIODevice::ReadOnly);

            stream >> config;

            // TODO CRC32 check of the data

            if((config.header.startSym == MSG_START_SYM) && (config.header.id == DEVICE_ID_CONFIG)) {
                this->devCfg = config.sensorConfigs;
            }

            retval = true;
        }
        else {
            qDebug() << "No Menu Received";
        }
    }
    else {
        qDebug() << "No data received from " << sealhat.portName();
    }

    sealhat.setDataTerminalReady(true);
    sealhat.clear();

    return retval;
}

bool SealHAT_device::sendConfig()
{
    const char configCmd = CONFIGURE_DEV;
    bool            retval;     // return value

    // set default return value
    retval = false;

    return retval;
}

void SealHAT_device::deserializeDataPackets()
{
    DATA_HEADER_t tmpHeader;        // temporary header to read stream into

    int index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
    while((index >= 0) && ((unsigned int)clean_data.size() > sizeof(DATA_HEADER_t))) {
        // remove any partial packets (should only happen in first read of a stream)
        clean_data.remove(0, index);

        // Attach the data stream to the clean_data array and grab the header
        QDataStream stream(&clean_data, QIODevice::ReadOnly);
        stream >> tmpHeader;

        // ONLY IF the entire packet is in the clean_data queu do we continue to parse and remove it
        if((unsigned int)clean_data.size() > (sizeof(DATA_HEADER_t) + tmpHeader.size)) {

            switch(tmpHeader.id) {
                case DEVICE_ID_LIGHT          : parse_max44009(stream, tmpHeader); break;
                case DEVICE_ID_TEMPERATURE    : parse_si7051(stream, tmpHeader); break;
                case DEVICE_ID_ACCELEROMETER  : parse_lsm303agr_acc(stream, tmpHeader); break;
                case DEVICE_ID_MAGNETIC_FIELD : parse_lsm303agr_mag(stream, tmpHeader); break;
                case DEVICE_ID_GPS            : parse_samm8q(stream, tmpHeader); break;
                case DEVICE_ID_EKG            : parse_max30003(stream, tmpHeader); break;
                case DEVICE_ID_GYROSCOPE      : qDebug() << "Err: no gyro sensor available"; break;
                case DEVICE_ID_PRESSURE       : qDebug() << "Err: no pressure sensor available"; break;
                case DEVICE_ID_DEPTH          : qDebug() << "Err: no depth sensor available"; break;
                case DEVICE_ID_SYSTEM         : qDebug() << "Err: no SYS code yet"; break;
                default : qDebug() << QString("Err: invalid sensor (%1) with packet size %2").arg(tmpHeader.id).arg(tmpHeader.size);
            };

            clean_data.remove(0, (tmpHeader.size + sizeof(DATA_HEADER_t)));
            index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
            if(index > int(tmpHeader.size + sizeof(DATA_HEADER_t))) {
                qDebug() << "warning: index of next header not immediatly after the last! index:" << index << "  arrSize:" << clean_data.size();
            }
        }
        else {
            qDebug() << "incomplete packet, leaving till next time";
            index = -1;
        }
    }

}

/***********************************************************************************************/
/***********************  HARDWARE SPECIFIC DATA PARSERS ***************************************/
/***********************************************************************************************/

void SealHAT_device::parse_si7051(QDataStream& stream, DATA_HEADER_t header)
{
    const float TEMP_STEP_SIZE   = 175.72;
    const float TEMP_ZERO_OFFSET = 46.85;
    const int   TEMP_RANGE       = 65536;
    const int   SAMPLE_SIZE      = sizeof(uint16_t);

    QDateTime     timestamp;
    int           msPeriod;
    int           sampleCount = header.size / SAMPLE_SIZE;
    quint16       tempRAW;
    SENSOR_DATA_t valueSI;

    msPeriod = devCfg.envConfig.period * 1000;

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(header.timestamp);
    timestamp = timestamp.addMSecs((-msPeriod)*sampleCount);

    while(sampleCount > 0) {
        stream >> tempRAW;
        valueSI.temperature = ((TEMP_STEP_SIZE*tempRAW) / TEMP_RANGE) - TEMP_ZERO_OFFSET;
        data_q.enqueue(SensorSample(DEVICE_ID_TEMPERATURE, timestamp, header.packetCount, valueSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(msPeriod);
    }
}

void SealHAT_device::parse_max44009(QDataStream& stream, DATA_HEADER_t header)
{
    const float FULL_ACCURACY_CONSTANT = 0.045;
    const int   MS_PERIOD              = 1000;
    const int   SAMPLE_SIZE            = sizeof(uint16_t);

    QDateTime     timestamp;
    int           msPeriod;
    int           sampleCount = header.size / SAMPLE_SIZE;
    quint8        luxMantissa, luxExponent;
    SENSOR_DATA_t valueSI;

    msPeriod = devCfg.envConfig.period * 1000;

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(header.timestamp);
    timestamp = timestamp.addMSecs((-MS_PERIOD)*sampleCount);

    while(sampleCount > 0) {
        stream >> luxMantissa >> luxExponent;
        valueSI.light = (1<<luxExponent) * (float)luxMantissa * FULL_ACCURACY_CONSTANT;
        data_q.enqueue(SensorSample(DEVICE_ID_LIGHT, timestamp, header.packetCount, valueSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(MS_PERIOD);
    }
}

void SealHAT_device::parse_lsm303agr_acc(QDataStream& stream, DATA_HEADER_t header)
{
    // different Scales, there are 3 modes and 4 full scale settings. these are mg/LSB values from data sheet page 13. these are rounded ints from the STM API
    static const uint8_t   intScale[3][4] = {{1,   2,  4,  12}, {4,   8, 16,  48}, {16, 32, 64, 192}};
    const int              SAMPLE_SIZE   = sizeof(AxesRaw_t);

    QDateTime        timestamp;
    ACC_OPMODE_t     accMode;
    ACC_FULL_SCALE_t accScale;
    unsigned int     i, j, shift, msPeriod;
    int              sampleCount = header.size / SAMPLE_SIZE;
    qint16           accX, accY, accZ;
    SENSOR_DATA_t    valSI;

    accMode  = devCfg.accConfig.opMode;
    accScale = devCfg.accConfig.scale;

    switch(accMode & ACC_POWER_MODE_MASK) {
        case ACC_NORMAL_POWER : i = 1; shift = 6; break;
        case ACC_LOW_POWER : i = 2; shift = 8; break;
        case ACC_HIGH_RESOLUTION :
        default :           i = 0; shift = 4;
    };

    switch(accMode & ACC_RATE_MASK) {
        case ACC_1_HZ   : msPeriod = 1000; break;
        case ACC_10_HZ  : msPeriod = 100; break;
        case ACC_25_HZ  : msPeriod = 40; break;
        case ACC_50_HZ  : msPeriod = 20; break;
        case ACC_100_HZ : msPeriod = 10; break;
        case ACC_200_HZ : msPeriod = 5; break;
        case ACC_400_HZ : msPeriod = 2; break;
        default    : msPeriod = 20;
    };

    switch(accScale) {
        case ACC_SCALE_4G:  j = 1; break;
        case ACC_SCALE_8G:  j = 2; break;
        case ACC_SCALE_16G: j = 3; break;
        case ACC_SCALE_2G:
        default:         j = 0;
    };

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(header.timestamp);
    timestamp = timestamp.addMSecs((-msPeriod)*sampleCount);

    while(sampleCount > 0) {
        stream >> accX >> accY >> accZ;
        valSI.acceleration.xAxis = ( (accX >> shift) * intScale[i][j] / 1000.0);
        valSI.acceleration.yAxis = ( (accY >> shift) * intScale[i][j] / 1000.0);
        valSI.acceleration.zAxis = ( (accZ >> shift) * intScale[i][j] / 1000.0);
        data_q.enqueue(SensorSample(DEVICE_ID_ACCELEROMETER, timestamp, header.packetCount, valSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(msPeriod);
    }
}

void SealHAT_device::parse_lsm303agr_mag(QDataStream& stream, DATA_HEADER_t header)
{
    const float        MAG_CONST   = 1.5;
    const int          SAMPLE_SIZE = sizeof(AxesRaw_t);

    QDateTime     timestamp;
    MAG_OPMODE_t  magRate;
    unsigned int  msPeriod;
    int           sampleCount = header.size / SAMPLE_SIZE;
    qint16        magX, magY, magZ;
    SENSOR_DATA_t valSI;

    magRate = devCfg.magConfig.opMode;

    switch(magRate & MAG_RATE_MASK) {
        case MAG_10_HZ  : msPeriod = 100; break;
        case MAG_20_HZ  : msPeriod = 50; break;
        case MAG_50_HZ  : msPeriod = 20; break;
        case MAG_100_HZ : msPeriod = 10; break;
        default: msPeriod = 50;
    };

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(header.timestamp);
    timestamp = timestamp.addMSecs((-msPeriod)*sampleCount);

    while(sampleCount > 0) {
        stream >> magX >> magY >> magZ;
        valSI.magnetic.xAxis = ((magX * MAG_CONST) / 1000.0);
        valSI.magnetic.yAxis = ((magY * MAG_CONST) / 1000.0);
        valSI.magnetic.zAxis = ((magZ * MAG_CONST) / 1000.0);
        data_q.enqueue(SensorSample(DEVICE_ID_MAGNETIC_FIELD, timestamp, header.packetCount, valSI));

        sampleCount--;
        timestamp = timestamp.addMSecs(msPeriod);
    }
}

void SealHAT_device::parse_samm8q(QDataStream& stream, DATA_HEADER_t header)
{
    qDebug() << "GPS??? What GPS??????";
}

void SealHAT_device::parse_max30003(QDataStream& stream, DATA_HEADER_t header)
{
    qDebug() << "EKG??? What EKG??????";
}

/***********************************************************************************************/
/***********************  SIGNAL HANDLERS FOR SERIAL PORT / USB DEVICE  *** ********************/
/***********************************************************************************************/

void SealHAT_device::StreamingReadyRead_cb()
{
    static const int dataAndCRC = (PAGE_SIZE_LESS + sizeof(int32_t));

    // write data to file before CRC, mainly for debug
    QByteArray dataFromDevice = sealhat.readAll();
    preCRCfile.write(dataFromDevice);
    preCRCfile.flush();

    in_data.append(dataFromDevice);

    if(in_data.contains(USB_TEXT_ADVENTURE_MENU)) {
        qDebug() << "recieved menu: " << in_data.data();
        in_data.clear();
    }
    else {
        int index = in_data.indexOf(QByteArray(USB_PACKET_START_SYM_STR));
        if(index >= 0) {
            in_data.remove(0, index);
        }
        else {
            qDebug() << "Pattern Not Found: \"CAFED00D\"";
        }

        if(in_data.size() >= dataAndCRC) {
            QDataStream stream(&in_data, QIODevice::ReadOnly);
            DATA_TRANSMISSION_t usbPacket;
            quint32 crcCalculated;

            stream >> usbPacket;

//            crc32.initInstance(1);
//            crc32.pushData(1, (char*)usbPacket.data, PAGE_SIZE_LESS);
//            crcCalculated = crc32.releaseInstance(1);

            // append to the clean buffer if header and CRC match
            if((usbPacket.startSymbol == USB_PACKET_START_SYM)) {
                clean_data.append((const char*)usbPacket.data, PAGE_SIZE_LESS);
            }
            else {
                qDebug() << "packet failed integrity check";
            }

            in_data.remove(0, sizeof(DATA_TRANSMISSION_t));
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

void SealHAT_device::SerialTimerTimout_cb()
{
    qDebug() << "No data from port " << sealhat.portName()
             << " For 5 seconds...";
}

void SealHAT_device::SerialError_cb(QSerialPort::SerialPortError serialPortError)
{
    if (!serialPortError) {
        qDebug() << "Serial Port Error: " << sealhat.errorString();
    }
}

/***********************************************************************************************/
/***********************  OVERLOADED STREAM INSERTION OPERATORS FOR STRUCTS ********************/
/***********************************************************************************************/

QDataStream& operator>>(QDataStream& stream, DATA_HEADER_t& header) {
    quint32 time;
    quint16 startSymbol, packetSize;
    quint8  devID, seqNum;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> startSymbol >> devID >> seqNum >> time >> packetSize;

    header.startSym    = (uint16_t)startSymbol;
    header.id          = (uint8_t)devID;
    header.packetCount = (uint8_t)seqNum;
    header.timestamp   = (uint32_t)time;
    header.size        = (uint16_t)packetSize;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, ACC_CFG_t& accCfg)
{
    quint32 tmpActiveHours;
    quint8  accScale, accMode, accSense;
    quint16 accThresh, accDuration;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tmpActiveHours >> accScale >> accMode >> accThresh >> accDuration >> accSense;

    accCfg.activeHour  = (uint32_t)tmpActiveHours;
    accCfg.scale       = (ACC_FULL_SCALE_t)accScale;
    accCfg.opMode      = (ACC_OPMODE_t)accMode;
    accCfg.threshold   = (int16_t)accThresh;
    accCfg.duration    = (int16_t)accDuration;
    accCfg.sensitivity = (uint8_t)accSense;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, MAG_CFG_t& magCfg)
{
    quint32 tmpActiveHours;
    quint8  magMode;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tmpActiveHours >> magMode;

    magCfg.activeHour = (uint32_t)tmpActiveHours;
    magCfg.opMode     = (MAG_OPMODE_t)magMode;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, ENV_CFG_t& envCfg)
{
    quint32 tmpActiveHours;
    quint16 envPeriod;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tmpActiveHours >> envPeriod;

    envCfg.activeHour = (uint32_t)tmpActiveHours;
    envCfg.period     = (uint16_t)envPeriod;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, GPS_CFG_t& gpsCfg)
{
    quint32 tmpActiveHours, gpsActiveRate, gpsIdleRate;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tmpActiveHours >> gpsActiveRate >> gpsIdleRate;

    gpsCfg.activeHour = (uint32_t)tmpActiveHours;
    gpsCfg.activeRate = (uint32_t)gpsActiveRate;
    gpsCfg.idleRate   = (uint32_t)gpsIdleRate;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, MOD_CFG_t& modCfg)
{
    quint32 tmpActiveHours;
    quint8  ekgRate, ekgGain, ekgFreq;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tmpActiveHours >> ekgRate >> ekgGain >> ekgFreq;

    modCfg.activeHour = (uint32_t)tmpActiveHours;
    modCfg.rate       = (ECG_SAMPLE_RATE_t)ekgRate;
    modCfg.gain       = (ECG_GAIN_t)ekgGain;
    modCfg.freq       = (ECG_LOW_PASS_t)ekgFreq;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, SENSOR_CONFIGS_t& sensorCfg)
{
    quint8    chipCount, startDay, startMonth, startHour;
    quint16   startYear;
    ACC_CFG_t tmpAcc;
    MAG_CFG_t tmpMag;
    ENV_CFG_t tmpEnv;
    GPS_CFG_t tmpGPS;
    MOD_CFG_t tmpEKG;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> chipCount >> startDay >> startMonth >> startYear >> startHour
           >> tmpAcc >> tmpMag >> tmpEnv >> tmpGPS >> tmpEKG;

    sensorCfg.num_flash_chips = (uint8_t)chipCount;
    sensorCfg.start_day       = (uint8_t)startDay;
    sensorCfg.start_month     = (uint8_t)startMonth;
    sensorCfg.start_year      = (uint16_t)startYear;
    sensorCfg.start_hour      = (uint8_t)startHour;
    sensorCfg.accConfig       = tmpAcc;
    sensorCfg.magConfig       = tmpMag;
    sensorCfg.envConfig       = tmpEnv;
    sensorCfg.gpsConfig       = tmpGPS;
    sensorCfg.ekgConfig       = tmpEKG;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, SYSTEM_CONFIG_t& sysCfg)
{
    DATA_HEADER_t    tempHeader;
    SENSOR_CONFIGS_t tempSensorCfg;
    quint32          tempCRC32;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> tempHeader >> tempSensorCfg >> tempCRC32;

    sysCfg.header        = tempHeader;
    sysCfg.sensorConfigs = tempSensorCfg;
    sysCfg.crc32         = (uint32_t)tempCRC32;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, DATA_TRANSMISSION_t& txData) {
    quint32 temp_startSymbol;
    quint32 temp_crc;
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> temp_startSymbol;
    txData.startSymbol = temp_startSymbol;

    for(int i=0;i<PAGE_SIZE_LESS; i++){
        stream >> txData.data[i];
    }

    stream >> temp_crc;
    txData.crc = (uint32_t)temp_crc;

    return stream;
}
