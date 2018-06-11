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

    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open Directory"),
                "C:/Users/hpan5/Downloads/gui",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );

    ui->storeData_destinationEdit->setText(dir);
}
