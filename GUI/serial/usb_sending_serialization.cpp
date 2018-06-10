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
    send_serialSetup();
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





QDataStream& operator<<(QDataStream& stream, const DATA_HEADER_t& header) {

    stream << header.startSym
           << header.id
           << (quint32)header.timestamp
           << (quint16)header.msTime
           << (quint16)header.size;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const Xcel_TX& xcel) {

    stream << xcel.acc_headerData
           << (quint32)xcel.acc_activeHour
           << xcel.acc_scale
           << xcel.acc_mode
           << (quint8)xcel.acc_sensitivity
           << (quint16)xcel.acc_sensitivity;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const Mag_TX& mag) {

    stream << mag.mag_headerData
           << (quint32)mag.mag_activeHour
           << mag.mag_mode;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const Temp_TX& temp) {

    stream << temp.temp_headerData
           << (quint32)temp.temp_activeHour
           << (quint32)temp.temp_samplePeriod;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const EKG_TX& ekg) {

    stream << ekg.ekg_headerData
           << (quint32)ekg.ekg_activeHour
           << ekg.ekg_sampleRate
           << ekg.ekg_gain
           << ekg.ekg_lowpassFreq;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const GPS_TX& gps) {

    stream << gps.gps_headerData
           << (quint32)gps.gps_activeHour
           << gps.default_profile;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const calendar_date& calender) {

    stream << (quint8)calender.day
           << (quint8)calender.month
           << (quint8)calender.year;

    return stream;
}

QDataStream& operator<<(QDataStream& stream, const SENSOR_CONFIGS& configs) {

    stream << configs.config_header
           << (quint8)configs.num_flash_chips
           << configs.start_logging_day
           << (quint32)configs.start_logging_time
           << configs.accelerometer_config
           << configs.magnetometer_config
           << configs.temperature_config
           << configs.ekg_config
           << configs.gps_config;

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
