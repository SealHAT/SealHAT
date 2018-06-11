#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QDataStream>
#include "maindialog.h"
#include "ui_maindialog.h"

/**************************************************************
 * FUNCTION: on_startStream_button_clicked
 * ------------------------------------------------------------
 *  This function gets called whenever the "START STREAMING" or
 *  "STOP STREAMING" button is clicked. It will set serial port
 *  user selected to be ReadOnly mode and start or stop
 *  streaming the sampling data GUI gets from microcontroller.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_startStream_button_clicked()
{
    dataBuffer.clear();

    if(ui->startStream_button->text() == "START STREAMING") {
        if(ui->RXstream_serialPort_comboBox->count()) {
            if(device.connectToDevice(ui->RXstream_serialPort_comboBox->currentText())) {

                ui->startStream_button->setText("STOP STREAMING");
                device.sendData("so");
                connect(&device, &SealHAT_device::data_in, this, &maindialog::serialReceived);
            }
        }
        else {
            qDebug() << "No ports available!";
        }
    }else{
        device.disconnectFromDevice();
        ui->startStream_button->setText("START STREAMING");
    }
}

// Slot to receive data from the serial device asyncronously
void maindialog::serialReceived(QByteArray data)
{
    dataBuffer.append(data);

    recognizeData_fromBuffer();

    headerAnalyze_display();

    if(streamOut.isOpen()) {
        streamOut.write(data);
        qDebug() << "size of the stream output file is "<< streamOut.size();
    }
}

/**************************************************************
 * FUNCTION: on_captureDatatoFile_button_clicked
 * ------------------------------------------------------------
 *  This function gets called whenever the "Capture to File"
 *  button is clicked. It will open a new file and store the data
 *  the same time data streaming from microcontroller.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_captureDatatoFile_button_clicked()
{

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a file to save data to"),
                                                    QDir::homePath(),
                                                    tr("CSV (*.csv);;Data File (*.dat);;Text (*.txt);;All (*.*)")
                                                    );

    streamOut.setFileName(filename);
    if(!streamOut.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening stream output file!";
    }
}

void maindialog::on_backButton_StreamPage_clicked()
{
    if(streamOut.isOpen()) {
        streamOut.flush();
        streamOut.close();
    }
    device.disconnectFromDevice();
    on_backButton_clicked();
}

void maindialog::on_backButton_retrieveData_clicked()
{
    on_backButton_clicked();
}

void maindialog::on_getDataButton_clicked()
{
    QString path = ui->storeData_destinationEdit->text();
    QDir dir;

    if(!dir.exists(path)){
        dir.mkpath(path);
        QMessageBox::information(this,tr("created new directory"), path);
    }

    QFile file( path +"/config.out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
           return;
    qDebug()<<"file now exists";

       QTextStream out(&file);
       //file.close();

}
