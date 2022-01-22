/******************************************************************************\
* Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
* Copyright (c) 2004
*
* Author(s):
*	Volker Fischer, Stephane Fillod, Tomi Manninen, Andrea Russo,
*      Julian Cable
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

#include "StationsDlg.h"
#include "../tables/TableStations.h"
#include "DialogUtil.h"
#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
# include "RigDlg.h"
#endif
#include <QHideEvent>
#include <QShowEvent>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QFileInfo>
#include <cmath>

/* Implementation *************************************************************/
#ifdef HAVE_LIBHAMLIB
StationsDlg::StationsDlg(CRx& nrx, CSettings& Settings, CRig& Rig, QMap<QWidget*,QString>& parents):
CWindow(parents, Settings, "Stations"),
rx(nrx), Rig(Rig),
#else
StationsDlg::StationsDlg(CRx& rx, CSettings& Settings, QMap<QWidget*,QString>& parents):
CWindow(parents, Settings, "Stations"),
rx(rx),
#endif
greenCube(":/icons/greenCube.png"), redCube(":/icons/redCube.png"),
orangeCube(":/icons/orangeCube.png"), pinkCube(":/icons/pinkCube.png"),
bReInitOnFrequencyChange(false)
{
	setupUi(this);
#if QWT_VERSION < 0x060100
    ProgrSigStrength->setScalePosition(QwtThermo::TopScale);
#else
    ProgrSigStrength->setScalePosition(QwtThermo::TrailingScale);
#endif

	/* Load settings */
	LoadSettings();

	/* Set help text for the controls */
	AddWhatsThisHelp();

	ProgrSigStrength->hide();
	TextLabelSMeter->hide();

	/* Set up frequency selector control (QWTCounter control) */
#if QWT_VERSION < 0x060100
	QwtCounterFrequency->setRange(0.0, MAX_RF_FREQ, 1.0);
#else
	QwtCounterFrequency->setRange(0.0, MAX_RF_FREQ);
	QwtCounterFrequency->setSingleStep(1.0);
#endif
	QwtCounterFrequency->setNumButtons(3); /* Three buttons on each side */
	QwtCounterFrequency->setIncSteps(QwtCounter::Button1, 1); /* Increment */
	QwtCounterFrequency->setIncSteps(QwtCounter::Button2, 10);
	QwtCounterFrequency->setIncSteps(QwtCounter::Button3, 100);

	ListViewStations->setAllColumnsShowFocus(true);
	ListViewStations->setColumnCount(9);
	ListViewStations->setRootIsDecorated ( false );
	ListViewStations->setSortingEnabled ( true );
	QStringList headers;
	headers
		<< QString() /* icon, enable sorting by online/offline */
		<< tr("Station Name")
		<< tr("Time [UTC]")
		<< tr("Frequency [kHz]")
		<< tr("Power [kW]")
		<< tr("Target")
		<< tr("Country")
		<< tr("Site")
		<< tr("Language")
		<< tr("Days");
	ListViewStations->setHeaderLabels(headers);
	ListViewStations->headerItem()->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	ListViewStations->headerItem()->setTextAlignment(3, Qt::AlignRight | Qt::AlignVCenter);
	ListViewStations->headerItem()->setTextAlignment(4, Qt::AlignRight | Qt::AlignVCenter);

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
	connect(ListViewStations->header(), SIGNAL(sectionClicked(int)), this, SLOT(OnHeaderClicked(int)));

	//connect(actionGetUpdate, SIGNAL(triggered()), this, SLOT(OnGetUpdate()));
# ifdef HAVE_LIBHAMLIB
	RigDlg *pRigDlg = new RigDlg(Rig, this);
	connect(actionChooseRig, SIGNAL(triggered()), pRigDlg, SLOT(show()));
# else
	actionChooseRig->setEnabled(false);
# endif
	connect(buttonOk, SIGNAL(clicked()), this, SLOT(close()));
	connect(actionEnable_S_Meter, SIGNAL(triggered()), this, SLOT(OnSMeterMenu()));

	/* Init progress bar for input s-meter */
	InitSMeter(this, ProgrSigStrength);

	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnUrlFinished(QNetworkReply*)));

	/* Connections ---------------------------------------------------------- */

	connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
	connect(QwtCounterFrequency, SIGNAL(valueChanged(double)),
		this, SLOT(OnFreqCntNewValue(double)));

	okMessage = tr("Update successful.");
	badMessage =
		tr("Update failed. The following things may caused the "
		"failure:\n"
		"\t- the internet connection was not set up properly\n"
		"\t- the server is currently not available\n"
		"\t- the file 'schedule.ini' could not be written");
}

