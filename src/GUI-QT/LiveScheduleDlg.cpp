/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2005
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

#include "LiveScheduleDlg.h"
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QHideEvent>
#include <QShowEvent>
#include "../util-QT/Util.h"

/* Implementation *************************************************************/

QString
LiveScheduleDlg::ExtractTime(const CAltFreqSched& schedule)
{
    int iTimeStart = schedule.iStartTime;
    int iDuration = schedule.iDuration;
    QString sDays = "";
    QString sResult = "";

    if ((iTimeStart == 0) && (iDuration == 0))
        return sResult;

    /* Start time */
    int iStartMinutes = iTimeStart % 60;
    int iStartHours = iTimeStart / 60;

    /* Stop time */
    bool bAllWeek24Hours = false;
    const int iTimeStop = iTimeStart + iDuration;

    int iStopMinutes = iTimeStop % 60;
    int iStopHours = iTimeStop / 60;

    if (iStopHours > 24)
    {
        int iDays = iStopHours / 24;

        if (iDays == 7)
            /* All the week */
            bAllWeek24Hours = true;
        else
        {
            /* Add information about days duration */
            if (iDays > 1)
                sDays.sprintf(" (%d days)", iDays);
            iStopHours = iStopHours % 24;
        }
    }

    if (bAllWeek24Hours)
        sResult = "24 hours, 7 days a week";
    else
    {
        sResult.sprintf("%02d:%02d-%02d:%02d", iStartHours, iStartMinutes, iStopHours, iStopMinutes);
        sResult += sDays;
    }

    return sResult;
}

QString
LiveScheduleDlg::ExtractDaysFlagString(const int iDayCode)
{
    string strDaysFlags = "0000000";
    for (int i = 0; i < 7; i++)
    {
        if ((1 << (6 - i)) & iDayCode)
            strDaysFlags[i]++;
    }

    /* Init days string vector */
    const QString strDayDef[] = {
        QObject::tr("Mon"),
        QObject::tr("Tue"),
        QObject::tr("Wed"),
        QObject::tr("Thu"),
        QObject::tr("Fri"),
        QObject::tr("Sat"),
        QObject::tr("Sun")
    };

    QString strDaysShow = "";
    /* First test for day constellations which allow to apply special names */
    /* 1111111 = Mon Tue Wed Thu Fri Sat Sun = 1234567 (1 = Monday, 7 = Sunday) */
    if (strDaysFlags == "1111111")
        strDaysShow = QObject::tr("daily");
    else if (strDaysFlags == "1111100")
        strDaysShow = QObject::tr("from Mon to Fri");
    else if (strDaysFlags == "1111110")
        strDaysShow = QObject::tr("from Mon to Sat");
    else
    {
        /* No special name could be applied, just list all active days */
        for (size_t i = 0; i < 7; i++)
        {
            /* Check if day is active */
            if (strDaysFlags[i] == CHR_ACTIVE_DAY_MARKER)
            {
                /* Set commas in between the days, to not set a comma at
                   the beginning */
                if (strDaysShow != "")
                    strDaysShow += ",";

                /* Add current day */
                strDaysShow += strDayDef[i];
            }
        }
    }
    return strDaysShow;
}

void
CDRMLiveSchedule::SetReceiverCoordinates(double latitude, double longitude)
{
    dReceiverLatitude = latitude;
    dReceiverLongitude = longitude;
}

