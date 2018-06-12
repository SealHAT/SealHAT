#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

void maindialog::xcel_changeMode()
{
    uint8_t pwrIndex = 0;
    uint8_t freq;

    pwrIndex= ui->xcel_pwrBox->currentIndex();
    freq = ui->xcel_freqBox->currentIndex();

    if(pwrIndex == ACC_LP)
    {
        switch(freq){
        case ACC_FREQ_1HZ:
            configuration_settings.accConfig.opMode = ACC_LP_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            configuration_settings.accConfig.opMode = ACC_LP_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            configuration_settings.accConfig.opMode = ACC_LP_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            configuration_settings.accConfig.opMode = ACC_LP_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            configuration_settings.accConfig.opMode = ACC_LP_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            configuration_settings.accConfig.opMode = ACC_LP_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            configuration_settings.accConfig.opMode = ACC_LP_400_HZ;
        break;
        }

    }else if(pwrIndex == ACC_NORMAL){
        switch(freq){
        case ACC_FREQ_1HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            configuration_settings.accConfig.opMode = ACC_NORM_400_HZ;
        break;
        }

    }else if(pwrIndex == ACC_HR){
        switch(freq){
        case ACC_FREQ_1HZ:
            configuration_settings.accConfig.opMode = ACC_HR_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            configuration_settings.accConfig.opMode = ACC_HR_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            configuration_settings.accConfig.opMode = ACC_HR_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            configuration_settings.accConfig.opMode = ACC_HR_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            configuration_settings.accConfig.opMode = ACC_HR_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            configuration_settings.accConfig.opMode = ACC_HR_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            configuration_settings.accConfig.opMode = ACC_HR_400_HZ;
        break;
        }
    }
}

void maindialog::on_xcel_scaleBox_currentIndexChanged(int index)
{
    switch(index){
    case ACC_2G:    configuration_settings.accConfig.scale = ACC_SCALE_2G;
        break;
    case ACC_4G:    configuration_settings.accConfig.scale = ACC_SCALE_4G;
        break;
    case ACC_8G:    configuration_settings.accConfig.scale = ACC_SCALE_8G;
        break;
    case ACC_16G:    configuration_settings.accConfig.scale = ACC_SCALE_16G;
        break;
    }
}

void maindialog::on_xcel_pwrBox_currentIndexChanged(int)
{
    xcel_changeMode();
}

void maindialog::on_xcel_freqBox_currentIndexChanged(int)
{
    xcel_changeMode();
}

void maindialog::on_xcel_sway_checkBox_clicked(bool checked)
{
    if(checked){
        configuration_settings.accConfig.sensitivity |= (MOTION_INT_X_LOW|MOTION_INT_X_HIGH);
    }else{
        configuration_settings.accConfig.sensitivity &= (~(MOTION_INT_X_LOW|MOTION_INT_X_HIGH));
    }

}

void maindialog::on_xcel_surge_checkBox_clicked(bool checked)
{
    if(checked){
        configuration_settings.accConfig.sensitivity |= (MOTION_INT_Y_LOW|MOTION_INT_Y_HIGH);
    }else{
        configuration_settings.accConfig.sensitivity &= (~(MOTION_INT_Y_LOW|MOTION_INT_Y_HIGH));
    }
}

void maindialog::on_xcel_heave_checkBox_clicked(bool checked)
{
    if(checked){
        configuration_settings.accConfig.sensitivity |= (MOTION_INT_Z_LOW|MOTION_INT_Z_HIGH);
    }else{
        configuration_settings.accConfig.sensitivity &= (~(MOTION_INT_Z_LOW|MOTION_INT_Z_HIGH));
    }
}

