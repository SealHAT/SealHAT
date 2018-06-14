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

    ui->retrieveDataButton->hide();

    //Hide all the warning labels
    labels_hide();

    //Set all the sensor configuration back to default
    xcel_setDefault();
    mag_setDefault();
    ekg_setDefault();
    temp_setDefault();
    gps_setDefault();

    //Caculate default power consumption, battery lifetime and
    generalEstimation();

    sensors_timeTable_control();
    sensor_esitimation_control();

    display_setReadOnly();
    configureSettingListDisplay();

    on_TX_ReScanButton_clicked();
    on_RXstream_ReScanButton_clicked();

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

/*
void maindialog::on_retrieveDataButton_clicked()
{
    this->setFixedSize(850, 558);

    ui->mainStacked->setCurrentIndex(RETRIEVE_MAIN_STACK);
    ui->ConfigurePages->setCurrentIndex(RETRIEVE_DATA_HOME_PAGE);
    //on_RX_ReScanButton_clicked();
    this->centerDialog();
}*/

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

/**************************************************************
 * This function gets called when the Back Button is pressed
 * from any of the Device Configure pages. It sets the active
 * page to the Welcome Page.
 **************************************************************/
void maindialog::on_backButton_clicked()
{
    // Sets the dialog back to a login screen
    this->setFixedSize(350, 350);
    ui->mainStacked->setCurrentIndex(WELCOME_MAIN_STACK);
    ui->StartPageStacked->setCurrentIndex(INITIAL_PAGE);
}

/**************************************************************
 * NAVIGATION FUNCTIONS
 * ------------------------------------------------------------
 * These functions change the active page and change the button highlight
 **************************************************************/
void maindialog::on_temperatureButton_clicked()
{
    setActiveButtonColor(TEMPERATURE_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(TEMPERATURE_CONFIGURE);
}

void maindialog::on_ekgButton_clicked()
{
    setActiveButtonColor(EKG_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(EKG_CONFIGURE);
}

void maindialog::on_xcelButton_clicked()
{
    setActiveButtonColor(XCEL_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(XCEL_CONFIGURE);
}

void maindialog::on_magButton_clicked()
{
    setActiveButtonColor(MAG_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(MAG_CONFIGURE);
}

void maindialog::on_gpsButton_clicked()
{
    setActiveButtonColor(GPS_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(GPS_CONFIGURE);
}

/**************************************************************
 * FUNCTION: setActiveButtonColor
 * ------------------------------------------------------------
 * This function sets the colors of the sensor buttons. The
 * button for the page that is currently active will be
 * highlighted a different color. All of the remaining buttons
 * will be their normal color. This function uses the
 * enumerated page number to determine which button to
 * highlight. All of the other buttons are "set" to their
 * normal colors if any of them were previously highlighted.
 * Upon returning to the configure homepage, all of the buttons
 * are set back to their normal color.
 *
 *  Parameters:
 *      pageToHighlight : Enum value of button to highlight.
 *
 *  Returns: void
 **************************************************************/
void maindialog::setActiveButtonColor(CONFIGURE_PAGES pageToHighlight)
{
    switch(pageToHighlight)
    {
        case GPS_CONFIGURE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->gpsButton->setStyleSheet("background-color:rgb(253, 199, 0)");
            ui->xcelButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->magButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(152, 162, 173)");
        } break;
        case XCEL_CONFIGURE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->gpsButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->xcelButton->setStyleSheet("background-color:rgb(253, 199, 0)");
            ui->magButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(152, 162, 173)");
        } break;
        case MAG_CONFIGURE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->gpsButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->magButton->setStyleSheet("background-color:rgb(253, 199, 0)");
            ui->xcelButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(152, 162, 173)");
        } break;
        case EKG_CONFIGURE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(253, 199, 0)");
            ui->gpsButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->xcelButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->magButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(152, 162, 173)");
        } break;
        case TEMPERATURE_CONFIGURE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->gpsButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->xcelButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->magButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(253, 199, 0)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(152, 162, 173)");
        } break;
        case CONFIGURE_DEV_HOME_PAGE: {
            ui->ekgButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->gpsButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->xcelButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->magButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->temperatureButton->setStyleSheet("background-color:rgb(152, 162, 173)");
            ui->configureHomeButton->setStyleSheet("background-color:rgb(253, 199, 0)");
        } break;
    }
}

/**
 * This function checks what serial port users selected
 * in the TX_serialPort_comboBox. Set serial port to Read only.
 */
void maindialog::on_TX_ReScanButton_clicked()
{
    ui->TX_serialPort_comboBox->clear();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        ui->TX_serialPort_comboBox->addItem(serialPortInfo.portName());
    }
}