StationsDlg::~StationsDlg()
{
}

void StationsDlg::OnShowStationsMenu(int iID)
{
	(void)iID;
    UpdateTransmissionStatus();
}

void StationsDlg::OnShowPreviewMenu(int iID)
{
	schedule.SetSecondsPreview(iID);
    UpdateTransmissionStatus();
}

void StationsDlg::on_ComboBoxFilterTarget_activated(const QString& s)
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		schedule.targetFilterdrm = s;
	else
		schedule.targetFilteranalog = s;
    UpdateTransmissionStatus();
}

void StationsDlg::on_ComboBoxFilterCountry_activated(const QString& s)
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		schedule.countryFilterdrm = s;
	else
		schedule.countryFilteranalog = s;
    UpdateTransmissionStatus();
}

void StationsDlg::on_ComboBoxFilterLanguage_activated(const QString& s)
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		schedule.languageFilterdrm = s;
	else
		schedule.languageFilteranalog = s;
    UpdateTransmissionStatus();
}

void StationsDlg::on_actionGetUpdate_triggered()
{
	const bool bDrmMode = schedule.GetSchedMode() == CSchedule::SM_DRM;
	QUrl& qurl(bDrmMode ? schedule.qurldrm : schedule.qurlanalog);
	if (QMessageBox::information(this, tr("Dream Schedule Update"),
		tr("Dream tries to download the newest schedule\n"
		"Your computer must be connected to the internet.\n\n"
		"The current file will be overwritten.\n"
		"Do you want to continue?"),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes )
	{
		/* Try to download the current schedule. Copy the file to the
		current working directory (which is "QDir().absFilePath(nullptr)") */
		manager->get(QNetworkRequest(qurl));
	}
}

void StationsDlg::OnUrlFinished(QNetworkReply* reply)
{
	if(reply->error()==QNetworkReply::NoError)
	{
		QFile f(schedule.schedFileName);
		if(f.open(QIODevice::WriteOnly)) {
			f.write(reply->readAll());
			f.close();
			/* Notify the user that update was successful */
			QMessageBox::information(this, "Dream", okMessage, QMessageBox::Ok);
            /* Read file */
            LoadSchedule();
            LoadScheduleView();
            UpdateTransmissionStatus();
        } else {
			QMessageBox::information(this, "Dream", tr("Can't save new schedule"), QMessageBox::Ok);
		}
	}
	else
	{
		QMessageBox::information(this, "Dream", badMessage, QMessageBox::Ok);
	}
}

void StationsDlg::LoadSchedule()
{
    schedule.LoadSchedule();
    /* add last update information on menu item */
    QFileInfo f = QFileInfo(schedule.schedFileName);
    actionGetUpdate->setText(
       tr("&Get Update (last update: %1)...").arg(f.lastModified().date().toString())
    );
}

void StationsDlg::eventClose(QCloseEvent*)
{
	/* Save settings */
	SaveSettings();
}

void StationsDlg::eventHide(QHideEvent*)
{
	/* Deactivate real-time timers */
	Timer.stop();
	DisableSMeter();
}

void StationsDlg::eventShow(QShowEvent*)
{
    QwtCounterFrequency->setValue(rx.GetFrequency());
	bool ensmeter = false;
	ensmeter = actionEnable_S_Meter->isChecked();
	if(ensmeter)
		EnableSMeter();
	else
		DisableSMeter();
    CSchedule::ESchedMode eSchedM = schedule.GetSchedMode();
    ERecMode eRecM = rx.GetReceiverMode();
    bool bIsDRM = eRecM == RM_DRM;
    if (
        (eSchedM == CSchedule::SM_DRM && !bIsDRM)
    ||
        (eSchedM == CSchedule::SM_ANALOG && bIsDRM)
    )
    {
        /* Store previous columns settings */
        if (eSchedM != CSchedule::SM_NONE)
        {
            ColumnParamToStr(bIsDRM?strColumnParamdrm:strColumnParamanalog);
        }
        // change mode
        schedule.SetSchedMode(bIsDRM?CSchedule::SM_DRM:CSchedule::SM_ANALOG);
        /* Restore columns settings */
        ColumnParamFromStr(bIsDRM?strColumnParamdrm:strColumnParamanalog);
    }
    if(schedule.GetNumberOfStations()==0)
	{
        QString s = "";
        if(QFile::exists(schedule.schedFileName))
		{
            LoadSchedule();
        }
        else
        {
            QMessageBox::information(this, "Dream", tr("The schedule file "
            " could not be found or contains no data.\n"
            "No stations can be displayed.\n"
            "Try to download this file by using the 'Update' menu."),
            QMessageBox::Ok);
            actionGetUpdate->setText(tr("&Get Update ..."));
        }
    }
    LoadScheduleView();
    UpdateTransmissionStatus();
    /* Activate real-time timer when window is shown */
    Timer.start(500 /* twice a second - nyquist for catching minute boundaries */);
    QTimer::singleShot(1000, this, SLOT(OnTimer()));
}

void StationsDlg::OnTimer()
{
	/* Get current UTC time */
	time_t ltime;
	time(&ltime);
	struct tm* gmtCur = gmtime(&ltime);

	/* Generate time in format "UTC 12:00" */
	QString strUTCTime = QString().sprintf("%02d:%02d UTC",
		gmtCur->tm_hour, gmtCur->tm_min);

	/* Only apply if time label does not show the correct time */
	if (TextLabelUTCTime->text().compare(strUTCTime))
		TextLabelUTCTime->setText(strUTCTime);

	/* frequency could be changed by evaluation dialog or RSCI */
    int iFrequency = rx.GetFrequency();
	int iCurFrequency = QwtCounterFrequency->value();

	if (iFrequency != iCurFrequency)
	{
		QwtCounterFrequency->setValue(iFrequency);
	}

	/* reload schedule on minute boundaries */
	if (ltime % 60 == 0)
	{
        UpdateTransmissionStatus();
	}
}

void StationsDlg::LoadSettings()
{
	/* S-meter settings */
	bool ensmeter = Settings.Get("Hamlib", "ensmeter", false);

	actionEnable_S_Meter->setChecked(ensmeter);

	bool showAll = getSetting("showall", false);
	int iPrevSecs = getSetting("preview", NUM_SECONDS_PREV_5MIN);
	schedule.SetSecondsPreview(iPrevSecs);

	if(showAll)
		actionShowAllStations->setChecked(true);
	else
		actionShowOnlyActiveStations->setChecked(true);

	switch (iPrevSecs)
	{
	case NUM_SECONDS_PREV_5MIN:
		action5minutes->setChecked(true);
		break;

	case NUM_SECONDS_PREV_15MIN:
		action15minutes->setChecked(true);
		break;

	case NUM_SECONDS_PREV_30MIN:
		action30minutes->setChecked(true);
		break;

	default: /* case 0, also takes care of out of value parameters */
		actionDisabled->setChecked(true);
		break;
	}

	/* get sorting and filtering behaviour */
    ERecMode eRecSM = rx.GetReceiverMode();
	switch (eRecSM)
	{
	case RM_DRM:
		schedule.SetSchedMode(CSchedule::SM_DRM);
		break;

	case RM_AM:
		schedule.SetSchedMode(CSchedule::SM_ANALOG);
		break;
	default: // can't happen!
		;
	}
	iSortColumndrm = getSetting("sortcolumndrm", 0);
	bCurrentSortAscendingdrm = getSetting("sortascendingdrm", true);
	strColumnParamdrm = getSetting("columnparamdrm", QString());
	iSortColumnanalog = getSetting("sortcolumnanalog", 0);
	bCurrentSortAscendinganalog = getSetting("sortascendinganalog", true);
	strColumnParamanalog = getSetting("columnparamanalog", QString());

	schedule.qurldrm = QUrl(getSetting("DRM URL", QString(DRM_SCHEDULE_URL)));
	schedule.targetFilterdrm = getSetting("targetfilterdrm", QString());
	schedule.countryFilterdrm = getSetting("countryfilterdrm", QString());
	schedule.languageFilterdrm = getSetting("languagefilterdrm", QString());
	schedule.targetFilteranalog = getSetting("targetfilteranalog", QString());
	schedule.countryFilteranalog = getSetting("countryfilteranalog", QString());
	schedule.languageFilteranalog = getSetting("languagefilteranalog", QString());
}

void StationsDlg::SaveSettings()
{
	Settings.Put("Hamlib", "ensmeter", actionEnable_S_Meter->isChecked());
	putSetting("showall", actionShowAllStations->isChecked());
	putSetting("DRM URL", schedule.qurldrm.toString());
	putSetting("ANALOG URL", schedule.qurlanalog.toString());
	putSetting("sortcolumndrm", iSortColumndrm);
	putSetting("sortascendingdrm", bCurrentSortAscendingdrm);
	putSetting("columnparamdrm", strColumnParamdrm);
	putSetting("sortcolumnanalog", iSortColumnanalog);
	putSetting("sortascendinganalog", bCurrentSortAscendinganalog);
	putSetting("columnparamanalog", strColumnParamanalog);
	putSetting("targetfilterdrm", schedule.targetFilterdrm);
	putSetting("countryfilterdrm", schedule.countryFilterdrm);
	putSetting("languagefilterdrm", schedule.languageFilterdrm);
	putSetting("targetfilteranalog", schedule.targetFilteranalog);
	putSetting("countryfilteranalog", schedule.countryFilteranalog);
	putSetting("languagefilteranalog", schedule.languageFilteranalog);

	/* Store preview settings */
	putSetting("preview", schedule.GetSecondsPreview());
}

void StationsDlg::LoadFilters()
{
	ComboBoxFilterTarget->clear();
	ComboBoxFilterCountry->clear();
	ComboBoxFilterLanguage->clear();
	ComboBoxFilterTarget->addItems(schedule.ListTargets);
	ComboBoxFilterCountry->addItems(schedule.ListCountries);
	ComboBoxFilterLanguage->addItems(schedule.ListLanguages);

	QString targetFilter,countryFilter,languageFilter;
	if(schedule.GetSchedMode()==CSchedule::SM_DRM)
	{
		targetFilter=schedule.targetFilterdrm;
		countryFilter=schedule.countryFilterdrm;
		languageFilter=schedule.languageFilterdrm;
	}
	else
	{
		targetFilter=schedule.targetFilteranalog;
		countryFilter=schedule.countryFilteranalog;
		languageFilter=schedule.languageFilteranalog;
	}
	ComboBoxFilterTarget->setCurrentIndex(ComboBoxFilterTarget->findText(targetFilter));
	ComboBoxFilterCountry->setCurrentIndex(ComboBoxFilterCountry->findText(countryFilter));
	ComboBoxFilterLanguage->setCurrentIndex(ComboBoxFilterLanguage->findText(languageFilter));
}

void StationsDlg::LoadScheduleView()
{
    ListViewStations->clear();
    for (int i = 0; i < schedule.GetNumberOfStations(); i++)
	{
		const CStationsItem& station = schedule.GetItem(i);

		/* Get power of the station. We have to do a special treatment
		* here, because we want to avoid having a "0" in the list when
		* a "?" was in the schedule-ini-file */
		const _REAL rPower = station.rPower;

		QString strPower;
		if (rPower == (_REAL) 0.0)
			strPower = "?";
		else
			strPower.setNum(rPower);

		QString strTimes = QString().sprintf("%04d-%04d", station.StartTime(), station.StopTime());

		/* Generate new list station with all necessary column entries */
		QTreeWidgetItem* item = new CaseInsensitiveTreeWidgetItem(ListViewStations);
		item->setText(1, station.strName);
		item->setText(2, strTimes /* time */);
		item->setText(3, QString().setNum(station.iFreq) /* freq. */);
		item->setText(4, strPower            /* power */);
		item->setText(5, station.strTarget   /* target */);
		item->setText(6, station.strCountry  /* country */);
		item->setText(7, station.strSite     /* site */);
		item->setText(8, station.strLanguage /* language */);
		item->setText(9, station.strDaysShow);
		item->setData(1, Qt::UserRole, i);
		item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
		item->setTextAlignment(3, Qt::AlignRight | Qt::AlignVCenter);
		item->setTextAlignment(4, Qt::AlignRight | Qt::AlignVCenter);
	}
	int c;
	bool b;
	if(schedule.GetSchedMode()==CSchedule::SM_DRM)
	{
		b = bCurrentSortAscendingdrm;
		c = iSortColumndrm;
	}
	else
	{
		b = bCurrentSortAscendinganalog;
		c = iSortColumnanalog;
	}
	ListViewStations->sortByColumn(c, b ? Qt::AscendingOrder : Qt::DescendingOrder);
	LoadFilters();
}

void StationsDlg::UpdateTransmissionStatus()
{
	Timer.stop();
	ListItemsMutex.lock();

	bool bShowAll = showAll();
	ListViewStations->setSortingEnabled(false);
    for (int i = 0; i < ListViewStations->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = ListViewStations->topLevelItem(i);
		int scheduleItem = item->data(1, Qt::UserRole).toInt();

		Station::EState iState = schedule.GetState(scheduleItem);

		switch (iState)
		{
		case Station::IS_ACTIVE:
			item->setData(0, Qt::UserRole, 1);
			item->setIcon(0, greenCube);
			break;
		case Station::IS_PREVIEW:
			item->setData(0, Qt::UserRole, 2);
			item->setIcon(0, orangeCube);
			break;
		case Station::IS_SOON_INACTIVE:
			item->setData(0, Qt::UserRole, 0);
			item->setIcon(0, pinkCube);
			break;
		case Station::IS_INACTIVE:
			item->setData(0, Qt::UserRole, 3);
			item->setIcon(0, redCube);
			break;
		default:
			item->setData(0, Qt::UserRole, 4);
			item->setIcon(0, redCube);
			break;
		}
		if(schedule.CheckFilter(scheduleItem) && (bShowAll || (iState != Station::IS_INACTIVE)))
		{
			item->setHidden(false);
		}
		else
		{
			item->setHidden(true);
		}
	}
	ListViewStations->setSortingEnabled(true);
	ListViewStations->sortItems(ListViewStations->sortColumn(), GetSortAscending()?Qt::AscendingOrder:Qt::DescendingOrder);
	ListViewStations->setFocus();
	ListItemsMutex.unlock();
	Timer.start();
}

void StationsDlg::OnFreqCntNewValue(double dVal)
{
    int iFrequency = rx.GetFrequency();
	int newFreq = int(dVal);

	if(newFreq != iFrequency) // avoid double tuning via RSCI
        rx.SetFrequency(int(dVal));
}

void StationsDlg::OnHeaderClicked(int c)
{
	/* Store the "direction" of sorting */
	if (currentSortColumn() == c)
		SetSortAscending(!GetSortAscending());
	else
		SetSortAscending(true);
	/* Store the column of sorting */
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		iSortColumndrm = c;
	else
		iSortColumnanalog = c;
}

int StationsDlg::currentSortColumn()
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		return iSortColumndrm;
	else
		return iSortColumnanalog;
}

