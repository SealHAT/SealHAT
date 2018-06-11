#include "sealhat_device.h"
#include <QCoreApplication>
#include <QByteArrayMatcher>

SealHAT_device::SealHAT_device(QObject *parent) : QObject(parent)
{

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

        connect(&sealhat, &QSerialPort::readyRead, this, &SealHAT_device::handleReadyRead);
        connect(&sealhat, &QSerialPort::errorOccurred, this, &SealHAT_device::handleError);
        connect(&pollTimer, &QTimer::timeout, this, &SealHAT_device::handleTimeout);

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
}

int SealHAT_device::sendData(QByteArray data)
{
    sealhat.write(data);
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
        QByteArrayMatcher usbPackerHeader(QByteArray("\xCA\xFE\xD0\x0D"));

        int index = usbPackerHeader.indexIn(in_data);
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
            emit(data_in(tempArray));
            in_data.remove(0, dataAndCRC);
        }
        else {
            qDebug() << "Not a full USB packet, saving data for next round.";
        }
    }

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
