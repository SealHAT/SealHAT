#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QList>
#include <QMessageBox>
#include <QDataStream>
#include "maindialog.h"
#include "ui_maindialog.h"

/**************************************************************
 * FUNCTION: send_serialSetup
 * ------------------------------------------------------------
 *  This function checks what serial port users selected
 *  in the TX_serialPort_comboBox. Set serial port to write only.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::send_serialSetup()
{
    microSerial_is_available = false;
    microSerial_port_name = "";
    serialDataBuffer = "";

    //microSerial = new QSerialPort(this);


    microSerial_port_name = ui->TX_serialPort_comboBox->currentText();

    if(ui->RXstream_serialPort_comboBox->count() != 0){
        microSerial_is_available = true;
    }else{
        microSerial_is_available = false;
    }

    if(microSerial_is_available)
    {
        //open and configure the port
        microSerial->setPortName(microSerial_port_name);
        microSerial->open(QSerialPort::ReadWrite);  //Set serial port to write only
        microSerial->setBaudRate(QSerialPort::Baud9600);
        microSerial->setDataBits(QSerialPort::Data8);
        microSerial->setParity(QSerialPort::NoParity);
        microSerial->setDataTerminalReady(true);

        microSerial->setStopBits(QSerialPort::OneStop);
        microSerial->setFlowControl(QSerialPort::NoFlowControl);

        qDebug() << "Found Serial Port:  " << microSerial_port_name;

    }else{
        QMessageBox::warning(this, "Port error", "Could not find the Microcontroller Serial Port!");
    }

}


/**************************************************************
 * FUNCTION: receive_serialSetup
 * ------------------------------------------------------------
 *  This function checks what serial port users selected
 *  in the RX_serialPort_comboBox. Set serial port to write only.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::receive_serialSetup()
{
    microSerial_is_available = false;
    microSerial_port_name = "";
    serialDataBuffer = "";

    //microSerial = new QSerialPort(this);


    microSerial_port_name = ui->RXstream_serialPort_comboBox->currentText();

    if(ui->RXstream_serialPort_comboBox->count() != 0){
        microSerial_is_available = true;
    }else{
        microSerial_is_available = false;
    }

    if(microSerial_is_available)
    {
        //open and configure the port
        microSerial->setPortName(microSerial_port_name);
        microSerial->open(QSerialPort::ReadWrite);  //Set serial port to ReadOnly
        microSerial->setBaudRate(QSerialPort::Baud9600);
        microSerial->setDataBits(QSerialPort::Data8);
        microSerial->setParity(QSerialPort::NoParity);
        microSerial->setDataTerminalReady(true);

        microSerial->setStopBits(QSerialPort::OneStop);
        microSerial->setFlowControl(QSerialPort::NoFlowControl);

        qDebug() << "Found Serial Port:  " << microSerial_port_name;

    }else{
        QMessageBox::warning(this, "Port error", "Could not find the Microcontroller Serial Port!");
    }
}


void maindialog::closeSerialPort()
{
    if (microSerial->isOpen())
        microSerial->close();

    qDebug() << "SeiralPort is closed!" ;
}

/**************************************************************
 * FUNCTION: on_TX_ReScanButton_clicked
 * ------------------------------------------------------------
 *  This function checks what serial port users selected
 *  in the TX_serialPort_comboBox. Set serial port to Read only.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_TX_ReScanButton_clicked()
{
    ui->TX_serialPort_comboBox->clear();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
            ui->TX_serialPort_comboBox->addItem(serialPortInfo.portName());
        }
}

/**************************************************************
 * FUNCTION: on_RX_ReScanButton_clicked
 * ------------------------------------------------------------
 *  This function checks what serial port users selected
 *  in the RX_serialPort_comboBox. Set serial port to Read only.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_RXstream_ReScanButton_clicked()
{
    ui->RXstream_serialPort_comboBox->clear();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
            ui->RXstream_serialPort_comboBox->addItem(serialPortInfo.portName());

        }
}
