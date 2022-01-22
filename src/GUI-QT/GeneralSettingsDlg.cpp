/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2006
 *
 * Author(s):
 *	Andrea Russo
 *
 * Description:
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include "DialogUtil.h"
#include "GeneralSettingsDlg.h"
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QMessageBox>
#include <QCheckBox>
#include <QShowEvent>
#include <QHideEvent>

/* Implementation *************************************************************/

GeneralSettingsDlg::GeneralSettingsDlg(CParameter& NParam, CSettings& NSettings,
    QWidget* parent) :
    QDialog(parent), Parameters(NParam), Settings(NSettings)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    setupUi(this);

    /* Set the validators fro the receiver coordinate */
    EdtLatitudeDegrees->setValidator(new QIntValidator(0, 90, EdtLatitudeDegrees));
    EdtLongitudeDegrees->setValidator(new QIntValidator(0, 180, EdtLongitudeDegrees));

    EdtLatitudeMinutes->setValidator(new QIntValidator(0, 59, EdtLatitudeMinutes));
    EdtLongitudeMinutes->setValidator(new QIntValidator(0, 59, EdtLongitudeMinutes));

    /* Connections */

    connect(buttonOk, SIGNAL(clicked()), SLOT(ButtonOkClicked()) );

    connect(EdtLatitudeNS, SIGNAL(textChanged( const QString &)), this
            , SLOT(CheckSN(const QString&)));

    connect(EdtLongitudeEW, SIGNAL(textChanged( const QString &)), this
            , SLOT(CheckEW(const QString&)));

    connect(CheckBoxUseGPS, SIGNAL(clicked()), SLOT(OnCheckBoxUseGPS()) );

    /* Set help text for the controls */
    AddWhatsThisHelp();
}

GeneralSettingsDlg::~GeneralSettingsDlg()
{
}

void GeneralSettingsDlg::hideEvent(QHideEvent*)
{
}

void GeneralSettingsDlg::showEvent(QShowEvent*)
{
    /* Clear all fields */
    EdtLongitudeDegrees->setText("");
    EdtLongitudeMinutes->setText("");
    EdtLongitudeEW->setText("");
    EdtLatitudeDegrees->setText("");
    EdtLatitudeMinutes->setText("");
    EdtLatitudeNS->setText("");

    LineEditGPSHost->setText(QString(Settings.Get("GPS", "host", string("localhost")).c_str()));
    LineEditGPSPort->setText(Settings.Get("GPS", "port", string("2947")).c_str());
    CheckBoxUseGPS->setChecked(Settings.Get("GPS", "usegpsd", false));

    /* Extract the receiver coordinates  */
    ExtractReceiverCoordinates();
}

void GeneralSettingsDlg::CheckSN(const QString& NewText)
{
    /* Only S or N char are accepted */

    const QString sVal = NewText.toUpper();

    if (sVal != "S" && sVal != "N" && sVal != "")
        EdtLatitudeNS->setText("");
    else
        EdtLatitudeNS->setText(sVal);
}

void GeneralSettingsDlg::CheckEW(const QString& NewText)
{
    /* Only E or W char are accepted */

    const QString sVal = NewText.toUpper();

    if (sVal != "E" && sVal != "W" && sVal != "")
        EdtLongitudeEW->setText("");
    else
        EdtLongitudeEW->setText(sVal);
}

void GeneralSettingsDlg::OnCheckBoxUseGPS()
{
    bool bUseGPS = CheckBoxUseGPS->isChecked();
    Parameters.Lock();
    Parameters.use_gpsd=bUseGPS;
    Parameters.restart_gpsd=bUseGPS;
    Parameters.Unlock();
}

