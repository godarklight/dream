/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2013
 *
 * Author(s):
 *	Volker Fischer, Stephane Fillod, Tomi Manninen, Andrea Russo,Julian Cable
 *
 *
 * Description: DRM and AM Schedules
 *
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
#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QTreeWidgetItem>
#include "../util/Vector.h"

/* Definitions ****************************************************************/

#define DRM_SCHEDULE_URL	"http://www.baseportal.com/cgi-bin/baseportal.pl?htx=/drmdx/scheduleini2"

/* File handle type */
#ifdef _WIN32
# define FILE_HANDLE					HANDLE
#else
# define FILE_HANDLE					int
#endif

/* Name for DRM and AM schedule file. If you change something here, make sure
   that you also change the std::strings and help texts!  */
#define DRMSCHEDULE_INI_FILE_NAME		"DRMSchedule.ini"
#define AMSCHEDULE_INI_FILE_NAME		"AMSchedule.ini"
#define AMSCHEDULE_CSV_FILE_NAME		"AMSchedule.csv"

/* Time definitions for preview */
#define NUM_SECONDS_PREV_5MIN			300
#define NUM_SECONDS_PREV_15MIN			900
#define NUM_SECONDS_PREV_30MIN			1800

#define NUM_SECONDS_SOON_INACTIVE		600

/* String definitions for schedule days */
#define FLAG_STR_IRREGULAR_TRANSM		"0000000"
#define CHR_ACTIVE_DAY_MARKER			'1'

#include <iostream>

namespace Station {
	enum EState {IS_ACTIVE, IS_INACTIVE, IS_PREVIEW, IS_SOON_INACTIVE};
};

/* Classes ********************************************************************/
class CaseInsensitiveTreeWidgetItem : public QTreeWidgetItem
{
public:
	CaseInsensitiveTreeWidgetItem(QTreeWidget* parent=0) : QTreeWidgetItem(parent)
	{
	}

	bool operator< ( const QTreeWidgetItem & rhs) const
	{
		// bug 29 - sort frequency and power numerically
		int col = treeWidget()->sortColumn();
		if (col == 0) // online/offline
			return data(0, Qt::UserRole).toInt() < rhs.data(0, Qt::UserRole).toInt();
		else if (col == 3) // integer frequency
			return text( col ).toInt() < rhs.text( col ).toInt();
		else if (col == 4) // real power
			return text( col ).toDouble() < rhs.text( col ).toDouble();
		else
			return text( col ).toLower() < rhs.text( col ).toLower();
	}
};

class CStationsItem
{
public:
	CStationsItem() : iStartHour(0), iStartMinute(0), iStopHour(0),
		iStopMinute(0), iFreq(0), strName(""),
		strTarget(""), strLanguage(""), strSite(""),
		strCountry(""), strDaysFlags(""), strDaysShow(""),
		rPower((_REAL) 0.0) { }

	Station::EState stateAt(time_t, int) const;
	bool activeAt(time_t) const;
	int StartTime() const {return iStartHour * 100 + iStartMinute;}
	int StopTime() const{return iStopHour * 100 + iStopMinute;}
	void SetStartTime(const int iStartTime)
	{
		/* Recover hours and minutes */
		iStartHour = iStartTime / 100;
		iStartMinute = iStartTime - iStartHour * 100;
	}
	void SetStopTime(const int iStopTime)
	{
		/* Recover hours and minutes */
		iStopHour = iStopTime / 100;
		iStopMinute = iStopTime - iStopHour * 100;
	}

	void SetDaysFlagString(const QString& strNewDaysFlags);

	int		iStartHour;
	int		iStartMinute;
	int		iStopHour;
	int		iStopMinute;
	int		iFreq;
	QString	strName;
	QString	strTarget;
	QString	strLanguage;
	QString	strSite;
	QString	strCountry;
	QString	strDaysFlags;
	QString	strDaysShow;
	_REAL	rPower;
};


class CSchedule
{
public:
	CSchedule();
	virtual ~CSchedule() {}

	enum ESchedMode {SM_NONE, SM_DRM, SM_ANALOG};

	void ReadINIFile(FILE* pFile);
	void ReadCSVFile(FILE* pFile);
	ESchedMode GetSchedMode() {return eSchedMode;}
	void SetSchedMode(const ESchedMode);

	int GetNumberOfStations() {return StationsTable.size();}
	CStationsItem& GetItem(const int iPos) {return StationsTable[iPos];}
	Station::EState GetState(const int iPos);
	bool CheckFilter(const int iPos);

	void SetSecondsPreview(int iSec);
	int GetSecondsPreview();
	void UpdateStringListForFilter(const CStationsItem& StationsItem);
	void LoadSchedule();

	QStringList		ListTargets;
	QStringList		ListCountries;
	QStringList		ListLanguages;

	QString			countryFilterdrm, targetFilterdrm, languageFilterdrm;
	QString			countryFilteranalog, targetFilteranalog, languageFilteranalog;
	QString			schedFileName;
	QUrl			qurldrm, qurlanalog;

protected:
	void			SetAnalogUrl();

	std::vector<CStationsItem>	StationsTable;
	ESchedMode		eSchedMode;

	/* Minutes for stations preview in seconds if zero then only show active */
	int iSecondsPreviewdrm;
	int iSecondsPreviewanalog;
};
#endif