void
CDRMLiveSchedule::DecodeTargets(const vector < CAltFreqRegion >
                                vecRegions, string & strRegions,
                                bool & bIntoTargetArea)
{
    int iCIRAF;
    int iReceiverLatitude = int (dReceiverLatitude);
    int iReceiverLongitude = int (dReceiverLongitude);
    stringstream ssRegions;

    bIntoTargetArea = false;

    for(size_t i = 0; i < vecRegions.size(); i++)
    {
        const int iLatitude = vecRegions[i].iLatitude;
        const int iLongitude = vecRegions[i].iLongitude;

        const int iLatitudeEx = vecRegions[i].iLatitudeEx;
        const int iLongitudeEx = vecRegions[i].iLongitudeEx;

        size_t iCIRAFSize = vecRegions[i].veciCIRAFZones.size();

        if (iCIRAFSize > 0)
        {
            /* Targets */
            for (size_t j = 0; j < iCIRAFSize; j++)
            {
                iCIRAF = vecRegions[i].veciCIRAFZones[j];

                if (ssRegions.str() != "")
                    ssRegions << ", ";

                ssRegions << strTableCIRAFzones[iCIRAF];
            }
        }
        else
        {
            /* if ciraf zones aren't defined show the latitude and
             * longitude of the centre of the target area */

            if (ssRegions.str() != "")
                ssRegions << ", ";

            int iLatitudeMed = (iLatitude + (iLatitudeEx / 2));

            ssRegions << "latitude " << abs(iLatitudeMed) << "\xb0 ";

            if (iLatitudeMed < 0)
                ssRegions << 'S';
            else
                ssRegions << 'N';

            int iLongitudeMed = (iLongitude + (iLongitudeEx / 2));

            if (iLongitudeMed >= 180)
                iLongitudeMed = iLongitudeMed - 360;

            ssRegions << " longitude " << abs(iLongitudeMed) << "\xb0 ";

            if (iLongitudeMed < 0)
                ssRegions << 'W';
            else
                ssRegions << 'E';

        }
        /* check if receiver coordinates are inside target area
         * TODO check if inside CIRAF zones */
        bool bLongitudeOK = ((iReceiverLongitude >= iLongitude)
                                 && (iReceiverLongitude <=
                                     (iLongitude + iLongitudeEx)))
                                || (((iLongitude + iLongitudeEx) >= 180)
                                    && (iReceiverLongitude <=
                                        (iLongitude + iLongitudeEx - 360)));

        bool bLatitudeOK = ((iReceiverLatitude >= iLatitude)
                                && (iReceiverLatitude <=
                                    (iLatitude + iLatitudeEx)));

        bIntoTargetArea = bIntoTargetArea || (bLongitudeOK && bLatitudeOK);
    }
    strRegions = ssRegions.str();
}

void
CDRMLiveSchedule::LoadServiceDefinition(const CServiceDefinition& service,
                                        const CAltFreqSign& AltFreqSign, const uint32_t iServiceID)
{
    string strRegions = "";
    bool bIntoTargetArea = false;

    /* Region */
    if (service.iRegionID != 0)
        DecodeTargets(AltFreqSign.vecRegions[service.iRegionID], strRegions, bIntoTargetArea);

    /* For all frequencies */
    for (size_t j = 0; j < service.veciFrequencies.size(); j++)
    {
        if (service.iScheduleID > 0)
        {
            const vector<CAltFreqSched>& vecSchedules = AltFreqSign.vecSchedules[service.iScheduleID];
            for (size_t k = 0; k < vecSchedules.size(); k++)
            {
                CLiveScheduleItem LiveScheduleItem;

                /* Frequency */
                LiveScheduleItem.strFreq = service.Frequency(j);

                /* Add the target */
                LiveScheduleItem.strTarget = strRegions;

                /* Add the schedule */
                LiveScheduleItem.Schedule = vecSchedules[k];

                /* Local receiver coordinates are into target area or not */
                LiveScheduleItem.bInsideTargetArea = bIntoTargetArea;

                /* Add the system (transmission mode) */
                LiveScheduleItem.strSystem = service.System();

                /* Add the Service ID - 0 for DRM Muxes, ID of the Other Service if present */
                LiveScheduleItem.iServiceID = iServiceID;

                /* Add new item in table */
                StationsTable.push_back(LiveScheduleItem);
            }
        }
        else
        {
            CLiveScheduleItem LiveScheduleItem;

            /* Frequency */
            LiveScheduleItem.strFreq = service.Frequency(j);

            /* Add the target */
            LiveScheduleItem.strTarget = strRegions;

            /* Local receiver coordinates are into target area or not */
            LiveScheduleItem.bInsideTargetArea = bIntoTargetArea;

            /* Add the system (transmission mode) */
            LiveScheduleItem.strSystem = service.System();

            /* Add the Service ID - 0 for DRM Muxes, ID of the Other Service if present */
            LiveScheduleItem.iServiceID = iServiceID;

            /* Add new item in table */
            StationsTable.push_back(LiveScheduleItem);
        }
    }
}

