#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include <cmath>
#include "maindialog.h"
#include "ui_maindialog.h"

/* Enable/Disable Ekg sensor.
 * Disable all the configuration option if this button is clicked.
*/

void maindialog::ekg_getloadData(){
    for(QPushButton* button : ui->ekgConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            shift_property = button->property("button_shift").toInt();
            bit_Mask = (0x01 << shift_property);
            if((guiConfig.getModularConfig().activeHour&bit_Mask))
            {
                button->setProperty("clicked", true);
                button->setStyleSheet("background-color:rgb(253,199,0);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");

            }
        }
    }

    uint16_t ekg_spsSelect = 512/(pow(2,(uint8_t)guiConfig.getModularConfig().rate));
        uint8_t ekg_gainSelect = guiConfig.getModularConfig().gain;
        uint8_t ekg_lpFreqSelect = guiConfig.getModularConfig().freq;

        if(ekg_spsSelect== 512)
        {
            ui->ekg_odr128->setChecked(false);
            ui->ekg_odr256->setChecked(false);
            ui->ekg_odr512->setChecked(true);
            on_ekg_odr512_clicked();
        }else if(ekg_spsSelect == 256)
        {
            ui->ekg_odr128->setChecked(false);
            ui->ekg_odr256->setChecked(true);
            ui->ekg_odr512->setChecked(false);
            on_ekg_odr256_clicked();
        }else if(ekg_spsSelect == 128)
        {
            ui->ekg_odr128->setChecked(true);
            ui->ekg_odr256->setChecked(false);
            ui->ekg_odr512->setChecked(false);
            on_ekg_odr128_clicked();
        }
        ui->ekg_gainBox->setCurrentIndex(ekg_gainSelect);
        ui->ekg_LPfreqBox->setCurrentIndex(ekg_lpFreqSelect);

}

void maindialog::on_ekg_gainBox_currentIndexChanged(int index)
{
    MOD_CFG_t temp = guiConfig.getModularConfig();

    switch(index){
    case EKG_20_GAIN:
        temp.gain = ECG_GAIN_20_V;
        break;
    case EKG_40_GAIN:
        temp.gain = ECG_GAIN_40_V;
        break;
    case EKG_80_GAIN:
        temp.gain = ECG_GAIN_80_V;
        break;
    case EKG_160_GAIN:
        temp.gain = ECG_GAIN_160_V;
        break;
    }

    guiConfig.setModular(temp);
}


void maindialog::on_ekg_SW_clicked()
{

    QString title = ui->ekg_SW->text();
    if(title == "ENABLE SENSOR")
    {
        ekg_Disable(false);
        ekg_disable_button(false);
        ui->ekg_SW->setText("DISABLE SENSOR");
    }else{
        ui->ekg_SW->setText("ENABLE SENSOR");
        ekg_Disable(true);
        ekg_disable_button(true);
    }

}

void maindialog::ekg_checkTimetoEnable(){
    if(guiConfig.getModularConfig().activeHour){
        ekg_Disable(false);
    }else{
        ekg_Disable(true);
    }
}

void maindialog::ekg_setDefault()
{
    ui->ekg_odr128->setChecked(true);
    on_ekg_odr128_clicked();

    ui->ekg_gainBox->setCurrentIndex(EKG_20_GAIN);
    ui->ekg_LPfreqBox->setCurrentIndex(EKG_LP_FREQ_40HZ);
    on_ekg_timeclear_button_clicked();

    MOD_CFG_t temp = {
        0,                                          // active hours
        ECG_RATE_MIN_SPS,                           // sampling rate
        ECG_GAIN_20_V,                              // gain
        ECG_LP_40_HZ                                // frequency
    };

    guiConfig.setModular(temp);

    ekg_checkTimetoEnable();
}


void maindialog::ekg_hour_clicked()
{
    MOD_CFG_t temp = guiConfig.getModularConfig();

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
    guiConfig.setModular(temp);
}

void maindialog::ekg_timeTable_control()
{
    for(QPushButton* button : ui->ekgConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            connect(button,SIGNAL(clicked()), this, SLOT(ekg_hour_clicked()));
        }
    }
}

void maindialog::ekg_estimation_control()
{
    for(QRadioButton* button : ui->ekgConfigPage->findChildren<QRadioButton*>())
    {
        connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
    }
    for(QPushButton* button : ui->ekgConfigPage->findChildren<QPushButton*>())
    {
        connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
        connect(button,SIGNAL(clicked()), this, SLOT(ekg_checkTimetoEnable()));
    }
}

