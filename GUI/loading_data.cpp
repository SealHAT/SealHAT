
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
    ACC_CFG_t accNew;
    MAG_CFG_t magNew;
    ENV_CFG_t envNew;
    GPS_CFG_t gpsNew;
    MOD_CFG_t modNew;

    accNew.activeHour  = config[ "Accelerometer Time"];
    accNew.scale       = (ACC_FULL_SCALE_t)config[ "Accelerometer Scale"];
    accNew.opMode      = (ACC_OPMODE_t)config[ "Accelerometer Mode"];
    accNew.sensitivity = config[ "Accelerometer Sensitivity"];
    accNew.threshold   = config["Accelerometer Threshold"];

    magNew.activeHour = config["Magnetometer Time"];
    magNew.opMode     = (MAG_OPMODE_t)config["Magnetometer Mode"];

    modNew.activeHour = config["EKG Time"];
    modNew.rate       = (ECG_SAMPLE_RATE_t)config["EKG Sample Rate"];
    modNew.gain       = (ECG_GAIN_t)config["EKG Gain"];
    modNew.freq       = (ECG_LOW_PASS_t)config["EKG Low Pass Frequency"];

    envNew.activeHour = config["Temperature and Light Time"];
    envNew.period     = config[ "Temperature and Light Sample Period"];

    gpsNew.activeHour = config["GPS Time"];

    guiConfig.setAcceleration(accNew);
    guiConfig.setMagnetometer(magNew);
    guiConfig.setEnvironmental(envNew);
    guiConfig.setGPS(gpsNew);
    guiConfig.setModular(modNew);

    QString tempSWtext = ui->temp_SW->text();

    if((guiConfig.getEnvConfig().activeHour > 0) && (tempSWtext== "Enable")) {
        ui->temp_SW->setText("Disable");
    }

    if(guiConfig.getAccelConfig().activeHour > 0 && ui->xcel_SW->text() == "Enable") {
        ui->xcel_SW->setText("Disable");
    }

    if(guiConfig.getMagConfig().activeHour > 0 && ui->mag_SW->text() == "Enable") {
        ui->mag_SW->setText("Disable");
    }

    if(guiConfig.getModularConfig().activeHour > 0 && ui->ekg_SW->text() == "Enable") {
        ui->ekg_SW->setText("Disable");
    }

    if(guiConfig.getGPSConfig().activeHour > 0 && ui->gps_SW->text() == "Enable") {
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