void
CDRMLiveSchedule::LoadAFSInformations(const CAltFreqSign& AltFreqSign)
{
    size_t i;

    /* Init table for stations */
    StationsTable.clear();

    /* Add AFS information for DRM multiplexes */
    for (i = 0; i < AltFreqSign.vecMultiplexes.size(); i++)
    {
        /* TODO multiplex and restrictions */
        //service.bIsSyncMultplx;

        //for ( k = 0; k < 4; k++)
        //  service.veciServRestrict[k];

        LoadServiceDefinition(AltFreqSign.vecMultiplexes[i], AltFreqSign);
    }

    /* Add AFS information for Other Services */
    for (i = 0; i < AltFreqSign.vecOtherServices.size(); i++)
    {
        COtherService OtherService = AltFreqSign.vecOtherServices[i];

        /* TODO same service */
        //OtherService.bSameService;

        LoadServiceDefinition(OtherService, AltFreqSign, OtherService.iServiceID);
    }
}

LiveScheduleDlg::LiveScheduleDlg(CRx& nrx, CSettings& Settings,
                                 QMap<QWidget*,QString>& parents):
    CWindow(parents, Settings, "Live Schedule"),
    rx(nrx),
    smallGreenCube(":/icons/smallGreenCube.png"),
    greenCube(":/icons/greenCube.png"), redCube(":/icons/redCube.png"),
    orangeCube(":/icons/orangeCube.png"), pinkCube(":/icons/pinkCube.png"),
    vecpListItems(), iColStationID(1), iWidthColStationID(0)
{
    setupUi(this);

    /* Load settings */
    LoadSettings();

    /* Set help text for the controls */
    AddWhatsThisHelp();

    /* Clear list box for file names and set up columns */
    ListViewStations->clear();

    connect(actionSave,  SIGNAL(triggered()), this, SLOT(OnSave()));

    previewMapper = new QSignalMapper(this);
    previewGroup = new QActionGroup(this);
    showMapper = new QSignalMapper(this);
    showGroup = new QActionGroup(this);
    showGroup->addAction(actionShowOnlyActiveStations);
    showMapper->setMapping(actionShowOnlyActiveStations, 0);
    showGroup->addAction(actionShowAllStations);
    showMapper->setMapping(actionShowAllStations, 1);
    connect(actionClose, SIGNAL(triggered()), SLOT(close()));
    connect(actionShowAllStations, SIGNAL(triggered()), showMapper, SLOT(map()));
    connect(actionShowOnlyActiveStations, SIGNAL(triggered()), showMapper, SLOT(map()));
    connect(showMapper, SIGNAL(mapped(int)), this, SLOT(OnShowStationsMenu(int)));
    previewGroup->addAction(actionDisabled);
    previewMapper->setMapping(actionDisabled, 0);
    previewGroup->addAction(action5minutes);
    previewMapper->setMapping(action5minutes, NUM_SECONDS_PREV_5MIN);
    previewGroup->addAction(action15minutes);
    previewMapper->setMapping(action15minutes, NUM_SECONDS_PREV_15MIN);
    previewGroup->addAction(action30minutes);
    previewMapper->setMapping(action30minutes, NUM_SECONDS_PREV_30MIN);
    connect(actionDisabled, SIGNAL(triggered()), previewMapper, SLOT(map()));
    connect(action5minutes, SIGNAL(triggered()), previewMapper, SLOT(map()));
    connect(action15minutes, SIGNAL(triggered()), previewMapper, SLOT(map()));
    connect(action30minutes, SIGNAL(triggered()), previewMapper, SLOT(map()));
    connect(previewMapper, SIGNAL(mapped(int)), this, SLOT(OnShowPreviewMenu(int)));

    connect(buttonOk,  SIGNAL(clicked()), this, SLOT(close()));
    //connect(actionGetUpdate, SIGNAL(triggered()), this, SLOT(OnGetUpdate()));

    /* Connections ---------------------------------------------------------- */
    connect(&TimerList, SIGNAL(timeout()), this, SLOT(OnTimerList()));
    connect(&TimerUTCLabel, SIGNAL(timeout()), this, SLOT(OnTimerUTCLabel()));

    /* Check boxes */
    connect(CheckBoxFreeze, SIGNAL(clicked()), this, SLOT(OnCheckFreeze()));

    /* Init UTC time shown with a label control */
    OnTimerUTCLabel();
}

LiveScheduleDlg::~LiveScheduleDlg()
{
}

