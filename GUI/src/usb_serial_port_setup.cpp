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

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
            ui->RXstream_serialPort_comboBox->addItem(serialPortInfo.portName());
    }
}
