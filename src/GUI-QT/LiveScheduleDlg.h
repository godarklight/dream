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

#ifndef __LiveScheduleDlg_H
#define __LiveScheduleDlg_H

#include "ui_LiveScheduleWindow.h"
#include "CWindow.h"
#include "../main-Qt/crx.h"
#include <QSignalMapper>
#include <QDialog>
#include <QTreeWidget>
#include <QPixmap>
#include <QRadioButton>
#include <QTimer>
#include <QMessageBox>
#include <QMenuBar>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QThread>
#include <vector>


/* Definitions ****************************************************************/
/* Define the timer interval of updating the list view */
#define GUI_TIMER_LIST_VIEW_UPDATE		5000 /* ms (5 seconds) */

/* Define the timer interval of updating the UTC label */
#define GUI_TIMER_UTC_TIME_LABEL		1000 /* ms (1 second) */

#define COL_FREQ 0 /* frequency column */
#define COL_TARGET 4 /* target column */

/* Time definitions for preview */
#define NUM_SECONDS_PREV_5MIN			300
#define NUM_SECONDS_PREV_15MIN			900
#define NUM_SECONDS_PREV_30MIN			1800

#define NUM_SECONDS_SOON_INACTIVE		600

/* String definitions for schedule days */
#define CHR_ACTIVE_DAY_MARKER			'1'

/* Classes ********************************************************************/
class CLiveScheduleItem
{
public:
	CLiveScheduleItem() : strFreq(""), strTarget(""), iServiceID(SERV_ID_NOT_USED),
	strSystem(""), bInsideTargetArea(false) {}

	bool IsActive(const time_t ltime);

	std::string		strFreq;
	std::string		strTarget;
	uint32_t	iServiceID;
	std::string		strSystem;
	bool	bInsideTargetArea;
	CAltFreqSched Schedule;
};

class CDRMLiveSchedule
{
public:
	CDRMLiveSchedule():StationsTable(),iSecondsPreview(0),
	dReceiverLatitude(0),dReceiverLongitude(0)
	{}

	virtual ~CDRMLiveSchedule() {}

	enum StationState {IS_ACTIVE, IS_INACTIVE, IS_PREVIEW, IS_SOON_INACTIVE};
	int GetStationNumber() {return StationsTable.size();}
	CLiveScheduleItem& GetItem(const int iPos) {return StationsTable[iPos];}
	StationState CheckState(const int iPos);

	void LoadAFSInformations(const CAltFreqSign& AltFreqSign);

	void LoadServiceDefinition(const CServiceDefinition& service,
			const CAltFreqSign& AltFreqSign, const uint32_t iServiceID=SERV_ID_NOT_USED);

	void DecodeTargets(const std::vector<CAltFreqRegion> vecAltFreqRegions,
		std::string& strRegions , bool& bIntoTargetArea);

	void SetSecondsPreview(int iSec) {iSecondsPreview = iSec;}
	int GetSecondsPreview() {return iSecondsPreview;}

    virtual void SetReceiverCoordinates(double latitude, double longitude);

protected:
	bool IsActive(const int iPos, const time_t ltime);

	std::vector<CLiveScheduleItem>	StationsTable;

	/* Minutes for stations preview in seconds if zero then no active */
	int			iSecondsPreview;

	/* receiver coordinates */
	double		dReceiverLatitude;
	double		dReceiverLongitude;
};

class MyListLiveViewItem : public QTreeWidgetItem
{
public:
	MyListLiveViewItem(QTreeWidget* parent, QString s1, QString s2 = QString::null,
		QString s3 = QString::null, QString s4 = QString::null,
		QString s5 = QString::null, QString s6 = QString::null,
		QString s7 = QString::null, QString s8 = QString::null) :	
	QTreeWidgetItem(parent, QStringList() << s1 << s2 << s3 << s4 << s5 << s6 << s7 << s8)
	{}

	/* Custom "key()" function for correct sorting behaviour */
	virtual QString key(int column, bool ascending) const;
	void setPixmap(int col, QPixmap p) { setIcon(col, p); }
};

class LiveScheduleDlg : public CWindow, public Ui_LiveScheduleWindow
{
	Q_OBJECT

public:
    LiveScheduleDlg(CRx&, CSettings&, QMap<QWidget*,QString>&);
	virtual ~LiveScheduleDlg();

protected:
	virtual void	eventClose(QCloseEvent* pEvent);
	virtual void	eventHide(QHideEvent* pEvent);
	virtual void	eventShow(QShowEvent* pEvent);
	void			LoadSettings();
	void			SaveSettings();
	void			LoadSchedule();
	int				iCurrentSortColumn;
	bool		bCurrentSortAscending;
	void			SetStationsView();
	void			AddWhatsThisHelp();
	void			SetUTCTimeLabel();
	QString			ExtractDaysFlagString(const int iDayCode);
	QString			ExtractTime(const CAltFreqSched& schedule);
	bool			showAll();
	int				currentSortColumn();

    CRx&            rx;
	CDRMLiveSchedule	DRMSchedule;
	QTimer			TimerList;
	QTimer			TimerUTCLabel;
	QIcon			smallGreenCube;
	QIcon			greenCube;
	QIcon			redCube;
	QIcon			orangeCube;
	QIcon			pinkCube;
	QSignalMapper*	previewMapper;
	QActionGroup*	previewGroup;
	QSignalMapper*	showMapper;
	QActionGroup*	showGroup;

	std::vector<MyListLiveViewItem*>	vecpListItems;
	QMutex			ListItemsMutex;
	QString			strCurrentSavePath;
	int				iColStationID;
	int				iWidthColStationID;

public slots:
	void OnTimerList();
	void OnTimerUTCLabel();
	void OnShowStationsMenu(int iID);
	void OnShowPreviewMenu(int iID);
	void OnHeaderClicked(int c);
	void OnSave();
	void OnCheckFreeze();
};

#endif