void
LiveScheduleDlg::LoadSettings()
{
    /* Set sorting behaviour of the list */
    iCurrentSortColumn = getSetting("sortcolumn", 0);
    bCurrentSortAscending = getSetting("sortascending", true);
    ListViewStations->sortItems(iCurrentSortColumn, bCurrentSortAscending?Qt::AscendingOrder:Qt::DescendingOrder);

    /* Retrieve the setting saved into the .ini file */
    strCurrentSavePath = QString::fromUtf8(rx.GetParameters()->GetDataDirectory("AFS").c_str());

    /* and make sure it exists */
	CreateDirectories(strCurrentSavePath);

    /* Set stations in list view which are active right now */
    bool bShowAll = getSetting("showall", false);
    int iPrevSecs = getSetting("preview", 0);

    if(bShowAll)
        actionShowAllStations->setChecked(true);
    else
        actionShowOnlyActiveStations->setChecked(true);
    DRMSchedule.SetSecondsPreview(iPrevSecs);
    switch (iPrevSecs)
    {
    case NUM_SECONDS_PREV_5MIN:
        action5minutes->setChecked(true);
        break;

    case NUM_SECONDS_PREV_15MIN:
        action5minutes->setChecked(true);
        break;

    case NUM_SECONDS_PREV_30MIN:
        action30minutes->setChecked(true);
        break;

    default: /* case 0, also takes care of out of value parameters */
        actionDisabled->setChecked(true);
        break;
    }
}

void
LiveScheduleDlg::SaveSettings()
{
    /* Store preview settings */
    putSetting("preview", DRMSchedule.GetSecondsPreview());

    /* Store sort settings */
    putSetting("sortcolumn", iCurrentSortColumn);
    putSetting("sortascending", bCurrentSortAscending);

    /* Store preview settings */
    putSetting("showall", showAll());
}

void
LiveScheduleDlg::OnCheckFreeze()
{
    /* if CheckBoxFreeze is checked the schedule is frozen */
    if (CheckBoxFreeze->isChecked())
        TimerList.stop();
    else
    {
        OnTimerList();
        TimerList.start(GUI_TIMER_LIST_VIEW_UPDATE);	/* Stations list */
    }
}

int LiveScheduleDlg::currentSortColumn()
{
	return ListViewStations->sortColumn();
}

bool LiveScheduleDlg::showAll()
{
	return actionShowAllStations->isChecked();
}

void
LiveScheduleDlg::OnTimerUTCLabel()
{
    /* Get current UTC time */
    time_t ltime;
    time(&ltime);
    struct tm *gmtCur = gmtime(&ltime);

    /* Generate time in format "UTC 12:00" */
    QString strUTCTime = QString().sprintf("%02d:%02d UTC",
                                           gmtCur->tm_hour, gmtCur->tm_min);

    /* Only apply if time label does not show the correct time */
    if (TextLabelUTCTime->text().compare(strUTCTime))
        TextLabelUTCTime->setText(strUTCTime);
}

void
LiveScheduleDlg::OnShowStationsMenu(int iID)
{
    /* Update list view */
    SetStationsView();
    (void)iID;
}

void
LiveScheduleDlg::OnShowPreviewMenu(int iID)
{
    DRMSchedule.SetSecondsPreview(iID);
}

void
LiveScheduleDlg::OnTimerList()
{
    CParameter& Parameters = *rx.GetParameters();

    Parameters.Lock();
    /* Get current receiver latitude and longitude if defined */
    if (Parameters.gps_data.set&LATLON_SET)
    {
        DRMSchedule.SetReceiverCoordinates(
		Parameters.gps_data.fix.latitude, Parameters.gps_data.fix.longitude);
    }
    Parameters.Unlock();

    /* Update schedule and list view */
    LoadSchedule();
}

QString
MyListLiveViewItem::key(int column, bool ascending) const
{
    /* Reimplement "key()" function to get correct sorting behaviour */

    const float fFreq = text(column).toFloat();

    if (column == COL_FREQ)
    {
        /* These columns are filled with numbers. Some items may have numbers
           after the decimal, therefore multiply with 10000 (which moves the
           numbers in front of the comma). Afterwards append zeros at the
           beginning so that positive integer numbers are sorted correctly */
        return QString().setNum(long(fFreq * 10000.0)).rightJustified(20, '0');
    }
    else
    {
        /* is a text column */
        /* sort the column and then sort for frequency */
        float d = 0.0;

        if (!ascending)
            d = 100000.0;

	const QString sFreq = QString().setNum(long((fFreq - d) * 10000.0)).rightJustified(20, '0');
        return text(column).toLower() + "|" + sFreq;
    }
}