void GeneralSettingsDlg::ButtonOkClicked()
{
    bool bOK = true;
    bool bAllEmpty = true;
    bool bAllCompiled = false;

    /* Check the values and close the dialog */

    if (ValidInput(EdtLatitudeDegrees) == false)
    {
        bOK = false;
        QMessageBox::information(this, "Dream",
                                 tr("Latitude value must be in the range 0 to 90")
                                 ,QMessageBox::Ok);
    }
    else if (ValidInput(EdtLongitudeDegrees) == false)
    {
        bOK = false;

        QMessageBox::information(this, "Dream",
                                 tr("Longitude value must be in the range 0 to 180")
                                 ,QMessageBox::Ok);
    }
    else if (ValidInput(EdtLongitudeMinutes) == false
             || ValidInput(EdtLatitudeMinutes) == false)
    {
        bOK = false;

        QMessageBox::information(this, "Dream",
                                 tr("Minutes value must be in the range 0 to 59")
                                 ,QMessageBox::Ok);
    }

    if (bOK)
    {
        /* Check if all coordinates are empty */

        bAllEmpty = (EdtLongitudeDegrees->text()
                     + EdtLongitudeMinutes->text()
                     + EdtLongitudeEW->text()
                     + EdtLatitudeDegrees->text()
                     + EdtLatitudeMinutes->text()
                     + EdtLatitudeNS->text()
                    ) == "";

        /* Check if all coordinates are compiled */

        bAllCompiled = (EdtLongitudeDegrees->text() != "")
                       && (EdtLongitudeMinutes->text() != "")
                       && (EdtLongitudeEW->text() != "")
                       && (EdtLatitudeDegrees->text() != "")
                       && (EdtLatitudeMinutes->text() != "")
                       && (EdtLatitudeNS->text() != "");

        if (!bAllEmpty && !bAllCompiled)
        {
            bOK = false;

            QMessageBox::information(this, "Dream",
                                     tr("Compile all fields on receiver coordinates")
                                     ,QMessageBox::Ok);
        }
    }

    if (bOK)
    {
        /* save current settings */

        Parameters.Lock();

        if (!bAllEmpty)
        {
            double latitude, longitude;

            latitude = EdtLatitudeDegrees->text().toDouble() + EdtLatitudeMinutes->text().toDouble()/60.0;
            if(EdtLatitudeNS->text()[0]=='S' || EdtLatitudeNS->text()[0]=='s')
                latitude = - latitude;

            longitude = EdtLongitudeDegrees->text().toDouble() + EdtLongitudeMinutes->text().toDouble()/60.0;
            if(EdtLongitudeEW->text()[0]=='W' || EdtLongitudeEW->text()[0]=='w')
                longitude = - longitude;

            Parameters.gps_data.set |= LATLON_SET;
            Parameters.gps_data.fix.latitude = latitude;
            Parameters.gps_data.fix.longitude = longitude;

        }
        else
        {
            Parameters.gps_data.set &= ~LATLON_SET;
        }

        string host = LineEditGPSHost->text().toUtf8().constData();
        if(Parameters.gps_host != host)
            Parameters.restart_gpsd = true;
        Parameters.gps_host=host;
        string port = LineEditGPSPort->text().toUtf8().constData();
        if(Parameters.gps_port != port)
            Parameters.restart_gpsd = true;
        Parameters.gps_port=port;

        Parameters.Unlock();

        accept(); /* If the values are valid close the dialog */
    }
}

bool GeneralSettingsDlg::ValidInput(const QLineEdit* le)
{
    QString sText;

    /* Use the validator for check if the value is valid */

    sText = le->text();

    if (sText == "")
        return true;
    else
    {
        int iPosCursor = 0;
        return le->validator()->validate(sText,iPosCursor) == QValidator::Acceptable;
    }
}

QString GeneralSettingsDlg::ExtractDigits(const QString strStr, const int iStart
        , const int iDigits)
{
    QString sVal;
    QChar ch;
    bool bStop;

    /* Extract the first iDigits from position iStart */

    sVal = "";
    bStop = false;

    for (int i = iStart - 1 ; i <= iStart + iDigits - 1; i++)
    {
        if (bStop == false)
        {
            ch = strStr.at(i);
            if (ch.isDigit())
                sVal = sVal + ch;
            else
                bStop = true;
        }
    }
    return sVal;
}

void GeneralSettingsDlg::ExtractReceiverCoordinates()
{
    QString sVal, sDir;
    double latitude, longitude;

    Parameters.Lock();
    latitude = Parameters.gps_data.fix.latitude;
    longitude = Parameters.gps_data.fix.longitude;

    /* Extract latitude values */

    if(latitude<0.0)
    {
        latitude = -latitude;
        EdtLatitudeNS->setText("S");
    }
    else
    {
        EdtLatitudeNS->setText("N");
    }

    int degrees = int(latitude);
    int minutes = int(((floor((latitude - degrees) * 1000000) / 1000000) + 0.00005) * 60.0);

    /* Extract degrees */

    /* Latitude degrees max 2 digits */
    sVal = QString("%1").arg(degrees);

    EdtLatitudeDegrees->setText(sVal);


    sVal = QString("%1").arg(minutes);

    EdtLatitudeMinutes->setText(sVal);

    /* Extract longitude values */

    if(longitude<0.0)
    {
        longitude = -longitude;
        EdtLongitudeEW->setText("W");
    }
    else if(longitude>180.0)
    {
        longitude = 360.0-longitude;
        EdtLongitudeEW->setText("E");
    }
    else
    {
        EdtLongitudeEW->setText("E");
    }

    /* Extract degrees */

    degrees = int(longitude);
    minutes = int(((floor((longitude - degrees) * 1000000) / 1000000) + 0.00005) * 60.0);

    /* Longitude degrees max 3 digits */
    sVal = QString("%1").arg(degrees);

    EdtLongitudeDegrees->setText(sVal);

    /* Extract minutes */
    sVal = QString("%1").arg(minutes);

    EdtLongitudeMinutes->setText(sVal);

    Parameters.Unlock();

}

void GeneralSettingsDlg::AddWhatsThisHelp()
{
    QString str =
        tr("<b>Receiver coordinates:</b> Are used on "
           "Live Schedule Dialog to show a little green cube on the left"
           " of the target column if the receiver coordinates (latitude and longitude)"
           " are inside the target area of this transmission.<br>"
           "Receiver coordinates are also saved into the Log file.");
    setWhatsThis(str);
}

