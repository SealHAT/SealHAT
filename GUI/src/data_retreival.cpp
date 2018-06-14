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
    QString filename = QFileDialog::getSaveFileName(this,
                                            tr("Download Device Data"),
                                            QDir::currentPath(),
                                            tr("CSV (*.csv);;Data File (*.dat);;Text (*.txt);;All (*.*)")
                                            );

    streamOut.setFileName(filename);

    ui->storeData_destinationEdit->setText(filename);

    //TODO: only set enabled to true if path is valid and COM port is valid too.
    ui->getDataButton->setEnabled(true);
    ui->getDataButton->setStyleSheet("background-color:rgb(253, 199, 0)");
}