void
LiveScheduleDlg::LoadSchedule()
{
    /* Lock mutex for modifying the vecpListItems */
    ListItemsMutex.lock();

    /* save the state of the station id column in case we want it later */
    iWidthColStationID = ListViewStations->columnWidth(iColStationID);

    /* Delete all old list view items (it is important that the vector
       "vecpListItems" was initialized to 0 at creation of the global object
       otherwise this may cause an segmentation fault) */
    for (size_t i = 0; i < vecpListItems.size(); i++)
    {
        if (vecpListItems[i] != nullptr)
            delete vecpListItems[i];
    }
    vecpListItems.clear();

    CParameter& Parameters = *rx.GetParameters();
    Parameters.Lock();
    DRMSchedule.LoadAFSInformations(Parameters.AltFreqSign);
    Parameters.Unlock();

    /* Init vector for storing the pointer to the list view items */
    const int iNumStations = DRMSchedule.GetStationNumber();

    vecpListItems.resize(iNumStations, nullptr);

    actionSave->setEnabled(iNumStations > 0);
    /* Unlock BEFORE calling the stations view update because in this function
       the mutex is locked, too! */
    ListItemsMutex.unlock();

    /* Update list view */
    SetStationsView();

    QString strTitle = tr("Live Schedule");

    if (iNumStations > 0)
    {
        Parameters.Lock();
        /* Get current service */
        const int iCurSelAudioServ =
            Parameters.GetCurSelAudioService();

        if (Parameters.Service[iCurSelAudioServ].IsActive())
        {
            /* Do UTF-8 to string conversion with the label strings */
            QString strStationName = QString().fromUtf8(
				Parameters.Service[iCurSelAudioServ].strLabel.c_str()
				);

            /* add station name on the title of the dialog */
            if (strStationName != "")
                strTitle += " [" + strStationName.trimmed() + "]";
        }
        Parameters.Unlock();
    }

	setWindowTitle(strTitle);
}

void
LiveScheduleDlg::eventClose(QCloseEvent*)
{
    /* Save settings */
    SaveSettings();
}

void
LiveScheduleDlg::eventHide(QHideEvent*)
{
    /* Deactivate real-time timers */
    TimerList.stop();
    TimerUTCLabel.stop();
}

void
LiveScheduleDlg::eventShow(QShowEvent*)
{
    /* Update window */
    OnTimerUTCLabel();
    TimerUTCLabel.start(GUI_TIMER_UTC_TIME_LABEL);

    if (!CheckBoxFreeze->isChecked())
    {
        OnTimerList();

        /* Activate real-time timer when window is shown */
        TimerList.start(GUI_TIMER_LIST_VIEW_UPDATE);	/* Stations list */
    }
}

