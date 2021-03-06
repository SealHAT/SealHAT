#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

void maindialog::xcel_changeMode()
{
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    uint8_t pwrIndex = 0;
    uint8_t freq;

    pwrIndex= ui->xcel_pwrBox->currentIndex();
    freq = ui->xcel_freqBox->currentIndex();

    if(pwrIndex == ACC_LP)
    {
        switch(freq){
        case ACC_FREQ_1HZ:
            temp.opMode = ACC_LP_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            temp.opMode = ACC_LP_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            temp.opMode = ACC_LP_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            temp.opMode = ACC_LP_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            temp.opMode = ACC_LP_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            temp.opMode = ACC_LP_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            temp.opMode = ACC_LP_400_HZ;
        break;
        }

    }else if(pwrIndex == ACC_NORMAL){
        switch(freq){
        case ACC_FREQ_1HZ:
            temp.opMode = ACC_NORM_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            temp.opMode = ACC_NORM_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            temp.opMode = ACC_NORM_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            temp.opMode = ACC_NORM_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            temp.opMode = ACC_NORM_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            temp.opMode = ACC_NORM_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            temp.opMode = ACC_NORM_400_HZ;
        break;
        }

    }else if(pwrIndex == ACC_HR){
        switch(freq){
        case ACC_FREQ_1HZ:
            temp.opMode = ACC_HR_1_HZ;
        break;
        case ACC_FREQ_10HZ:
            temp.opMode = ACC_HR_10_HZ;
        break;
        case ACC_FREQ_25HZ:
            temp.opMode = ACC_HR_25_HZ;
        break;
        case ACC_FREQ_50HZ:
            temp.opMode = ACC_HR_50_HZ;
        break;
        case ACC_FREQ_100HZ:
            temp.opMode = ACC_HR_100_HZ;
        break;
        case ACC_FREQ_200HZ:
            temp.opMode = ACC_HR_200_HZ;
        break;
        case ACC_FREQ_400HZ:
            temp.opMode = ACC_HR_400_HZ;
        break;
        }
    }
    guiConfig.setAcceleration(temp);
}

void maindialog::on_xcel_scaleBox_currentIndexChanged(int index)
{
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    switch(index){
    case ACC_2G:    temp.scale = ACC_SCALE_2G;
        break;
    case ACC_4G:    temp.scale = ACC_SCALE_4G;
        break;
    case ACC_8G:    temp.scale = ACC_SCALE_8G;
        break;
    case ACC_16G:    temp.scale = ACC_SCALE_16G;
        break;
    }
    guiConfig.setAcceleration(temp);
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
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    if(checked){
        temp.sensitivity |= (MOTION_INT_X_LOW|MOTION_INT_X_HIGH);
    }else{
        temp.sensitivity &= (~(MOTION_INT_X_LOW|MOTION_INT_X_HIGH));
    }
    guiConfig.setAcceleration(temp);
}

void maindialog::on_xcel_surge_checkBox_clicked(bool checked)
{
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    if(checked){
        temp.sensitivity |= (MOTION_INT_Y_LOW|MOTION_INT_Y_HIGH);
    }else{
        temp.sensitivity &= (~(MOTION_INT_Y_LOW|MOTION_INT_Y_HIGH));
    }
    guiConfig.setAcceleration(temp);
}

void maindialog::on_xcel_heave_checkBox_clicked(bool checked)
{
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    if(checked){
        temp.sensitivity |= (MOTION_INT_Z_LOW|MOTION_INT_Z_HIGH);
    }else{
        temp.sensitivity &= (~(MOTION_INT_Z_LOW|MOTION_INT_Z_HIGH));
    }
    guiConfig.setAcceleration(temp);
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

    ACC_CFG_t temp = {
        0,              // active hours
        ACC_SCALE_2G,   // scale
        ACC_HR_50_HZ,   // mode
        300,            // threshold
        0,              // duration
        0x00            // sensitivity
    };
    xcel_checkTimetoEnable();
}

void maindialog::on_xcel_SW_clicked()
{
    QString title = ui->xcel_SW->text();
    if(title == "ENABLE SENSOR")
    {
        ui->xcel_SW->setText("DISABLE SENSOR");
        IMUxcel_Disable(false);
        xcel_disable_button(false);
    }else{
        ui->xcel_SW->setText("ENABLE SENSOR");
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
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()) {
        if(button->property("button_shift").isValid()) {
            temp.activeHour = 0;
            button->setProperty("clicked", false);
            button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
        }
    }
    guiConfig.setAcceleration(temp);
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
    ACC_CFG_t temp = guiConfig.getAccelConfig();

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
        temp.threshold = (ui->xcel_thres->text().toDouble())*1000;
        //qDebug() << temp.accConfig.acc_threshold << endl;
    }
    guiConfig.setAcceleration(temp);
}


void maindialog::xcel_hour_clicked()
{
    ACC_CFG_t temp = guiConfig.getAccelConfig();
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
    guiConfig.setAcceleration(temp);
}

void maindialog::xcel_checkTimetoEnable(){
    if(guiConfig.getAccelConfig().activeHour){
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
            if((guiConfig.getAccelConfig().activeHour&bit_Mask))
            {
                button->setProperty("clicked", true);
                button->setStyleSheet("background-color:rgb(142, 152, 163);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");

            }else{
                button->setProperty("clicked", false);
                button->setStyleSheet("background-color:rgb(202, 212, 223);border:none;border-right-style:solid;border-left-style:solid;border-color:rgb(132, 142, 153);border-width:1px;border-top-style:none;border-bottom-style:none;");
            }
        }
    }

    uint8_t acc_scaleSelect =  (guiConfig.getAccelConfig().scale/16)%10 ;
    uint8_t acc_freqSelect = (guiConfig.getAccelConfig().opMode/16)%10 - 1;
    uint8_t xcel_pwrSelect = (guiConfig.getAccelConfig().opMode%16)/4;
    uint8_t xcel_sensitivity = guiConfig.getAccelConfig().sensitivity;
    QString xcel_threshold = QString::number((double)guiConfig.getAccelConfig().threshold/1000,'f',2);

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
    ACC_CFG_t temp = guiConfig.getAccelConfig();
    for(QPushButton* button : ui->xcelConfigPage->findChildren<QPushButton*>()) {
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
    guiConfig.setAcceleration(temp);
}
