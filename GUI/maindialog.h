#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include <QByteArray>
#include <QTextEdit>

#include <QThread>
#include <QSemaphore>
#include "src/sealhat_device.h"
#include "src/sensorconfig.h"

#include "seal_Types.h"

#define SPLIT_MSG_START_SYM (0xDEAD)
namespace Ui {class maindialog;}

class maindialog : public QDialog
{
    Q_OBJECT

    uint16_t accFrequency[7]    = {1,10,25,50,100,200,400};
    double   acc_actPower[3][7] = {
                                   {(3.7/1000000), (5.4/1000000), (8.0/1000000), (12.6/1000000), (22.0/1000000), (40.0/1000000), (75.0/1000000)},
                                   {(3.7/1000000), (5.4/1000000), (8.0/1000000), (12.6/1000000), (22.0/1000000), (40.0/1000000), (75.0/1000000)},
                                   {(3.7/1000000), (4.4/1000000), (5.6/1000000), (7.7/1000000),(11.7/1000000),(20.0/1000000),(36.0/1000000)}
                                  };

    uint16_t magFrequency[4] = {1,20,50,100};
    double   magPower[2][4]  = {
                                {(100.0/1000000), (200.0/1000000), (475.0/1000000), (950.0/1000000)},
                                {(25.0/1000000), (50.0/1000000), (125.0/1000000), (250.0/1000000)}
                               };

    double powerEst;
    uint32_t storageEst;

    // Time button mask and setting
    uint8_t shift_property;
    uint32_t bit_Mask;

    /*************
     * GUI PAGES *
     *************/
    enum MAIN_STACK_PAGES {
        WELCOME_MAIN_STACK      = 0,
        CONFIGURE_MAIN_STACK    = 1,
        RETRIEVE_MAIN_STACK     = 2,
        STREAM_MAIN_STACK       = 3,
    };

    enum WELCOME_PAGES {
        INITIAL_PAGE        = 0,
    };

    enum CONFIGURE_PAGES {
        CONFIGURE_DEV_HOME_PAGE = 0,
        XCEL_CONFIGURE          = 1,
        MAG_CONFIGURE           = 2,
        GPS_CONFIGURE           = 3,
        EKG_CONFIGURE           = 4,
        TEMPERATURE_CONFIGURE   = 5,
    };

    enum DATA_RETRIEVAL_PAGES {
        RETRIEVE_DATA_HOME_PAGE = 0,
        DOWNLOAD_PROGRESS_PAGE  = 1,
    };

    enum STREAM_DATA_PAGES {
        STREAM_DATA_HOME_PAGE   = 0,
    };

    enum ACC_PWR_MODE {
        ACC_NORMAL          = 0,
        ACC_HR              = 1,
        ACC_LP              = 2,
    };

    enum ACC_SCALE {
        ACC_2G              = 0,
        ACC_4G              = 1,
        ACC_8G              = 2,
        ACC_16G             = 3,
    };


    enum ACC_FREQUENCY_VALUES {
        ACC_FREQ_1HZ        = 0,
        ACC_FREQ_10HZ       = 1,
        ACC_FREQ_25HZ       = 2,
        ACC_FREQ_50HZ       = 3,
        ACC_FREQ_100HZ      = 4,
        ACC_FREQ_200HZ      = 5,
        ACC_FREQ_400HZ      = 6,
    };

    enum MAG_PWR_MODE {
        MAG_NORMAL          = 0,
        MAG_LP              = 1
    };

    enum MAG_FREQUENCY_VALUES {
        MAG_FREQ_10HZ       = 0,
        MAG_FREQ_20HZ       = 1,
        MAG_FREQ_50HZ       = 2,
        MAG_FREQ_100HZ      = 3,
    };

    enum MAG_GAIN_VALUES {
        EKG_20_GAIN         = 0,
        EKG_40_GAIN         = 1,
        EKG_80_GAIN         = 2,
        EKG_160_GAIN        = 3,
    };

    enum EKG_LP_FREQUENCY_VALUES {
        EKG_LP_FREQ_BYPASS  = 0,
        EKG_LP_FREQ_40HZ    = 1,
        EKG_LP_FREQ_100HZ   = 2,
        EKG_LP_FREQ_150HZ   = 3,
    };

    enum VALIDATOR_STATES {
        INVALID             = 0,
        INTERMEDIATE        = 1,
        ACCEPTABLE          = 2
    };

public:
    explicit maindialog(QWidget *parent = 0);
    friend QDataStream& operator>>(QDataStream& stream, const DATA_TRANSMISSION_t& txData);
    void centerDialog();
    ~maindialog();

private:
    // helper function to write to stream boxes
    void writeToStreamBox(QTextEdit* textBox, QString str);
    // changes the highlighting of the buttons
    void setActiveButtonColor(CONFIGURE_PAGES pageToHighlight);

private slots:
    void samplesReceived_stream(QQueue<SensorSample>* q);
    void on_retrieveDataButton_clicked();