void
LiveScheduleDlg::SetStationsView()
{
    /* Set lock because of list view items. These items could be changed
       by another thread */
    CParameter& Parameters = *rx.GetParameters();
    Parameters.Lock();
    int sNo = Parameters.GetCurSelAudioService();
    string thisServiceLabel = Parameters.Service[sNo].strLabel;
    Parameters.Unlock();

    ListItemsMutex.lock();

    const int iNumStations = DRMSchedule.GetStationNumber();

    bool bListHastChanged = false;

    bool bHaveOtherServiceIDs = false;

    /* Add new item for each station in list view */
    for (int i = 0; i < iNumStations; i++)
    {
        if (!((showAll() == false) &&
                (DRMSchedule.CheckState(i) == CDRMLiveSchedule::IS_INACTIVE)))
        {
            /* Only insert item if it is not already in the list */
            if (vecpListItems[i] == nullptr)
            {
                /* Generate new list item with all necessary column entries */
                const CLiveScheduleItem& item = DRMSchedule.GetItem(i);
                QString name = "";

                if(item.iServiceID != SERV_ID_NOT_USED)
                {
                    bHaveOtherServiceIDs = true;

                    Parameters.Lock();
                    map <uint32_t,CServiceInformation>::const_iterator
                    si = Parameters.ServiceInformation.find(item.iServiceID);
                    if(si != Parameters.ServiceInformation.end())
                        name = QString::fromUtf8(si->second.label.begin()->c_str());
                    else
                    {
                        ulong sid = item.iServiceID;
                        name = QString("(%1)").arg(sid, 0, 16);
                    }
                    Parameters.Unlock();
                }

                vecpListItems[i] = new MyListLiveViewItem(ListViewStations,
                        QString(item.strFreq.c_str()) /* freq. */ ,
                        name /* station name or id or blank */ ,
                        QString(item.strSystem.c_str()) /* system */ ,
                        ExtractTime(item.Schedule) /* time */,
                        QString(item.strTarget.c_str()) /* target */ ,
                        ExtractDaysFlagString(item.Schedule.iDayCode) /* Show list of days */
                                                         );

                /* Set flag for sorting the list */
                bListHastChanged = true;
            }

            /* If receiver coordinates are within the target area add a little green cube */
            if (DRMSchedule.GetItem(i).bInsideTargetArea)
                vecpListItems[i]->setIcon(COL_TARGET, smallGreenCube);

            /* Check, if station is currently transmitting. If yes, set
               special pixmap */
            switch (DRMSchedule.CheckState(i))
            {
            case CDRMLiveSchedule::IS_ACTIVE:
                vecpListItems[i]->setIcon(COL_FREQ, greenCube);
                break;
            case CDRMLiveSchedule::IS_PREVIEW:
                vecpListItems[i]->setIcon(COL_FREQ, orangeCube);
                break;
            case CDRMLiveSchedule::IS_SOON_INACTIVE:
                vecpListItems[i]->setIcon(COL_FREQ, pinkCube);
                break;
            case CDRMLiveSchedule::IS_INACTIVE:
                vecpListItems[i]->setIcon(COL_FREQ, redCube);
                break;
            default:
                vecpListItems[i]->setIcon(COL_FREQ, redCube);
                break;
            }
        }
        else
        {
            /* Delete this item since it is not used anymore */
            if (vecpListItems[i] != nullptr)
            {
                /* If one deletes a menu item in QT list view, it is
                   automaticall removed from the list and the list gets
                   repainted */
                delete vecpListItems[i];

                /* Reset pointer so we can distinguish if it is used or not */
                vecpListItems[i] = nullptr;

                /* Set flag for sorting the list */
                bListHastChanged = true;
            }
        }
    }


    if(bHaveOtherServiceIDs)
    {
        ListViewStations->setColumnWidth(iColStationID, iWidthColStationID);
    }
    else
    {
        ListViewStations->setColumnWidth(iColStationID, 0);
    }

    /* Sort the list if items have changed */
    if(bListHastChanged)
        ListViewStations->sortItems(iCurrentSortColumn, bCurrentSortAscending?Qt::AscendingOrder:Qt::DescendingOrder);
    ListItemsMutex.unlock();
}

void
LiveScheduleDlg::OnHeaderClicked(int c)
{
    /* Store the "direction" of sorting */
    if (iCurrentSortColumn == c)
        bCurrentSortAscending = !bCurrentSortAscending;
    else
        bCurrentSortAscending = true;

    iCurrentSortColumn = c;
}

QString
ColValue(const QString strValue)
{
    if (strValue == "")
        return "&nbsp;";
    else
        return strValue;
}