void maindialog::ekg_Disable(bool disable)
{
    ui->ekg_timeclear_button->setDisabled(disable);
    ui->ekg_odr128->setDisabled(disable);
    ui->ekg_odr256->setDisabled(disable);
    ui->ekg_odr512->setDisabled(disable);
    ui->ekg_gainBox->setDisabled(disable);
    ui->ekg_LPfreqBox->setDisabled(disable);
}


void maindialog::on_ekg_odr128_clicked()
{
    MOD_CFG_t temp = guiConfig.getModularConfig();
    int rmIndex1;
    int rmIndex2;

    temp.rate = ECG_RATE_MIN_SPS;
    rmIndex1 = ui->ekg_LPfreqBox->findText("100 Hz");
    if(rmIndex1 >= 0)
    {
        ui->ekg_LPfreqBox->setCurrentIndex(EKG_LP_FREQ_40HZ);
        on_ekg_LPfreqBox_currentIndexChanged(EKG_LP_FREQ_40HZ);
        ui->ekg_LPfreqBox->removeItem(rmIndex1);
    }
    rmIndex2 = ui->ekg_LPfreqBox->findText("150 Hz");
    if(rmIndex2 >= 0)
    {
        ui->ekg_LPfreqBox->setCurrentIndex(EKG_LP_FREQ_40HZ);
        on_ekg_LPfreqBox_currentIndexChanged(EKG_LP_FREQ_40HZ);
        ui->ekg_LPfreqBox->removeItem(rmIndex2);
    }

    guiConfig.setModular(temp);
}

void maindialog::on_ekg_odr256_clicked()
{
    int addIndex;
    int rmIndex2;
    MOD_CFG_t temp = guiConfig.getModularConfig();

    temp.rate = ECG_RATE_MED_SPS;
    addIndex = ui->ekg_LPfreqBox->findText("100 Hz");
    if(addIndex < 0)
    {
        ui->ekg_LPfreqBox->insertItem(EKG_LP_FREQ_100HZ,"100 Hz");
    }
    rmIndex2 = ui->ekg_LPfreqBox->findText("150 Hz");
    if(rmIndex2 >= 0)
    {
        ui->ekg_LPfreqBox->setCurrentIndex(EKG_LP_FREQ_100HZ);
        on_ekg_LPfreqBox_currentIndexChanged(EKG_LP_FREQ_100HZ);
        ui->ekg_LPfreqBox->removeItem(rmIndex2);
    }
    guiConfig.setModular(temp);
}

void maindialog::on_ekg_odr512_clicked()
{
    int addIndex1;
    int addIndex2;
    MOD_CFG_t temp = guiConfig.getModularConfig();

    temp.rate = ECG_RATE_MAX_SPS;
    addIndex1 = ui->ekg_LPfreqBox->findText("100 Hz");
    addIndex2 = ui->ekg_LPfreqBox->findText("150 Hz");
    if(addIndex1 < 0)
    {
        ui->ekg_LPfreqBox->addItem("100 Hz");
    }
    if(addIndex2 < 0)
    {
        ui->ekg_LPfreqBox->addItem("150 Hz");
    }
    guiConfig.setModular(temp);
}

void maindialog::ekg_disable_button(bool disable)
{
    MOD_CFG_t temp = guiConfig.getModularConfig();
    for(QPushButton* button : ui->ekgConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid())
        {
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
    guiConfig.setModular(temp);
}

void maindialog::on_ekg_LPfreqBox_currentIndexChanged(int index)
{
    MOD_CFG_t temp = guiConfig.getModularConfig();

    switch (index) {
    case EKG_LP_FREQ_BYPASS:
        temp.freq= ECG_LP_BYPASS;
        break;
    case EKG_LP_FREQ_40HZ:
        temp.freq= ECG_LP_40_HZ;
        break;
    case EKG_LP_FREQ_100HZ:
        temp.freq= ECG_LP_100_HZ;
        break;
    case EKG_LP_FREQ_150HZ:
        temp.freq= ECG_LP_150_HZ;
        break;
    }
    guiConfig.setModular(temp);
}




void maindialog::on_ekg_timeclear_button_clicked()
{
    MOD_CFG_t temp = guiConfig.getModularConfig();
    for(QPushButton* button : ui->ekgConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            temp.activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        }
    }
    guiConfig.setModular(temp);
}

