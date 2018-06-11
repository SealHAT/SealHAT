#ifndef SEALHAT_DEVICE_H
#define SEALHAT_DEVICE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QQueue>
#include <QByteArray>

#include <stdint.h>
#include "seal_Types.h"

class SealHAT_device : public QObject
{
    Q_OBJECT
public:
    // constructor
    explicit SealHAT_device(QObject *parent = nullptr);
    // destructor
    ~SealHAT_device();

    // connect to the SealHAT device
    bool connectToDevice(QString portName);
    // enqueue data to send
    int sendData(QByteArray data);

signals:
    // signal emitted to indicate the device has connected
    void sealhat_connected();
    // signal emitted to indicate the device has disconnected
    void sealhat_disconnected();
    // signal emitted with data from the device
    void data_in(QByteArray data);

public slots:
    // disconnect from the device
    void disconnectFromDevice();

private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
     QSerialPort     sealhat;       // serial object for connecting to device
     QTimer          pollTimer;     // timer to poll for incoming data
     QQueue<quint8>  out_data;      // queue to send data to device
     QByteArray      in_data;       // Data from device before processing
};

#endif // SEALHAT_DEVICE_H
