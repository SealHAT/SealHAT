#include <QDesktopWidget>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include <QPalette>
#include <cmath>
#include "maindialog.h"
#include "analyze.h"
#include "seal_Types.h"
#include "ui_maindialog.h"


/**************************************************************
 * FUNCTION: num_Hours
 * ------------------------------------------------------------
 * This function get the 32 bits binary time settings
 * and calculate number of active hours for all the sensors.
 *
 *  Parameters: Time configuration setting.(23bits one hot encoding)
 *
 *  Returns: void
 **************************************************************/
uint8_t maindialog::num_Hours(uint32_t x) {
  uint8_t hours =0;
  while(x!=0) {
    x = x & (x-1);
     hours++;
  }
  return hours;
}


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

    temp_activeHour = num_Hours(configuration_settings.temperature_config.temp_activeHour);
    temp_sampleNumber = temp_activeHour*3600/(configuration_settings.temperature_config.temp_samplePeriod);


    light_activeHour = num_Hours(configuration_settings.temperature_config.temp_activeHour);
    light_sampleNumber = light_activeHour*3600/(configuration_settings.temperature_config.temp_samplePeriod);

    ekg_activeHour = num_Hours(configuration_settings.ekg_config.ekg_activeHour);
    ekg_sampleNumber = ekg_activeHour*3600*(512/(pow(2,(uint8_t)configuration_settings.ekg_config.ekg_sampleRate)));

    acc_tens = (configuration_settings.accelerometer_config.acc_mode/16)%10 - 1; //logic Rethink
    acc_pwrMode = (configuration_settings.accelerometer_config.acc_mode%16)/4;
    acc_activeHour = num_Hours(configuration_settings.accelerometer_config.acc_activeHour);
    acc_sampleNumber = acc_activeHour*(3600)*(accFrequency[acc_tens]);

    mag_ones = (configuration_settings.magnetometer_config.mag_mode%16)/4;
    mag_pwrMode = (configuration_settings.magnetometer_config.mag_mode/16)%10;
    mag_activeHour = num_Hours(configuration_settings.magnetometer_config.mag_activeHour);
    mag_sampleNumber = mag_activeHour*3600*(magFrequency[mag_ones]);

    gps_activeHour = num_Hours(configuration_settings.gps_config.gps_activeHour);
    if(gps_activeHour <= 2){
        gps_sampleNumber = (gps_activeHour*3600)/(30);
    }else{
        gps_sampleNumber = ((2*3600)/(30) + (gps_activeHour-2));
    }

    storageEstimation();
    powerEstimation();
}


/**************************************************************
 * FUNCTION: powerEstimation
 * ------------------------------------------------------------
 *  This function takes all the configuration setting factors for
 *  all the sensors that could potentially affect power consumption.
 *  It caculate active power and inactive power for accelerometer,
 *  magnetometer, ekg, gps, temperature and light sensors respectively.
 *  And then display the sum of the power to the frontend.
 *  It will get called each configuration setting got changed.
 *  It gets called inside generalEstimation() function.
 *
 *  generalEstimation() will get called whenever each configuration
 *  setting that could affect power or storage got changed.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::powerEstimation(){
   /*TEMPERATURE POWER*/
    temp_activePower = ((((3600 - (temp_sampleNumber * TEMP_CONV_TIME)) - (temp_sampleNumber * TEMP_BIT_NUM * I2C_Speed)) * TEMP_SB_PWR)
                        + (temp_sampleNumber * TEMP_CONV_TIME * TEMP_CONV_PWR)
                        + (temp_sampleNumber * TEMP_BIT_NUM * TEMP_I2C_PWR * I2C_Speed)) * temp_activeHour/3600;
    temp_inactivePower = (24 - temp_activeHour) * TEMP_SB_PWR;
    temp_totalPower = (temp_activePower + temp_inactivePower);   //temp power per day//

    /*LIGHT POWER*/
     light_activePower = (((3600 - (light_sampleNumber * LIGHT_BIT_NUM * I2C_Speed)) * LIGHT_INACT_PWR)
                        + (( light_sampleNumber * LIGHT_BIT_NUM * I2C_Speed) * LIGHT_ACT_PWR)) * light_activeHour/3600;
     light_inactivePower = (24 - light_activeHour) * LIGHT_INACT_PWR;
     light_totalPower = light_activePower + light_inactivePower;   //light power per day//

     /*EKG POWER*/
     ekg_inactivePower = EKG_I_SAVDV * (24 - ekg_activeHour) + (EKG_I_OV * (24 - ekg_activeHour));
     ekg_activePower = (EKG_I_AVDV + EKG_I_OV) * ekg_activeHour;
     ekg_totalPower = ekg_inactivePower + ekg_activePower + EKG_OSCILLATOR;

     /*ACCELEROMETER POWER*/
     acc_inactivePower = IMU_SB_PWR * (24 - acc_activeHour);
     acc_activePower = acc_actPower[acc_pwrMode][acc_tens] * acc_activeHour ;//(EKG_I_AVDV + EKG_I_OV) * ekg_activeHour;
     acc_totalPower = acc_inactivePower + acc_activePower;

     /*MAGNETOMETER POWER*/
     mag_inactivePower = IMU_SB_PWR*(24 - mag_activeHour);
     mag_activePower = magPower[mag_pwrMode][mag_ones] * mag_activeHour;
     mag_totalPower = mag_inactivePower + mag_activePower;

     /*GPS POWER*/
     gps_inactivePower = GPS_SB_PWR * (24 - gps_activeHour);
     if(gps_activeHour <= 2){
         gps_activePower = (((GPS_ACQ_PWR*30.0/15.0)  + (GPS_SB_PWR* (30*14.0/15.0)))/30)  * gps_activeHour;
     }else{
         gps_activePower = (( (GPS_ACQ_PWR*30/15) + (GPS_SB_PWR* (30*14.0/15.0)) )/30) * 2 + ((GPS_ACQ_PWR*45.0)  + (GPS_SB_PWR* (3600-45)))  * (gps_activeHour-2)/3600;
     }

     gps_totalPower = gps_inactivePower + gps_activePower;

     /*MEMORY POWER*/  //what is the correct SPI time
     memory_totalpower = storageEst * SPI_SPEED/3600 * SPI_CURRENT
                         + (24 - (storageEst * SPI_SPEED)/3600) * SPI_SB_CURRENT
                         + (24 * SPI_SB_CURRENT * 3);

     /*MICRO POWER*/
     micro_tempActiveTime = temp_sampleNumber * 2 * (I2C_Speed + SPI_SPEED);
     micro_lightActiveTime = light_sampleNumber * 2 * (I2C_Speed + SPI_SPEED);
     micro_accActiveTime = acc_sampleNumber * 6 * (I2C_Speed + SPI_SPEED);
     micro_magActiveTime = mag_sampleNumber * 6 * (I2C_Speed + SPI_SPEED);
     micro_gpsActiveTime = (gps_sampleNumber * 100 * I2C_Speed) + (gps_sampleNumber * 20 * SPI_SPEED);
     micro_ekgActiveTime = ekg_sampleNumber * 3 * (I2C_Speed + SPI_SPEED);

     micro_activehour = (micro_tempActiveTime + micro_lightActiveTime + micro_accActiveTime + micro_magActiveTime
                      + micro_gpsActiveTime + micro_ekgActiveTime)*8/3600;
     micro_totalpower = micro_activehour * MICRO_ACT_PWR * (12) + ((24-micro_activehour) * MICRO_SB_PWR* (12));

    //SUM OF POWER

     powerEst = (temp_totalPower + light_totalPower + ekg_totalPower + acc_totalPower + mag_totalPower + gps_totalPower
                 + memory_totalpower + micro_totalpower) * 1000;
     QString powerEstString = " " + (QString::number(powerEst,'f',5));

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



