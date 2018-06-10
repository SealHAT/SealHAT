
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

/**************************************************************
 * FUNCTION: collectLoadingData_fromFile
 * ------------------------------------------------------------
 * This function gets called file is loaded back from clicking
 * the load button on the Device Configure pages.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::collectLoadingData_fromFile()
{
    configuration_settings.accelerometer_config.acc_activeHour = config[ "Accelerometer Time"];
    configuration_settings.accelerometer_config.acc_scale = (ACC_FULL_SCALE_t)config[ "Accelerometer Scale"];
    configuration_settings.accelerometer_config.acc_mode = (ACC_OPMODE_t)config[ "Accelerometer Mode"];
    configuration_settings.accelerometer_config.sensitivity = config[ "Accelerometer Sensitivity"];
    configuration_settings.accelerometer_config.threshold   = config["Accelerometer Threshold"];

    configuration_settings.magnetometer_config.mag_activeHour = config["Magnetometer Time"];
    configuration_settings.magnetometer_config.mag_mode = (MAG_OPMODE_t)config["Magnetometer Mode"];

    configuration_settings.ekg_config.ekg_activeHour = config["EKG Time"];
    configuration_settings.ekg_config.ekg_sampleRate = (ECG_SAMPLE_RATE_t)config["EKG Sample Rate"];
    configuration_settings.ekg_config.ekg_gain   = (ECG_GAIN_t)config["EKG Gain"];
    configuration_settings.ekg_config.ekg_lpFreq = (ECG_LOW_PASS_t)config["EKG Low Pass Frequency"];

    configuration_settings.temperature_config.temp_activeHour = config["Temperature and Light Time"];
    configuration_settings.temperature_config.temp_samplePeriod = config[ "Temperature and Light Sample Period"];

    configuration_settings.gps_config.gps_activeHour = config["GPS Time"];
    QString tempSWtext = ui->temp_SW->text();
    //qDebug() << "temperature switch text is"<< tempSWtext << configuration_settings.temperature_config.temp_activeHour;

    if((configuration_settings.temperature_config.temp_activeHour > 0) &&
            (tempSWtext== "Enable"))
    {
        ui->temp_SW->setText("Disable");
        //qDebug() << "temperature switch text is"<< tempSWtext;
    }
    if(configuration_settings.accelerometer_config.acc_activeHour > 0 &&
            ui->xcel_SW->text() == "Enable")
    {
        ui->xcel_SW->setText("Disable");
    }
    if(configuration_settings.magnetometer_config.mag_activeHour > 0 &&
            ui->mag_SW->text() == "Enable")
    {
        ui->mag_SW->setText("Disable");
    }
    if(configuration_settings.ekg_config.ekg_activeHour > 0 &&
            ui->ekg_SW->text() == "Enable")
    {
        ui->ekg_SW->setText("Disable");
    }
    if(configuration_settings.gps_config.gps_activeHour > 0 &&
            ui->gps_SW->text() == "Enable")
    {
        ui->gps_SW->setText("Disable");
    }

    loaddata_fromSensors();
    generalEstimation();

}
void maindialog::loaddata_fromSensors(){
    gps_getloadData();
    xcel_getloadData();
    ekg_getloadData();
    mag_getloadData();
    temp_getloadData();

    gps_checkTimetoEnable();
    xcel_checkTimetoEnable();
    ekg_checkTimetoEnable();
    mag_checkTimetoEnable();
    temp_checkTimetoEnable();


}

