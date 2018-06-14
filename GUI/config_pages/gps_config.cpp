#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"


void maindialog::gps_setDefault()
{
    // TODO: Are the rates in seconds?
    on_gps_timeclear_button_clicked();
    GPS_CFG_t temp = {
                        0,   // active hours
                        30,  // move rate
                        3600 // rest rate
                    };
    guiConfig.setGPS(temp);
    gps_checkTimetoEnable();
}

/*
 * Check if the gps button had been clicked,
 * If clicked, set the corresponding hour button
 * to be green and change the active time in GPS
 * configuration.
*/
void maindialog::gps_hour_clicked()
{
    GPS_CFG_t temp = guiConfig.getGPSConfig();
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(!button->property("clicked").isValid()) {
        button->setProperty("clicked", false);
    }
    bool clicked = button->property("clicked").toBool();
    button->setProperty("clicked", !clicked);
    if(!clicked) {
        button->setStyleSheet("background-color:rgb(253,199,0);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");//background-color: rgb(172, 182, 193);
        temp.activeHour |= 1 << button->property("button_shift").toInt();
    } else {
        button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        temp.activeHour &= ~(1 << button->property("button_shift").toInt());
    }
    guiConfig.setGPS(temp);
}

void maindialog::gps_timeTable_control()
{
    for(QPushButton* button : ui->gpsConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            connect(button,SIGNAL(clicked()), this, SLOT(gps_hour_clicked()));
        }
    }
}

void maindialog::gps_checkTimetoEnable(){
    if(guiConfig.getGPSConfig().activeHour){
        ui->gps_timeclear_button->setDisabled(false);
    }else{

        ui->gps_timeclear_button->setDisabled(true);
    }
}

void maindialog::gps_estimation_control()
{
    for(QPushButton* button : ui->gpsConfigPage->findChildren<QPushButton*>())
    {
        connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
        connect(button,SIGNAL(clicked()), this, SLOT(gps_checkTimetoEnable()));
    }
}

/* Enable/Disable GPS sensor.
 * Disable all the configuration option if this button is clicked.
*/
void maindialog::gps_disable(bool disable)
{
    ui->gps_timeclear_button->setDisabled(disable);
    gps_disable_button(disable);
}


void maindialog::on_gps_SW_clicked()
{
    QString title = ui->gps_SW->text();
    if(title == "Enable")
    {
        gps_disable(false);
        ui->gps_SW->setText("Disable");
    }else{
        ui->gps_SW->setText("Enable");
        gps_disable(true);
    }
    generalEstimation();
}


void maindialog::gps_getloadData(){

    for(QPushButton* button : ui->gpsConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            shift_property = button->property("button_shift").toInt();
            bit_Mask = (0x01 << shift_property);
            if((guiConfig.getGPSConfig().activeHour&bit_Mask))
            {
                      button->setProperty("clicked", true);
                      button->setStyleSheet("background-color:rgb(34,139,34)");

            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(152, 162, 173)");
            }
        }
    }

}


void maindialog::gps_disable_button(bool disable)
{
    for(QPushButton* button : ui->gpsConfigPage->findChildren<QPushButton*>()) {

        if(button->property("button_shift").isValid()) {
            button->setDisabled(disable);
            if(disable){
                gps_setDefault();
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }else{
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }
}

void maindialog::on_gps_timeclear_button_clicked()
{
    GPS_CFG_t temp = guiConfig.getGPSConfig();
    for(QPushButton* button : ui->gpsConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            temp.activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        }
    }
    guiConfig.setGPS(temp);
    //powerEstimation();
    //storageEstimation();
    //qDebug << "gps time is :" << configuration_settings.gpsConfig.activeHour << endl;
}



