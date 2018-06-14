#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include <QPalette>
#include <cmath>
#include "maindialog.h"
#include "seal_Types.h"
#include "ui_maindialog.h"

/**************************************************************
 * FUNCTION: powerEstimation
 * ------------------------------------------------------------
 *  This function caculated number of active hours,
 *  number of samples per hours when active for each sensor and
 *  pass them to both storage and power caculation functions.
 *  Both powerEstimation() and storageEstimation() get called in
 *  this function.
 *
 *  It will get called whenever each configuration
 *  setting that could affect power or storage got changed.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::generalEstimation(){
    QPalette warning_palette;
    warning_palette.setColor(QPalette::WindowText, Qt::red);
    double StorageConsump = guiConfig.getEstimatedMemoryUse();
    QString storageconsumpString = " " + QString::number(StorageConsump,'f',2) + " % ";


    if(StorageConsump > (90.0)){
        warning_palette.setColor(QPalette::Text,Qt::red);
    }else{
        warning_palette.setColor(QPalette::Text,Qt::black);
    }

    ui->storageConsumption_Text->setAutoFillBackground(true);
    ui->storageConsumption_Text->setPalette(warning_palette);
    ui->storageConsumption_Text->setText(storageconsumpString);

    powerEst = guiConfig.getPowerUse();
    on_batterySizeText_editingFinished();
}

/**************************************************************
 * FUNCTION: on_batterySizeText_editingFinished
 * ------------------------------------------------------------
 *  This function gets called whenever user finish editing the
 *  size of the battery capacity they are going to use.
 *  This function will check estimated power value(in mA)
 *  we got from powerEstimation() function and caculated power
 *  consumption of the battery user choose to used.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_batterySizeText_editingFinished()
{
    //QString powerEstString = (QString::number(powerEst,'f',5));
    double batterySize = (ui->batterySizeText->text().toDouble())*(0.85);
    uint16_t timeDuration = batterySize/(powerEst);
    uint16_t monthConsump = timeDuration/30;
    uint16_t dayConsump = timeDuration%30;
    QString powerconsumpString;
    if(monthConsump){
        powerconsumpString = " " + (QString::number(monthConsump)) + " Months "
                                    + (QString::number(dayConsump)) + " Days ";
    }else{
        powerconsumpString = " " + (QString::number(dayConsump)) + " Days ";
    }


    if(!(ui->batterySizeText->text().isEmpty()))
    {
        ui->pwrConsumption_Text->setText(powerconsumpString);
    }else{
        ui->pwrConsumption_Text->clear();
    }
}