void StationsDlg::SetSortAscending(bool b)
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		bCurrentSortAscendingdrm = b;
	else
		bCurrentSortAscendinganalog = b;
}

bool StationsDlg::GetSortAscending()
{
	if (schedule.GetSchedMode() == CSchedule::SM_DRM)
		return bCurrentSortAscendingdrm;
	else
		return bCurrentSortAscendinganalog;
}

void StationsDlg::ColumnParamFromStr(const QString& strColumnParam)
{
	QStringList list(strColumnParam.split(QChar('|')));
	const int n = list.count(); /* width and position */
	if (n == 2)
	{
		for (int j = 0; j < n; j++)
		{
			int c = ListViewStations->header()->count();
			QStringList values(list[j].split(QChar(',')));
			const int lc = (int)values.count();
			if (lc < c)
				c = lc;
			for (int i = 0; i < c; i++)
			{
				int v = values[i].toInt();
				if (!j) /* width*/
					ListViewStations->header()->resizeSection(i, v);
				else /* position */
					ListViewStations->header()->moveSection(ListViewStations->header()->visualIndex(i), v);
			}
		}
	}
	else
	{
		ListViewStations->header()->resizeSections(QHeaderView::ResizeToContents);
		ListViewStations->header()->resizeSections(QHeaderView::Interactive);
		ListViewStations->header()->resizeSection(0, ListViewStations->header()->minimumSectionSize());
	}
}

