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
                device.startStream();
                connect(&device, &SealHAT_device::samplesReady, this, &maindialog::samplesReceived_stream);
            }
        }
        else {
            qDebug() << "No ports available!";
        }
    }else{
        device.stopStream();
        disconnect(&device, &SealHAT_device::samplesReady, this, &maindialog::samplesReceived_stream);
        device.disconnectFromDevice();
        ui->startStream_button->setText("START STREAMING");
    }
}

void maindialog::writeToStreamBox(QTextEdit* textBox, QString str)
{
    if(textBox->toPlainText().size() > 1000) {
        textBox->clear();
    }

    textBox->moveCursor(QTextCursor::End);
    textBox->insertPlainText(str);
}

// Slot to receive data from the serial device asyncronously
void maindialog::samplesReceived_stream(QQueue<SensorSample>* q)
{
    while(!q->empty()) {
        SensorSample sample = q->first();

        switch(sample.get_type()) {
            case DEVICE_ID_LIGHT          : writeToStreamBox(ui->light_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_TEMPERATURE    : writeToStreamBox(ui->temp_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_ACCELEROMETER  : writeToStreamBox(ui->xcel_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_MAGNETIC_FIELD : writeToStreamBox(ui->mag_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_GPS            : writeToStreamBox(ui->gps_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_EKG            : writeToStreamBox(ui->ekg_streamText, sample.get_csv()+"\n"); break;
            case DEVICE_ID_GYROSCOPE      : qDebug() << "Err: no gyro sensor available"; break;
            case DEVICE_ID_PRESSURE       : qDebug() << "Err: no pressure sensor available"; break;
            case DEVICE_ID_DEPTH          : qDebug() << "Err: no depth sensor available"; break;
            case DEVICE_ID_SYSTEM         : qDebug() << "Err: no SYS code yet"; break;
            default : qDebug() << QString("Err: invalid sensor (%1)").arg(sample.get_type());
        };

        if(streamOut.isOpen()) {
            streamOut.write((sample.get_csv() + "\n").toUtf8());
            streamOut.flush();
        }

        q->removeFirst();
    }
}

void maindialog::on_captureDatatoFile_button_clicked()
{

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a file to save data to"),
                                                    QDir::currentPath(),
                                                    tr("All (*.*);;CSV (*.csv);;Data File (*.dat);;Text (*.txt)")
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
