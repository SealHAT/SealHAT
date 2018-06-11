#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QtWidgets>
#include <QByteArray>
#include <QDataStream>
#include "maindialog.h"
#include "ui_maindialog.h"

/*Cast sensor configuration structures to QByteArray*/

void maindialog::sendSerial_Config(){
    //send_serialSetup();
    QByteArray configData;
    configData = config_serialize();

    const qint64 bytesWritten = microSerial->write(configData);
    qDebug() << "number of bytes sending" <<bytesWritten << endl;

    if (bytesWritten == -1)
    {
        qDebug() <<"Failed to write the data to port" << endl;
        serial_retry = true;
    } else if (bytesWritten != configData.size()) {
        qDebug() <<"Failed to write all the data to port" << endl;
        serial_retry = true;
    } else if (!microSerial->waitForBytesWritten(5000)) {
        qDebug() <<"Operation timed out or an error "
                   "occurred, error:"<< microSerial->errorString()<< endl;
    }else{
        qDebug() <<"Data successfully sent to port"<< endl;
    }

    if(microSerial->isOpen()){
        microSerial->close();
    }

}

QByteArray maindialog::config_serialize(){
    QByteArray byteArray;

    QDataStream stream(&byteArray, QSerialPort::ReadWrite); //QIODevice:WriteOnly
    stream.setVersion(QDataStream::Qt_4_5);

    stream << configuration_settings;

    return byteArray;
}

QDataStream& operator<<(QDataStream& stream, const SENSOR_CONFIGS& configs) {

    stream << (uint8_t*)&configs;
    return stream;
}


/*foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
{
    if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
    {
        if(serialPortInfo.vendorIdentifier() == microSerial_vendor_id)
        {
            if(serialPortInfo.productIdentifier() == microSerial_product_id)
            {
                //microSerial_port_name = serialPortInfo.portName();
                microSerial_port_name = ui->serialPort_comboBox->currentText();
                microSerial_is_available = true;
            }
        }
    }
}*/