void StationsDlg::ColumnParamToStr(QString& strColumnParam)
{
	strColumnParam = "";
	const int n = 2; /* width and position */
	for (int j = 0; j < n; j++)
	{
		const int c = ListViewStations->header()->count();
		for (int i = 0; i < c; i++)
		{
			int v;
			if (!j) /* width*/
				v = ListViewStations->header()->sectionSize(i);
			else /* position */
				v = ListViewStations->header()->visualIndex(i);
			QString strValue;
			strValue.setNum(v);
			strColumnParam += strValue;
			if (i < (c-1))
				strColumnParam += ",";
		}
		if (j < (n-1))
			strColumnParam += "|";
	}
}

void StationsDlg::SetFrequencyFromGUI(int iFreq)
{
	QwtCounterFrequency->setValue(iFreq);

	/* If the mode has changed re-initialise the receiver */
    ERecMode eCurrentMode = rx.GetReceiverMode();

	/* if "bReInitOnFrequencyChange" is not true, initiate a reinit when
	schedule mode is different from receiver mode */
	switch (schedule.GetSchedMode())
	{
	case CSchedule::SM_NONE:
		break;

	case CSchedule::SM_DRM:
		if (eCurrentMode != RM_DRM)
            rx.SetReceiverMode(RM_DRM);
		if (bReInitOnFrequencyChange)
            rx.Restart();
		break;

	case CSchedule::SM_ANALOG:
		if (eCurrentMode != RM_AM)
            rx.SetReceiverMode(RM_AM);
		if (bReInitOnFrequencyChange)
            rx.Restart();
		break;
	}
}