/**************************************************************
 * FUNCTION: storageEstimation
 * ------------------------------------------------------------
 *  This function takes all the configuration setting factors for
 *  all the sensors that could potentially affect storage consumption.
 *  It caculate active power and inactive power for accelerometer,
 *  magnetometer, ekg, gps, temperature and light sensors respectively.
 *  And then display the sum of the storgae to the frontend.
 *  It gets called inside generalEstimation() function.
 *
 *  generalEstimation() will get called whenever each configuration
 *  setting that could affect power or storage got changed.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::storageEstimation(){

    templight_storage = (4 * ENV_LOG_SIZE + sizeof(DATA_HEADER_t)); //Environment size(bytes) of samples per day
    acc_storage = (6 * IMU_LOG_SIZE + sizeof(DATA_HEADER_t));  //Accelerometer size(bytes) of samples per day
    mag_storage = (6 * IMU_LOG_SIZE + sizeof(DATA_HEADER_t));  //Magnetometer size(bytes) of samples per day
    gps_storage = (20 * GPS_LOG_SIZE + sizeof(DATA_HEADER_t)); //Gps size(bytes) of samples per day
    ekg_storage = (3 * ECG_LOG_SIZE + sizeof(DATA_HEADER_t));  //Ekg size(bytes) of samples per day



    templight_groupNum = (temp_sampleNumber)/ENV_LOG_SIZE; //
    acc_groupNum = (acc_sampleNumber)/IMU_LOG_SIZE;
    mag_groupNum = (mag_sampleNumber)/IMU_LOG_SIZE;
    gps_groupNum = (gps_sampleNumber)/GPS_LOG_SIZE;
    ekg_groupNum = (ekg_sampleNumber)/ECG_LOG_SIZE;


    storageEst = (templight_storage * templight_groupNum
             + acc_storage * acc_groupNum
             + mag_storage * mag_groupNum
             + gps_storage * gps_groupNum
             + ekg_storage * ekg_groupNum) * 8; //Storage caculate in bits = total Bits

    QPalette warning_palette;
    warning_palette.setColor(QPalette::WindowText, Qt::red);

    double StorageConsump = (((double)storageEst * 90.0)*100.0)/STORAGECAPACITY;
    QString storageconsumpString = " " + QString::number(StorageConsump,'f',2) + " % ";


    if(StorageConsump > (90.0)){
        warning_palette.setColor(QPalette::Text,Qt::red);
    }else{
        warning_palette.setColor(QPalette::Text,Qt::black);
    }

    ui->storageConsumption_Text->setAutoFillBackground(true);

    ui->storageConsumption_Text->setPalette(warning_palette);

    ui->storageConsumption_Text->setText(storageconsumpString);
}
