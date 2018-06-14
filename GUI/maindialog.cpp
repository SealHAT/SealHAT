#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QtWidgets>
#include "maindialog.h"
#include "ui_maindialog.h"

/**
 * Initial GUI setup.
 **/
maindialog::maindialog(QWidget *parent) : QDialog(parent), ui(new Ui::maindialog)
{
    ui->setupUi(this);
    microSerial = new QSerialPort(this);

    //configuration_settings.temperature_config.temp_samplePeriod = 1;
    // On the Login stack, set the welcome page.
    ui->StartPageStacked->setCurrentIndex(INITIAL_PAGE);

    // Set size for smaller welcome screen.
    this->setFixedSize(350, 350);

    //Hide all the warning labels
    labels_hide();

    //Set all the sensor configuration back to default
    sensors_setDefault();

    //Caculate default power consumption, battery lifetime and
    generalEstimation();

    sensors_timeTable_control();
    sensor_esitimation_control();

    display_setReadOnly();
    configureSettingListDisplay();

    on_TX_ReScanButton_clicked();
    on_RXstream_ReScanButton_clicked();

}

/*
 * Set all the sensors to default configuration
 * and default display
*/
void maindialog::sensors_setDefault()
{
    xcel_setDefault();
    mag_setDefault();
    ekg_setDefault();
    temp_setDefault();
    gps_setDefault();
}

void maindialog::sensor_esitimation_control(){
    ekg_estimation_control();
    xcel_estimation_control();
    mag_estimation_control();
    gps_estimation_control();
}

/**/
void maindialog::display_setReadOnly()
{
    ui->gps_highSamplingRateValue->setReadOnly(true);
    ui->gps_lowSamplingRateValue->setReadOnly(true);
    //ui->pwrEst_Text->setReadOnly(true);
    ui->pwrConsumption_Text->setReadOnly(true);
    //ui->storageEst_Text->setReadOnly(true);
    ui->storageConsumption_Text->setReadOnly(true);
    ui->gps_configList->setReadOnly(true);
    ui->xcel_configList->setReadOnly(true);
    ui->mag_configList->setReadOnly(true);
}

/*Set all the sensors to default configuration*/
void maindialog::sensors_timeTable_control()
{
    temp_timeTable_control();
    ekg_timeTable_control();
    gps_timeTable_control();
    xcel_timeTable_control();
    mag_timeTable_control();
}


/* Hide all the warning label*/
void maindialog::labels_hide()
{
    ui->thres_warnLABEL->hide();
    ui->temp_warnLABEL->hide();
}

/**
 * Delete GUI on exit.
 **/
maindialog::~maindialog()
{
    streamOut.close();
    delete microSerial;
    delete ui;
}

void maindialog::on_configureDevOptionButton_clicked()
{
    this->setFixedSize(850, 558);

    ui->mainStacked->setCurrentIndex(CONFIGURE_MAIN_STACK);
    ui->ConfigurePages->setCurrentIndex(CONFIGURE_DEV_HOME_PAGE);
    setActiveButtonColor(CONFIGURE_DEV_HOME_PAGE);
}

/**
 * Center the contents of the page.
 **/
void maindialog::centerDialog() {
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);
}


void maindialog::on_streamDataButton_clicked()
{
    this->setFixedSize(850, 558);

    ui->mainStacked->setCurrentIndex(STREAM_MAIN_STACK);
    ui->ConfigurePages->setCurrentIndex(STREAM_DATA_HOME_PAGE);
    on_RXstream_ReScanButton_clicked();
    this->centerDialog();
}

void maindialog::on_batterySizeText_selectionChanged()
{
    ui->batterySizeText->clear();
}

void maindialog::on_retrieveDataButton_clicked()
{
    this->setFixedSize(850, 558);

    on_button_rescanDataRetrievalCOM_clicked();

    ui->mainStacked->setCurrentIndex(RETRIEVE_MAIN_STACK);
    ui->ConfigurePages->setCurrentIndex(RETRIEVE_DATA_HOME_PAGE);
    ui->getDataButton->setEnabled(false);
    ui->getDataButton->setStyleSheet("background-color:rgb(152, 162, 173)");
}

void maindialog::on_button_rescanDataRetrievalCOM_clicked()
{
    ui->comboBox_retrieveData->clear();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->comboBox_retrieveData->addItem(serialPortInfo.portName());
    }
}