void StationsDlg::on_ListViewStations_itemSelectionChanged()
{
	QList<QTreeWidgetItem *> items =  ListViewStations->selectedItems();
	if(items.size()==1)
	{
		SetFrequencyFromGUI(QString(items.first()->text(3)).toInt());
	}
}

void StationsDlg::OnSMeterMenu(int iID)
{
	(void)iID;
}

void StationsDlg::OnSMeterMenu()
{
	if(actionEnable_S_Meter->isChecked())
		EnableSMeter();
	else
		DisableSMeter();
}

void StationsDlg::EnableSMeter()
{
	TextLabelSMeter->setEnabled(true);
	ProgrSigStrength->setEnabled(true);
	TextLabelSMeter->show();
	ProgrSigStrength->show();
	emit subscribeRig();
}

void StationsDlg::DisableSMeter()
{
	TextLabelSMeter->hide();
	ProgrSigStrength->hide();
	emit unsubscribeRig();
}

void StationsDlg::OnSigStr(double rCurSigStr)
{
	ProgrSigStrength->setValue(rCurSigStr);
}

void StationsDlg::AddWhatsThisHelp()
{
	/* Stations List */
	QString strList =
		tr("<b>Stations List:</b> In the stations list "
		"view all DRM stations which are stored in the schedule.ini file "
		"are shown. It is possible to show only active stations by changing a "
		"setting in the 'view' menu. The color of the cube on the left of a "
		"menu item shows the current status of the DRM transmission. A green "
		"box shows that the transmission takes place right now, a "
		//           "yellow cube shows that this is a test transmission and with a "
		"red cube it is shown that the transmission is offline, "
		"a pink cube shown that the transmission soon will be offline.<br>"
		"If the stations preview is active an orange box shows the stations "
		"that will be active.<br>"
		"The list can be sorted by clicking on the headline of the "
		"column.<br>By clicking on a menu item, a remote front-end can "
		"be automatically switched to the current frequency and the "
		"Dream software is reset to a new acquisition (to speed up the "
		"synchronization process). Also, the log-file frequency edit "
		"is automatically updated.");

	/* Frequency Counter */
	QString strCounter =
		tr("<b>Frequency Counter:</b> The current frequency "
		"value can be changed by using this counter. The tuning steps are "
		"100 kHz for the  buttons with three arrows, 10 kHz for the "
		"buttons with two arrows and 1 kHz for the buttons having only "
		"one arrow. By keeping the button pressed, the values are "
		"increased / decreased automatically.");

	/* UTC time label */
	QString strTime =
		tr("<b>UTC Time:</b> Shows the current Coordinated "
		"Universal Time (UTC) which is also known as Greenwich Mean Time "
		"(GMT).");

	/* S-meter */
	const QString strSMeter =
		tr("<b>Signal-Meter:</b> Shows the signal strength "
		"level in dB relative to S9.<br>Note that not all "
		"front-ends controlled by hamlib support this feature. If the s-meter "
		"is not available, the controls are disabled.");

	ListViewStations->setWhatsThis(strList);
	QwtCounterFrequency->setWhatsThis(strCounter);
	TextLabelUTCTime->setWhatsThis(strTime);
	TextLabelSMeter->setWhatsThis(strSMeter);
	ProgrSigStrength->setWhatsThis(strSMeter);
}

bool StationsDlg::showAll()
{
	return actionShowAllStations->isChecked();
}