/* Enable/Disable xcel sensor.
 * Disable all the configuration option if this button is clicked.
*/
void maindialog::IMUxcel_Disable(bool disable)
{
    ui->xcel_timeclear_button->setDisabled(disable);
    ui->xcel_scaleBox->setDisabled(disable);
    ui->xcel_pwrBox->setDisabled(disable);
    ui->xcel_freqBox->setDisabled(disable);
    ui->xcel_sway_checkBox->setDisabled(disable);
    ui->xcel_surge_checkBox->setDisabled(disable);
    ui->xcel_heave_checkBox->setDisabled(disable);
    ui->xcel_thres->setDisabled(disable);
    if(disable){
        ui->thres_warnLABEL->setVisible(!disable);
    }
}

void maindialog::xcel_setDefault()
{
    on_xcel_timeclear_button_clicked();
    ui->xcel_scaleBox->setCurrentIndex(ACC_2G);
    ui->xcel_pwrBox->setCurrentIndex(ACC_HR);
    ui->xcel_freqBox->setCurrentIndex(ACC_FREQ_50HZ);
    ui->xcel_thres->setText("0.3");
    uint16_t size = sizeof(ACC_FULL_SCALE_t) + sizeof(ACC_OPMODE_t) + 2*sizeof(uint8_t) + sizeof(uint16_t);

    configuration_settings.accConfig = {
        0,                                                      // active hours
        ACC_SCALE_2G,                                           // scale
        ACC_HR_50_HZ,                                           // mode
        0x00,                                                  // sensitivity
        300                                                    // threshold
    };
    xcel_checkTimetoEnable();
}

void maindialog::on_xcel_SW_clicked()
{
    QString title = ui->xcel_SW->text();
    if(title == "Enable")
    {
        ui->xcel_SW->setText("Disable");
        IMUxcel_Disable(false);
        xcel_disable_button(false);
    }else{
        ui->xcel_SW->setText("Enable");
        IMUxcel_Disable(true);
        xcel_disable_button(true);
    }
}

/*
 * Reset all the time buttons in accelerometer page and set all the color back to default
 * Reset time configuration to zero for accelerometer
*/
void maindialog::on_xcel_timeclear_button_clicked()
{
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            configuration_settings.accConfig.activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
}

void maindialog::xcel_estimation_control()
{
    for(QComboBox* box : ui->xcelConfigPage->findChildren<QComboBox*>())
    {
        connect(box,SIGNAL(currentIndexChanged(int)), this, SLOT(generalEstimation()));
    }
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()){
        connect(button,SIGNAL(clicked()), this, SLOT(generalEstimation()));
        connect(button,SIGNAL(clicked()), this, SLOT(xcel_checkTimetoEnable()));
    }
}

/*Check the value in threshold blank and enable the warning
 * if the value user put in is not is the range [0,16] with
 * less than or equal to 5 digits after the decimal point
*/
void maindialog::on_xcel_thres_editingFinished()
{
    QPalette sample_palette;
    sample_palette.setColor(QPalette::WindowText, Qt::red);

    int valid = INVALID;
    int pos;

    if(ui->xcel_scaleBox->currentIndex() == ACC_2G)
    {
        QDoubleValidator v(0, 2, 5,this);
        QString thres = ui->xcel_thres->text();
        valid = v.validate(thres, pos);
        ui->thres_warnLABEL->setText("Invalid threshold value! (0-2)");

    }else if(ui->xcel_scaleBox->currentIndex() == ACC_4G)
    {
        QDoubleValidator v(0, 4, 3,this);
        QString thres = ui->xcel_thres->text();
        valid = v.validate(thres, pos);
        ui->thres_warnLABEL->setText("Invalid threshold value! (0-4)");
    }else if(ui->xcel_scaleBox->currentIndex() == ACC_8G)
    {
        QDoubleValidator v(0, 8, 3,this);
        QString thres = ui->xcel_thres->text();
        valid = v.validate(thres, pos);
        ui->thres_warnLABEL->setText("Invalid threshold value! (0-8)");
    }else if(ui->xcel_scaleBox->currentIndex() == ACC_16G)
    {
        QDoubleValidator v(0, 16, 3,this);
        QString thres = ui->xcel_thres->text();
        valid = v.validate(thres, pos);
        ui->thres_warnLABEL->setText("Invalid threshold value! (0-16)");
    }
    ui->thres_warnLABEL->setAutoFillBackground(true);
    ui->thres_warnLABEL->setPalette(sample_palette);

    if(valid != ACCEPTABLE){
        ui->thres_warnLABEL->show();
    }else{
        ui->thres_warnLABEL->hide();
        configuration_settings.accConfig.threshold = (ui->xcel_thres->text().toDouble())*1000;
        //qDebug() << configuration_settings.accConfig.acc_threshold << endl;
    }

}