    // Slots for navigation buttons
    void on_ekgButton_clicked();
    void on_gpsButton_clicked();
    void on_magButton_clicked();
    void on_xcelButton_clicked();
    void on_temperatureButton_clicked();
    void on_streamDataButton_clicked();
    void on_backButton_clicked();
    void on_configureDevOptionButton_clicked();
    void on_backButton_StreamPage_clicked();
    void on_backButton_retrieveData_clicked();

    // clears and re-populates the serial port list
    void on_TX_ReScanButton_clicked();
    void on_RXstream_ReScanButton_clicked();

    //Main function control
    //void hour_clicked();
    void sensors_timeTable_control();
    void labels_hide();
    void sensor_esitimation_control();
    void display_setReadOnly();

    //Accelerometer
    void xcel_setDefault();

    void on_xcel_SW_clicked();
    void on_xcel_scaleBox_currentIndexChanged(int index);
    void on_xcel_pwrBox_currentIndexChanged(int);
    void on_xcel_freqBox_currentIndexChanged(int);
    void on_xcel_sway_checkBox_clicked(bool checked);
    void on_xcel_surge_checkBox_clicked(bool checked);
    void on_xcel_heave_checkBox_clicked(bool checked);

    void on_xcel_thres_editingFinished();

    void IMUxcel_Disable(bool disable);
    void xcel_disable_button(bool disable);
    void on_xcel_timeclear_button_clicked();

    void xcel_timeTable_control();
    void xcel_estimation_control();
    void xcel_hour_clicked();
    void xcel_changeMode();
    void xcel_checkTimetoEnable();

    void xcel_getloadData();


//Magnetometer
    void mag_setDefault();

    void on_mag_SW_clicked();
    void on_mag_timeclear_button_clicked();
    void on_mag_pwrBox_currentIndexChanged(int);
    void on_mag_freqBox_currentIndexChanged(int);

    void IMUmag_Disable(bool disable);
    void mag_disable_button(bool disable);

    void mag_timeTable_control();
    void mag_estimation_control();
    void mag_hour_clicked();
    void mag_dataCollect();
    void mag_checkTimetoEnable();

    void mag_getloadData();

//EKG
    void ekg_setDefault();

    void on_ekg_SW_clicked();
    void on_ekg_timeclear_button_clicked();
    void on_ekg_odr256_clicked();
    void on_ekg_odr128_clicked();
    void on_ekg_odr512_clicked();
    void on_ekg_gainBox_currentIndexChanged(int index);
    void on_ekg_LPfreqBox_currentIndexChanged(int index);

    void ekg_Disable(bool disable);
    void ekg_disable_button(bool disable);

    void ekg_timeTable_control();
    void ekg_estimation_control();
    void ekg_hour_clicked();
    void ekg_checkTimetoEnable();

    void ekg_getloadData();

//GPS

    void gps_setDefault();

    void on_gps_SW_clicked();
    void on_gps_timeclear_button_clicked();

    void gps_disable(bool disable);
    void gps_disable_button(bool disable);

    void gps_timeTable_control();
    void gps_estimation_control();
    void gps_hour_clicked();
    void gps_checkTimetoEnable();

    void gps_getloadData();

//Temperature
    void temp_setDefault();

    void on_temp_SW_clicked();
    void on_temp_timeclear_button_clicked();
    void on_temp_samplePeriod_editingFinished();

    void temp_disable(bool disable);
    void temp_disable_button(bool disable);

    void temp_timeTable_control();
    void temp_hour_clicked();
    void temp_checkTimetoEnable();

    void temp_getloadData();

//Data-Retrival Page
    void on_chooseDestButton_clicked();
    void on_storeData_destinationEdit_returnPressed();

//Completed Configuration list setup and ready to submit
    void on_getDataButton_clicked();
    void submitConfig();
    void on_saveButton_clicked();
    void configureSettingListDisplay();
    void on_loadButton_clicked();

//Load data and reset sensors display
    void collectLoadingData_fromFile();
    void loaddata_fromSensors();

//Estimation&Analyzation

    void generalEstimation();

    void on_batterySizeText_selectionChanged();

    void on_batterySizeText_editingFinished();

//Data Sample Stream
    void on_captureDatatoFile_button_clicked();
    void on_startStream_button_clicked();

    void on_sendConfigsButton_clicked();
    void on_configureHomeButton_clicked();

    //void on_batterySizeText_returnPressed();
    void on_button_rescanDataRetrievalCOM_clicked();

    private:
        Ui::maindialog  *ui;
        SealHAT_device   device;         // the sealHat device
        QByteArray       dataBuffer;     // data from the device
        QFile            streamOut;      // stream file output
        SensorConfig     guiConfig;      // configuration being created in the gui
        QMap<QString, uint32_t> config;

        QSerialPort *microSerial;
        static const quint16 microSerial_vendor_id = 1003;
        static const quint16 microSerial_product_id = 9220;

        QString microSerial_port_name;
        int pos;
        bool microSerial_is_available;
        bool serial_retry;

        DATA_TRANSMISSION_t retrieve_data;
        DATA_HEADER_t header;


};

QDataStream& operator<<(QDataStream& stream, const SENSOR_CONFIGS_t& configs);

#endif // MAINDIALOG_H
