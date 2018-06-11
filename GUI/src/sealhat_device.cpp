#include "sealhat_device.h"
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
    QList<QSerialPortInfo> portList; // list of ports on the computer
    bool retval = false;

    portList = QSerialPortInfo::availablePorts();

    sealhat.setPortName(portName);
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
    }
    else {
        qDebug() << "Error opening " << portName;
    }

    return retval;
}

void SealHAT_device::disconnectFromDevice()
{
    if(sealhat.isOpen()) {
        if(sealhat.disconnect()) {
            qDebug() << "Disconnected from device";
            emit(sealhat_disconnected());
        }
        else {
            qDebug() << "Failed to disconnect from device!";
        }
    }

    pollTimer.stop();
    in_data.clear();
}

int SealHAT_device::sendData(QByteArray data)
{
    sealhat.write(data);
}

void SealHAT_device::deserializeDataPackets()
{
    quint16       startSym, size;   // size and startSym from data header
    quint8        devID, seqNum;    // device ID and sequence number from header
    quint32       timestamp;        // timestamp from header

    int index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
    while(index >= 0) {
        // remove any partial packets (should only happen in first read of a stream)
        clean_data.remove(0, index);

        // Attach the data stream to the clean_data array. Set to little endian.
        QDataStream stream(&clean_data, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream >> startSym >> devID >> seqNum >> timestamp >> size;

        parseSensorPackets(stream, device, timestamp, seqNum, length);

        clean_data.remove(0, (size + sizeof(DATA_HEADER_t)));
        index = clean_data.indexOf(QByteArray(MSG_START_SYM_STR));
    }

}

void SealHAT_device::parseSensorPackets(QDataStream& stream, quint8 device, quint32 time, quint8 seqNum, quint16 length)
{
    switch(devID) {
        case DEVICE_ID_ENVIRONMENTAL  :
        case DEVICE_ID_LIGHT          : value = QString::number(data.light, 'f');
                break;
        case DEVICE_ID_TEMPERATURE    : value = QString::number(data.temperature, 'f');
                break;
        case DEVICE_ID_ACCELEROMETER  : value = QString("%1,%2,%3").arg(data.acceleration.xAxis, 0, 'f', 6).arg(data.acceleration.yAxis, 0, 'f', 6).arg(data.acceleration.zAxis, 0, 'f', 6);
                break;
        case DEVICE_ID_MAGNETIC_FIELD : value = QString("%1,%2,%3").arg(data.magnetic.xAxis, 0, 'f', 6).arg(data.magnetic.yAxis, 0, 'f', 6).arg(data.magnetic.zAxis, 0, 'f', 6);
                break;
        case DEVICE_ID_GYROSCOPE      : value = QString("Err: no gyro code yet");
                break;
        case DEVICE_ID_PRESSURE       : value = QString("Err: no pressure code yet");
                break;
        case DEVICE_ID_DEPTH          : value = QString("Err: no depth code yet");
                break;
        case DEVICE_ID_GPS            : value = QString("Err: no GPS code yet");
                break;
        case DEVICE_ID_EKG            : value = QString("Err: no EKG code yet");
                break;
        case DEVICE_ID_SYSTEM         : value = QString("Err: no SYS code yet");
                break;
        default : value = QString("Err: invalid sensor (%1) with packet size %2").arg(devID).arg(size);
    };

}

void SealHAT_device::parseEnvironmental(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{
    const float FULL_ACCURACY_CONSTANT = 0.045;
    const float LOW_ACCURACY_CONSTANT  = 0.720;
    const float TEMP_STEP_SIZE         = 175.72;
    const float TEMP_ZERO_OFFSET       = 46.85;
    const int   TEMP_RANGE             = 65536;
    const int   MS_PERIOD   = 1000;
    const int   SAMPLE_SIZE = sizeof(uint16_t);

    QDateTime     timestamp;
    int           sampleCount = length / (SAMPLE_SIZE*2);
    quint8        luxMantissa, luxExponent;
    quint16       tempRAW;
    SENSOR_DATA_t valueSI;

    // set the time, then subract the period * number of sample for time of first sample in buffer
    timestamp.setSecsSinceEpoch(time);
    timestamp.addMSecs((-MS_PERIOD)*sampleCount);

    while(sampleCount > 0) {
        stream >> luxExponent >> luxMantissa;
        valueSI.light = (1<<luxExponent) * (float)luxMantissa * FULL_ACCURACY_CONSTANT;
        data_q.enqueue(SensorSample(DEVICE_ID_LIGHT, timestamp, seqNum, valueSI);

        stream >> tempRAW;
        valueSI.temperature = ((TEMP_STEP_SIZE*tempRAW) / TEMP_RANGE) - TEMP_ZERO_OFFSET;
        data_q.enqueue(SensorSample(DEVICE_ID_TEMPERATURE, timestamp, seqNum, valueSI);

        sampleCount--;
        timestamp.addMSecs(MS_PERIOD);
    }
}

void parse_elsm303agr(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{

}

void parse_samm8q(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{

}

void parse_max30003(QDataStream& stream, quint32 time, quint8 seqNum, quint16 length)
{

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
        qDebug() << in_data.data();

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

    pollTimer.start(5000);
    this->deserializeDataPackets();
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