void
LiveScheduleDlg::OnSave()
{
    QString strFilename;
    QString strSchedule = "";
    QString strValue = "";

    CParameter& Parameters = *rx.GetParameters();

    Parameters.Lock();

    const int iCurSelAudioServ =
        Parameters.GetCurSelAudioService();
    /* Do UTF-8 to QString (UNICODE) conversion with the station name strings */
    QString strStationName =
        QString().fromUtf8(Parameters.Service[iCurSelAudioServ].strLabel.c_str());

    Parameters.Unlock();

    /* Lock mutex for use the vecpListItems */
    ListItemsMutex.lock();

    /* Force the sort for all items */
	ListViewStations->sortItems(iCurrentSortColumn, bCurrentSortAscending?Qt::AscendingOrder:Qt::DescendingOrder);

    /* Extract values from the list */

	for(int i=0; i<ListViewStations->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* myItem = ListViewStations->topLevelItem(i);
        strSchedule += "<tr>" "<td align=\"right\">" + myItem->text(COL_FREQ) + "</td>"	/* freq */
                       "<td>" + ColValue(myItem->text(1)) + "</td>"	/* system */
                       "<td>" + ColValue(myItem->text(2)) + "</td>"	/* time */
                       "<td>" + ColValue(myItem->text(3)) + "</td>"	/* target */
                       "<td>" + ColValue(myItem->text(4)) + "</td>"	/* days */
                       "</tr>\n";
	}

    ListItemsMutex.unlock();

    if (strSchedule != "")
    {
        /* Save to file current schedule  */
        QString strTitle(tr("AFS Live Schedule"));
        QString strItems("");

        /* Prepare HTML page for storing the content */
        QString strText = "<html>\n<head>\n"
                          "<meta http-equiv=\"content-Type\" "
                          "content=\"text/html; charset=utf-8\">\n<title>"
                          + strStationName + " - " + strTitle +
                          "</title>\n</head>\n\n<body>\n"
                          "<h4>" + strTitle + "</h4>"
                          "<h3>" + strStationName + "</h3>"
                          "\n<table border=\"1\"><tr>\n"
                          "<th>" + tr("Frequency [kHz/MHz]") + "</th>"
                          "<th>" + tr("System") + "</th>"
                          "<th>" + tr("Time [UTC]") + "</th>"
                          "<th>" + tr("Target") + "</th>"
                          "<th>" + tr("Start day") + "</th>\n"
                          "</tr>\n" + strSchedule + "</table>\n"
                          /* Add current date and time */
                          "<br><p align=right><font size=-2><i>" +
                          QDateTime().currentDateTime().toString() + "</i></font></p>"
                          "</body>\n</html>";

        QString strPath = strCurrentSavePath +
                          strStationName + "_" + "LiveSchedule.html";
        strFilename = QFileDialog::getSaveFileName(this, "*.html", strPath);

        if (!strFilename.isEmpty())
        {
            /* Save as a text stream */
            QFile FileObj(strFilename);

            if (FileObj.open(QIODevice::WriteOnly))
            {
                QTextStream TextStream(&FileObj);
                TextStream << strText;	/* Actual writing */
                FileObj.close();
                strCurrentSavePath = QFileInfo(strFilename).path() + "/";
            }
        }
    }
}

void
LiveScheduleDlg::AddWhatsThisHelp()
{
    /* Stations List */
	QString strView =
                     tr("<b>Live Schedule List:</b> In the live schedule list "
                        "it's possible to view AFS (Alternative Frequency Signalling) "
                        "information transmitted with the current DRM or AMSS signal.</b>"
                        "It is possible to limit the view to active stations by changing a "
                        "setting in the 'view' menu.<br>"
                        "The color of the cube on the left of the "
                        "frequency shows the current status of the transmission.<br>"
                        "A green box shows that the transmission takes place right now "
                        "a red cube it is shown that the transmission is offline, "
                        "a pink cube shown that the transmission soon will be offline.<br>"
                        "If the stations preview is active an orange box shows the stations "
                        "that will be active.<br>"
                        "A little green cube on the left of the target column shows that the receiver"
                        " coordinates (latitude and longitude) stored into Dream settings are within"
                        " the target area of this transmission.<br>"
                        "The list can be sorted by clicking on the headline of the column.");

    /* UTC time label */
	QString strTime =
                     tr("<b>UTC Time:</b> Shows the current Coordinated "
                        "Universal Time (UTC) which is also known as Greenwich Mean Time "
                        "(GMT).");

    /* Check box freeze */
	QString strFreeze = tr("<b>Freeze:</b> If this check box is selected the live schedule is frozen.");
	ListViewStations->setWhatsThis(strView);
    TextLabelUTCTime->setWhatsThis(strTime);
    CheckBoxFreeze->setWhatsThis(strFreeze);
}

CDRMLiveSchedule::StationState CDRMLiveSchedule::CheckState(const int iPos)
{
    /* Get system time */
    time_t
    ltime;
    time(&ltime);

    if (IsActive(iPos, ltime))
    {
        /* Check if the station soon will be inactive */
        if (IsActive(iPos, ltime + NUM_SECONDS_SOON_INACTIVE))
            return IS_ACTIVE;
        else
            return IS_SOON_INACTIVE;
    }
    else
    {
        /* Station is not active, check preview condition */
        if (iSecondsPreview > 0)
        {
            if (IsActive(iPos, ltime + iSecondsPreview))
                return IS_PREVIEW;
            else
                return IS_INACTIVE;
        }
        else
            return IS_INACTIVE;
    }
}

bool
CDRMLiveSchedule::IsActive(const int iPos, const time_t ltime)
{
    return StationsTable[iPos].IsActive(ltime);
}

bool
CLiveScheduleItem::IsActive(const time_t ltime)
{
    return Schedule.IsActive(ltime);
}
