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
    if(ui->startStream_button->text() == "START STREAMING"){


        ui->startStream_button->setText("STOP STREAMING");

        receiveSerial_samples();


    }else{
        closeSerialPort();
        ui->startStream_button->setText("START STREAMING");
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

    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open Directory"),
                "C:/Users/hpan5/Downloads/gui",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );

    acc_file.setFileName( dir +"/accelerometer.txt");
    mag_file.setFileName( dir +"/magnetometer.txt");
    ekg_file.setFileName( dir +"/electrocardiogram.txt");
    temp_file.setFileName( dir +"/temperature.txt");
    light_file.setFileName( dir +"/light.txt");
    gps_file.setFileName( dir +"/gps.txt");

    if (!acc_file.open(QIODevice::Append | QIODevice::Text))
           return;
    //qDebug()<<"acc_file now exists";

    if (!mag_file.open(QIODevice::Append | QIODevice::Text))
           return;
    //qDebug()<<"mag_file now exists";

    if (!ekg_file.open(QIODevice::Append | QIODevice::Text))
           return;
    //qDebug()<<"ekg_file now exists";

    if (!temp_file.open(QIODevice::Append | QIODevice::Text))
           return;
    //qDebug()<<"temp_file now exists";

    if (!light_file.open(QIODevice::Append | QIODevice::Text))
           return;
    //qDebug()<<"light_file now exists";

    if (!gps_file.open(QIODevice::Append | QIODevice::Text))
           return;
}




void maindialog::dataFiles_Setup(){


    //qDebug()<<"gps_file now exists";

       QTextStream acc_out(&acc_file);
       QTextStream mag_out(&mag_file);
       QTextStream ekg_out(&ekg_file);
       QTextStream gps_out(&gps_file);
       QTextStream temp_out(&temp_file);
       QTextStream light_out(&light_file);

       acc_out << acc_DataBuffer;
       mag_out << mag_DataBuffer;
       ekg_out << ekg_DataBuffer;
       gps_out << gps_DataBuffer;
       temp_out << temp_DataBuffer;
       light_out << light_DataBuffer;

       qDebug() << "size of acc file is "<< acc_file.size();
}

void maindialog::on_backButton_retrieveData_clicked()
{
    on_backButton_clicked();
}




void maindialog::on_storeData_destinationEdit_returnPressed()
{
    QString path = ui->storeData_destinationEdit->text();
     QDir newDir;
     if(!newDir.exists(path)){
         QMessageBox::information(this,tr("created new directory"), path);
         newDir.mkpath(path);
         qDebug()<<"Created new directory";
     }
}

void maindialog::on_chooseDestButton_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open Directory"),
                "C:/Users/hpan5/Downloads/gui",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );

    ui->storeData_destinationEdit->setText(dir);
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

/*QString fileName = QFileDialog::getSaveFileName(this, tr("Save Destination Address"),
                                                "C:/Users/hpan5/Downloads/gui",
                                                tr("Text files (*.txt);;XML files (*.xml)"));
if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_5);
        //out << config;
        //file.close();
}*/
