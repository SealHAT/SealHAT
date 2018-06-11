#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include "maindialog.h"
#include "ui_maindialog.h"

/**************************************************************
 * FUNCTION: on_backButton_clicked
 * ------------------------------------------------------------
 * This function gets called when the Back Button is pressed
 * from any of the Device Configure pages. It sets the active
 * page to the Welcome Page.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_backButton_clicked()
{
    // Sets the dialog back to a login screen
    this->setFixedSize(350, 350);
    ui->mainStacked->setCurrentIndex(WELCOME_MAIN_STACK);
    ui->StartPageStacked->setCurrentIndex(INITIAL_PAGE);
}

/**************************************************************
 * FUNCTION: on_temperatureButton_clicked
 * ------------------------------------------------------------
 * This function gets called when the Temperature Button is
 * pressed. It sets the active page to the Configure
 * Temperature Sensor page. On this page, different attributes
 * of the temperature sensor may be set.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_temperatureButton_clicked()
{
    //set button colors. selected tab will be highlighted
    setActiveButtonColor(TEMPERATURE_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(TEMPERATURE_CONFIGURE);
}

/**************************************************************
 * FUNCTION: on_ekgButton_clicked
 * ------------------------------------------------------------
 * This function gets called when the EKG Button is pressed. It
 * sets the active page to the Configure EKG Sensor page. On
 * this page, different attributes of the EKG sensor may be set.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_ekgButton_clicked()
{
    //set button colors. selected tab will be highlighted
    setActiveButtonColor(EKG_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(EKG_CONFIGURE);
}

/**************************************************************
 * FUNCTION: on_imuButton_clicked
 * ------------------------------------------------------------
 * This function gets called when the IMU Button is pressed. It
 * sets the active page to the Configure IMU Sensor page. On
 * this page, different attributes of the IMU sensor may be set.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_xcelButton_clicked()
{
    setActiveButtonColor(XCEL_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(XCEL_CONFIGURE);
}

/*Switch from ACC to MAG*/
void maindialog::on_magButton_clicked()
{
    setActiveButtonColor(MAG_CONFIGURE);
    ui->ConfigurePages->setCurrentIndex(MAG_CONFIGURE);
}
/**************************************************************
 * FUNCTION: on_gpsButton_clicked
 * ------------------------------------------------------------
 * This function gets called when the GPS Button is pressed. It
 * sets the active page to the Configure GPS Sensor page. On
 * this page, different attributes of the GPS sensor may be set.
 *
 *  Parameters: none
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_gpsButton_clicked()
{
    //set button colors. selected tab will be highlighted
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

/**************************************************************
 * FUNCTION: on_TX_ReScanButton_clicked
 * ------------------------------------------------------------
 *  This function checks what serial port users selected
 *  in the TX_serialPort_comboBox. Set serial port to Read only.
 *
 *  Parameters: None
 *
 *  Returns: void
 **************************************************************/
void maindialog::on_TX_ReScanButton_clicked()
{
    ui->TX_serialPort_comboBox->clear();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
            ui->TX_serialPort_comboBox->addItem(serialPortInfo.portName());
        }
}
