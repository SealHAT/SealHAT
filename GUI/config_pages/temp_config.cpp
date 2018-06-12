#include <QDesktopWidget>
#include <QMessageBox>
#include <QIntValidator>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

void maindialog::temp_getloadData(){
    for(QPushButton* button : ui->tempConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            shift_property = button->property("button_shift").toInt();
            bit_Mask = (0x01 << shift_property);
            if((configuration_settings.temperature_config.temp_activeHour&bit_Mask))
            {
                      button->setProperty("clicked", true);
                      button->setStyleSheet("background-color:rgb(34,139,34)");

            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(152, 162, 173)");
            }
        }
    }

    QString temp_samplePeriod = QString::number(configuration_settings.temperature_config.temp_samplePeriod);
    ui->temp_samplePeriod->setText(temp_samplePeriod);

}

void maindialog::on_temp_SW_clicked()
{
    QString title = ui->temp_SW->text();
    if(title == "Enable")
    {
        temp_disable(false);
        temp_disable_button(false);
        ui->temp_SW->setText("Disable");
    }else{
        ui->temp_SW->setText("Enable");
        temp_disable_button(true);
        temp_disable(true);
    }
    generalEstimation();
}

void maindialog::temp_setDefault()
{
    configuration_settings.temperature_config.temp_samplePeriod = 1;
   on_temp_timeclear_button_clicked();
   ui->temp_samplePeriod->setText("1");
   configuration_settings.temperature_config = {
       0,                                                       // active hours
       1                                                        // sample period
   };
   qDebug() << sizeof(uint16_t);
   temp_checkTimetoEnable();
}

void maindialog::temp_disable(bool disable)
{
    ui->temp_timeclear_button->setDisabled(disable);
    ui->temp_samplePeriod->setDisabled(disable);
    ui->temp_warnLABEL->hide();
}

void maindialog::temp_disable_button(bool disable)
{
    for(QPushButton* button : ui->tempConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            button->setDisabled(disable);
            if(disable){
                configuration_settings.temperature_config = {
                    0,                                                       // active hours
                    1                                                        // sample period
                };
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }else{
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }
}

void maindialog::temp_hour_clicked()
{

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(!button->property("clicked").isValid()) {
        button->setProperty("clicked", false);
    }
    bool clicked = button->property("clicked").toBool();
    button->setProperty("clicked", !clicked);
        if(!clicked) {
            button->setStyleSheet("background-color:rgb(253,199,0);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            configuration_settings.temperature_config.temp_activeHour |= 1 << button->property("button_shift").toInt();
        } else {
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            configuration_settings.temperature_config.temp_activeHour &= ~(1 << button->property("button_shift").toInt());
        }

}

void maindialog::temp_timeTable_control()
{
    for(QPushButton* button : ui->tempConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            connect(button,SIGNAL(clicked()), this, SLOT(temp_hour_clicked()));
            connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
        }
        connect(button,SIGNAL(clicked()), this, SLOT(temp_checkTimetoEnable()));
    }
}

void maindialog::temp_checkTimetoEnable(){
    if(configuration_settings.temperature_config.temp_activeHour){
        temp_disable(false);
    }else{
        temp_disable(true);
    }
}

void maindialog::on_temp_samplePeriod_editingFinished()
{
    int valid;
    int pos;
    QIntValidator v(1, 65000, this);

    QString thres = ui->temp_samplePeriod->text();
    valid = v.validate(thres, pos);
    if(valid != ACCEPTABLE){
        ui->temp_warnLABEL->show();

    }else{
        configuration_settings.temperature_config.temp_samplePeriod = (ui->temp_samplePeriod->text().toUInt());
        ui->temp_warnLABEL->hide();
    }
    generalEstimation();
}

void maindialog::on_temp_timeclear_button_clicked()
{
    for(QPushButton* button : ui->tempConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            configuration_settings.temperature_config.temp_activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        }
    }
    generalEstimation();
}