void maindialog::xcel_hour_clicked()
{

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(!button->property("clicked").isValid()) {
        button->setProperty("clicked", false);
    }
    bool clicked = button->property("clicked").toBool();
    button->setProperty("clicked", !clicked);
        if(!clicked) {
            button->setStyleSheet("background-color:rgb(253,199,0);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");//background-color: rgb(172, 182, 193);
            configuration_settings.accConfig.activeHour |= 1 << button->property("button_shift").toInt();
        } else {
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            configuration_settings.accConfig.activeHour &= ~(1 << button->property("button_shift").toInt());
        }
}

void maindialog::xcel_checkTimetoEnable(){
    if(configuration_settings.accConfig.activeHour){
        IMUxcel_Disable(false);
    }else{
        IMUxcel_Disable(true);
    }
}

void maindialog::xcel_timeTable_control()
{
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>())
    {
        if(button->property("button_shift").isValid())
        {
            connect(button,SIGNAL(clicked()), this, SLOT(xcel_hour_clicked()));
        }

    }
}

void maindialog::xcel_getloadData(){
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            shift_property = button->property("button_shift").toInt();
            bit_Mask = (0x01 << shift_property);
            if((configuration_settings.accConfig.activeHour&bit_Mask))
            {
                      button->setProperty("clicked", true);
                      button->setStyleSheet("background-color:rgb(34,139,34)");

            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(152, 162, 173)");
            }
        }
    }

    uint8_t acc_scaleSelect =  (configuration_settings.accConfig.scale/16)%10 ;
    uint8_t acc_freqSelect = (configuration_settings.accConfig.opMode/16)%10 - 1;
    uint8_t xcel_pwrSelect = (configuration_settings.accConfig.opMode%16)/4;
    uint8_t xcel_sensitivity = configuration_settings.accConfig.sensitivity;
    QString xcel_threshold = QString::number((double)configuration_settings.accConfig.threshold/1000,'f',2);

    ui->xcel_scaleBox->setCurrentIndex(acc_scaleSelect);
    ui->xcel_pwrBox->setCurrentIndex(xcel_pwrSelect);
    ui->xcel_freqBox->setCurrentIndex(acc_freqSelect);

    if((xcel_sensitivity&(MOTION_INT_X_LOW|MOTION_INT_X_HIGH)))
    {
        ui->xcel_sway_checkBox->setChecked(true);
        on_xcel_sway_checkBox_clicked(true);
    }else{
        ui->xcel_sway_checkBox->setChecked(false);
        on_xcel_sway_checkBox_clicked(false);
    }
    if((xcel_sensitivity&(MOTION_INT_Y_LOW|MOTION_INT_Y_HIGH)))
    {
        ui->xcel_surge_checkBox->setChecked(true);
        on_xcel_surge_checkBox_clicked(true);
    }else{
        ui->xcel_surge_checkBox->setChecked(false);
        on_xcel_surge_checkBox_clicked(false);
    }
    if((xcel_sensitivity&(MOTION_INT_Z_LOW|MOTION_INT_Z_HIGH)))
    {
        ui->xcel_heave_checkBox->setChecked(true);
        on_xcel_heave_checkBox_clicked(true);
    }else{
        ui->xcel_heave_checkBox->setChecked(false);
        on_xcel_heave_checkBox_clicked(false);
    }

    ui->xcel_thres->setText(xcel_threshold);

}

void maindialog::xcel_disable_button(bool disable)
{
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            button->setDisabled(disable);
            if(disable){
                configuration_settings.accConfig.activeHour = 0;
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }else{
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }
}
