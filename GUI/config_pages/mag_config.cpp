#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

void maindialog::mag_getloadData(){
    for(QPushButton* button : ui->magConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            shift_property = button->property("button_shift").toInt();
            bit_Mask = (0x01 << shift_property);
            if((guiConfig.getMagConfig().activeHour&bit_Mask))
            {
                button->setProperty("clicked", true);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");

            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }

    uint8_t mag_freqSelect = (guiConfig.getMagConfig().opMode%16)/4;
    uint8_t mag_pwrSelect = (guiConfig.getMagConfig().opMode/16)%10;

    ui->mag_pwrBox->setCurrentIndex(mag_pwrSelect);
    ui->mag_freqBox->setCurrentIndex(mag_freqSelect);

}

void maindialog::mag_checkTimetoEnable(){
    if(guiConfig.getMagConfig().activeHour){
        IMUmag_Disable(false);
    }else{
        IMUmag_Disable(true);
    }
}

void maindialog::mag_estimation_control()
{
    for(QComboBox* box : ui->magConfigPage->findChildren<QComboBox*>())
    {
        connect(box,SIGNAL(currentIndexChanged(int)), this, SLOT(generalEstimation()));
    }
    for(QPushButton* button : ui->magConfigPage->findChildren<QPushButton*>())
    {
        connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
        connect(button,SIGNAL(clicked()), this, SLOT(mag_checkTimetoEnable()));
    }
}

void maindialog::mag_dataCollect()
{
    uint8_t pwrIndex = 0;
    uint8_t freq;
    MAG_CFG_t temp = guiConfig.getMagConfig();

    pwrIndex= ui->mag_pwrBox->currentIndex();
    freq = ui->mag_freqBox->currentIndex();

    if(pwrIndex == MAG_LP)
    {
        switch(freq){
        case MAG_FREQ_10HZ:
            temp.opMode = MAG_LP_10_HZ;
        break;
        case MAG_FREQ_20HZ:
            temp.opMode = MAG_LP_20_HZ;
        break;
        case MAG_FREQ_50HZ:
            temp.opMode = MAG_LP_50_HZ;
        break;
        case MAG_FREQ_100HZ:
            temp.opMode = MAG_LP_100_HZ;
        break;
        }

    }else if(pwrIndex == MAG_NORMAL){
            switch(freq){
            case MAG_FREQ_10HZ:
                temp.opMode = MAG_NORM_10_HZ;
            break;
            case MAG_FREQ_20HZ:
                temp.opMode = MAG_NORM_20_HZ;
            break;
            case MAG_FREQ_50HZ:
                temp.opMode = MAG_NORM_50_HZ;
            break;
            case MAG_FREQ_100HZ:
                temp.opMode = MAG_NORM_100_HZ;
            break;
            }
    }
    guiConfig.setMagnetometer(temp);
     //qDebug << "mag mode is 0x:" << QString::number(configuration_settings.magConfig.opMode, 16) << endl;
}

/*
 * Whenever the user changed power setting for magnetometer
*/
void maindialog::on_mag_pwrBox_currentIndexChanged(int)
{
    mag_dataCollect();
}

/*
 * Whenever the user changed frequency setting for magnetometer
*/
void maindialog::on_mag_freqBox_currentIndexChanged(int)
{
    mag_dataCollect();
}

/* Enable/Disable mag sensor.
 * Disable all the configuration option if this button is clicked.
*/
void maindialog::IMUmag_Disable(bool disable)
{
    qDebug() << "IMU mag buttons status is :" << disable;
    ui->mag_timeclear_button->setDisabled(disable);
    ui->mag_pwrBox->setDisabled(disable);
    ui->mag_freqBox->setDisabled(disable);
}

/**/
void maindialog::mag_setDefault()
{
    on_mag_timeclear_button_clicked();
    ui->mag_pwrBox->setCurrentIndex(MAG_LP);
    ui->mag_freqBox->setCurrentIndex(MAG_FREQ_50HZ);

    MAG_CFG_t temp = {
              0,                // active hours
              MAG_LP_50_HZ      // mode
              };
    guiConfig.setMagnetometer(temp);
    mag_checkTimetoEnable();
}

void maindialog::on_mag_SW_clicked()
{
    QString title = ui->mag_SW->text();
    if(title == "ENABLE SENSOR")
    {
        ui->mag_SW->setText("DISABLE SENSOR");
        IMUmag_Disable(false);
        mag_disable_button(false);
    }else{
        ui->mag_SW->setText("ENABLE SENSOR");
        IMUmag_Disable(true);
        mag_disable_button(true);
    }
}


void maindialog::mag_timeTable_control()
{
    for(QPushButton* button : ui->magConfigPage->findChildren<QPushButton*>())
    {

        if(button->property("button_shift").isValid())
        {
            connect(button,SIGNAL(clicked()), this, SLOT(mag_hour_clicked()));
        }
    }

}

void maindialog::mag_hour_clicked()
{
    MAG_CFG_t temp = guiConfig.getMagConfig();
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(!button->property("clicked").isValid()) {
        button->setProperty("clicked", false);
    }
    bool clicked = button->property("clicked").toBool();
    button->setProperty("clicked", !clicked);

    if(!clicked) {
        button->setStyleSheet("background-color:rgb(253,199,0);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        temp.activeHour |= 1 << button->property("button_shift").toInt();
    } else {
        button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        temp.activeHour &= ~(1 << button->property("button_shift").toInt());
    }
    qDebug() << "mag hours are "<< temp.activeHour;
    guiConfig.setMagnetometer(temp);
}


void maindialog::mag_disable_button(bool disable)
{
    MAG_CFG_t temp = guiConfig.getMagConfig();
    for(QPushButton* button : ui->magConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            button->setDisabled(disable);
            if(disable){
                temp.activeHour = 0;
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }else{
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }
    guiConfig.setMagnetometer(temp);
}

void maindialog::on_mag_timeclear_button_clicked()
{
    MAG_CFG_t temp = guiConfig.getMagConfig();
    for(QPushButton* button : ui->magConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            temp.activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        }
    }
    guiConfig.setMagnetometer(temp);
}

