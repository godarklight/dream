/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2013
 *
 * Author(s):
 *	Volker Fischer, Stephane Fillod, Tomi Manninen, Andrea Russo, Julian Cable
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

#include "Schedule.h"
#include "../tables/TableStations.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QFileInfo>
#include <ctime>
#ifdef _WIN32
#include "../windows/platform_util.h"
#endif
#ifdef __ANDROID_API__
#include "../android/platform_util.h"
#endif
using namespace std;

/* Implementation *************************************************************/

CSchedule::CSchedule():
ListTargets(), ListCountries(), ListLanguages(),
StationsTable(),eSchedMode(SM_NONE),iSecondsPreviewdrm(0),iSecondsPreviewanalog(0)
{
	SetAnalogUrl();
}

void CSchedule::SetSecondsPreview(int iSec)
{
    if(eSchedMode==SM_NONE)
        return;
    if(eSchedMode==SM_DRM)
		iSecondsPreviewdrm = iSec;
	else
		iSecondsPreviewanalog = iSec;
}

int CSchedule::GetSecondsPreview()
{
    if(eSchedMode==SM_NONE)
        return 0;
    if(eSchedMode==SM_DRM)
		return iSecondsPreviewdrm;
	else
		return iSecondsPreviewanalog;
}

void CSchedule::SetSchedMode(const ESchedMode eNewSchM)
{
	eSchedMode = eNewSchM;
	StationsTable.clear();
	if(eSchedMode==SM_DRM)
	{
		schedFileName = DRMSCHEDULE_INI_FILE_NAME;
	}
    else if(eSchedMode==SM_ANALOG)
	{
		schedFileName = AMSCHEDULE_CSV_FILE_NAME;
	}
    else
    {
        schedFileName = "";
    }
}

void CSchedule::UpdateStringListForFilter(const CStationsItem& StationsItem)
{
	if (!ListTargets.contains(StationsItem.strTarget))
		ListTargets.append(StationsItem.strTarget);

	if (!ListCountries.contains(StationsItem.strCountry))
		ListCountries.append(StationsItem.strCountry);

	if (!ListLanguages.contains(StationsItem.strLanguage))
		ListLanguages.append(StationsItem.strLanguage);
}

void CSchedule::LoadSchedule()
{
	string filename(schedFileName.toUtf8().constData());
	QApplication::setOverrideCursor(Qt::BusyCursor);
	ListTargets = QStringList("");
	ListCountries = QStringList("");
	ListLanguages = QStringList("");
	StationsTable.clear();
	FILE* pFile = fopen(filename.c_str(), "r");
	if(pFile)
	{
		if(schedFileName.contains("ini"))
			ReadINIFile(pFile);
		else
			ReadCSVFile(pFile);
		fclose(pFile);
	}
	ListTargets.sort();
	ListCountries.sort();
	ListLanguages.sort();
	QApplication::restoreOverrideCursor();
}

void CSchedule::ReadINIFile(FILE* pFile)
{
	const int	iMaxLenName = 256;
	char	cName[iMaxLenName], *r;
	int		iFileStat, n;
	bool	bReadOK = true;

	r = fgets(cName, iMaxLenName, pFile); /* [DRMSchedule] */
	do
	{
		CStationsItem StationsItem;

		/* Start stop time */
		int iStartTime, iStopTime;
		iFileStat = fscanf(pFile, "StartStopTimeUTC=%04d-%04d\n",
			&iStartTime, &iStopTime);

		if (iFileStat != 2)
			bReadOK = false;
		else
		{
			StationsItem.SetStartTime(iStartTime);
			StationsItem.SetStopTime(iStopTime);
		}

		/* Days */
		/* Init days with the "irregular" marker in case no valid string could
		be read */
		QString strNewDaysFlags = FLAG_STR_IRREGULAR_TRANSM;

		iFileStat = fscanf(pFile, "Days[SMTWTFS]=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
		{
			/* Check for length of input string (must be 7) */
			QString strTMP(cName);
			if (strTMP.length() == 7)
				strNewDaysFlags = strTMP;
		}

		/* Frequency */
		iFileStat = fscanf(pFile, "Frequency=%d\n", &StationsItem.iFreq);
		if (iFileStat != 1)
			bReadOK = false;

		/* Target */
		iFileStat = fscanf(pFile, "Target=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.strTarget = cName;

		/* Power */
		iFileStat = fscanf(pFile, "Power=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.rPower = QString(cName).toFloat();

		/* Name of the station */
		iFileStat = fscanf(pFile, "Programme=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.strName = cName;

		/* Language */
		iFileStat = fscanf(pFile, "Language=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.strLanguage = cName;

		/* Site */
		iFileStat = fscanf(pFile, "Site=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.strSite = cName;

		/* Country */
		iFileStat = fscanf(pFile, "Country=%255[^\n|^\r]\n", cName);
		if (iFileStat != 1)
			n = fscanf(pFile, "\n");
		else
			StationsItem.strCountry = cName;

		iFileStat = fscanf(pFile, "\n");

		/* Check for error before applying data */
		if (bReadOK)
		{
			/* Set "days flag string" and generate strings for displaying active
			days */
			StationsItem.SetDaysFlagString(strNewDaysFlags);

			/* Add new item in table */
			StationsTable.push_back(StationsItem);

			UpdateStringListForFilter(StationsItem);
		}
	} while (!((iFileStat == EOF) || (bReadOK == false)));

	(void)r; (void)n;
}

void CSchedule::ReadCSVFile(FILE* pFile)
{
	const int	iMaxLenRow = 1024;
	char		cRow[iMaxLenRow];
	CStationData data;

	do {
		CStationsItem StationsItem;

		char* r = fgets(cRow, iMaxLenRow, pFile); (void)r;
		QStringList fields;
		stringstream ss(cRow);
		do {
			string s;
			getline(ss, s, ';');
			fields.push_back(s.c_str());
		} while(!ss.eof());

		StationsItem.iFreq = floor(fields[0].toFloat());

		if(fields[1] == "")
		{
			StationsItem.SetStartTime(0);
			StationsItem.SetStopTime(2400);
		}
		else
		{
			QStringList times = fields[1].split("-");
			if(times.count()==2)
			{
				StationsItem.SetStartTime(times[0].toInt());
				StationsItem.SetStopTime(times[1].toInt());
			}
		}

		if(fields[2].length()>0)
		{
			stringstream ss(fields[2].toUtf8().constData());
			char c;
			enum Days { Sunday=0, Monday=1, Tuesday=2, Wednesday=3,
				Thursday=4, Friday=5, Saturday=6
			};
			Days first=Sunday, last=Sunday;
			enum { no, in, done } range_state = no;
			// Days[SMTWTFS]=1111111
			QString strDays = "0000000";
			while(!ss.eof())
			{
				ss >> c;
				switch(c)
				{
				case '-':
					range_state = in;
					break;
				case 'M':
					ss >> c;
					last = Monday;
					break;
				case 'T':
					ss >> c;
					last = (c=='u')?Tuesday:Thursday;
					break;
				case 'W':
					ss >> c;
					last = Wednesday;
					break;
				case 'F':
					ss >> c;
					last = Friday;
					break;
				case 'S':
					ss >> c;
					last = (c=='u')?Sunday:Saturday;
					break;
				}
				switch(range_state)
				{
				case no:
					strDays[last] = '1';
					break;
				case in:
					first = last;
					range_state = done;
					break;
				case done:
					if(first<last)
					{
						for(int d=first; d<=last; d++)
							strDays[d] = '1';
					}
					range_state = no;
					break;
				}
			}
			StationsItem.SetDaysFlagString(strDays);
		}
		else
			StationsItem.SetDaysFlagString("1111111");

		//StationsItem.rPower = 0.0;
		//0   ;1        ;2    ;3  ;4               ;5;6;7;8;9;10
		//1170;1600-1700;Mo-Fr;USA;Voice of America;E; ; ;0; ;
		string homecountry;
		int fieldcount = fields.size();
		if(fieldcount>3)
		{
			homecountry = fields[3].toUtf8().constData();
			string c = data.itu_r_country(homecountry);
			if(c == "")
				c = homecountry;
			StationsItem.strCountry = QString::fromUtf8(c.c_str());
		}

		if(fieldcount>4)
			StationsItem.strName = fields[4];

		if(fieldcount>5)
		{
			string l = data.eibi_language(fields[5].toUtf8().constData());
			StationsItem.strLanguage = QString::fromUtf8(l.c_str());
		}

		if(fieldcount>6)
		{
			string s = fields[6].toUtf8().constData();
			string t = data.eibi_target(s);
			if(t == "")
			{
				string c = data.itu_r_country(s);
				if(c == "")
					StationsItem.strTarget = QString::fromUtf8(s.c_str());
				else
					StationsItem.strTarget = QString::fromUtf8(c.c_str());
			}
			else
			{
				StationsItem.strTarget = QString::fromUtf8(t.c_str());
			}
		}
		string country;
		string stn;
		if(fieldcount>7)
		{
			StationsItem.strSite = fields[7];
			string s  = fields[7].toUtf8().constData();
			if(s=="") // unknown or main Tx site of the home country
			{
				country = homecountry;
			}
			else
			{
				size_t i=0;
				s += '-';
				if(s[0]=='/') // transmitted from another country
					i++;
				string a,b;
				while(s[i]!='-')
					a += s[i++];
				i++;
				if(i<s.length())
					while(s[i]!='-')
						b += s[i++];
				if(s[0]=='/')
				{
					country = a;
					stn = b;
				}
				else
				{
					if(a.length()==3)
					{
						country = a;
						stn = b;
					}
					else
					{
						country = homecountry;
						stn = a;
					}
				}
			}
		}
		else
		{
			country = homecountry;
		}
		QString site = QString::fromUtf8(data.eibi_station(country, stn).c_str());
		if(site == "")
		{
			//cout << StationsItem.iFreq << " [" << StationsItem.strSite << "] [" << country << "] [" << stn << "]" << endl;
		}
		else
		{
			StationsItem.strSite = site;
		}

		/* Add new item in table */
		StationsTable.push_back(StationsItem);

		UpdateStringListForFilter(StationsItem);

	} while(!feof(pFile));
}

Station::EState CSchedule::GetState(const int iPos)
{
	return StationsTable[iPos].stateAt(time(nullptr), GetSecondsPreview());
}

bool CSchedule::CheckFilter(const int iPos)
{
	const CStationsItem& station = StationsTable[iPos];
	if(eSchedMode==SM_DRM && targetFilterdrm != "" && targetFilterdrm != station.strTarget)
		return false;
	if(eSchedMode==SM_DRM && countryFilterdrm != "" && countryFilterdrm != station.strCountry)
		return false;
	if(eSchedMode==SM_DRM && languageFilterdrm != "" && languageFilterdrm != station.strLanguage)
		return false;
	if(eSchedMode==SM_ANALOG && targetFilteranalog != "" && targetFilteranalog != station.strTarget)
		return false;
	if(eSchedMode==SM_ANALOG && countryFilteranalog != "" && countryFilteranalog != station.strCountry)
		return false;
	if(eSchedMode==SM_ANALOG && languageFilteranalog != "" && languageFilteranalog != station.strLanguage)
		return false;
	return true;
}

Station::EState CStationsItem::stateAt(time_t ltime, int previewSeconds) const
{
	if (activeAt(ltime))
	{
		time_t dt = ltime+NUM_SECONDS_SOON_INACTIVE;

		/* Check if the station soon will be inactive */
		if (activeAt(dt))
			return Station::IS_ACTIVE;
		else
			return Station::IS_SOON_INACTIVE;
	}
	else
	{
		for(time_t dt = ltime; dt < ltime + previewSeconds; dt += 60)
		{
			if (activeAt(dt))
			{
				return Station::IS_PREVIEW;
			}
		}
		return Station::IS_INACTIVE;
	}
}

#ifdef _WIN32
extern time_t timegm(struct tm *tm); // defined in Scheduler.cpp
#endif

bool CStationsItem::activeAt(time_t wantedTime) const
{
	struct tm ptm = *gmtime (&wantedTime);
	ptm.tm_hour = 0;
	ptm.tm_min = 0;
	ptm.tm_sec = 0;
	time_t wantedDay = timegm(&ptm); // start of daytime

	/* Get start time */
	time_t broadcastStart = wantedDay + 60*(60*iStartHour+iStartMinute);

	/* Get stop time */
	time_t broadcastStop = wantedDay + 60*(60*iStopHour+iStopMinute);

	/* Check, if stop time is on next day */
	if(broadcastStop < broadcastStart)
	{
		broadcastStop += 86400l;
	}

	/* Check day
	tm_wday: day of week (0 - 6; Sunday = 0). "strDaysFlags" are coded with
	pseudo binary representation. A one signals that day is active. The most
	significant 1 is the sunday, then followed the monday and so on. */
	if ((strDaysFlags[ptm.tm_wday] == CHR_ACTIVE_DAY_MARKER) ||
		/* Check also for special case: days are 0000000. This is reserved for
		DRM test transmissions or irregular transmissions. We define here
		that these stations are transmitting every day */
		(strDaysFlags == FLAG_STR_IRREGULAR_TRANSM))
	{
		if((broadcastStart <= wantedTime) && (wantedTime < broadcastStop))
		{
			return true;
		}
		else
		{
			/* yesterday's broadcast might still be on */
			int wday = ptm.tm_wday - 1;
			if(wday < 0)
				wday = 6;
			if ((strDaysFlags[wday] == CHR_ACTIVE_DAY_MARKER) || (strDaysFlags == FLAG_STR_IRREGULAR_TRANSM))
			{
				broadcastStart -= 86400l;
				broadcastStop -= 86400l;
				if((broadcastStart <= wantedTime) && (wantedTime < broadcastStop))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CStationsItem::SetDaysFlagString(const QString& strNewDaysFlags)
{
	/* Set internal "days flag" string and "show days" string */
	strDaysFlags = strNewDaysFlags;
	strDaysShow = "";

	/* Init days string vector */
	const QString strDayDef [] =
	{
		QObject::tr("Sun"),
		QObject::tr("Mon"),
		QObject::tr("Tue"),
		QObject::tr("Wed"),
		QObject::tr("Thu"),
		QObject::tr("Fri"),
		QObject::tr("Sat")
	};

	/* First test for day constellations which allow to apply special names */
	if (strDaysFlags == FLAG_STR_IRREGULAR_TRANSM)
		strDaysShow = QObject::tr("irregular");
	else if (strDaysFlags == "1111111")
		strDaysShow = QObject::tr("daily");
	else if (strDaysFlags == "1111100")
		strDaysShow = QObject::tr("from Sun to Thu");
	else if (strDaysFlags == "1111110")
		strDaysShow = QObject::tr("from Sun to Fri");
	else if (strDaysFlags == "0111110")
		strDaysShow = QObject::tr("from Mon to Fri");
	else if (strDaysFlags == "0111111")
		strDaysShow = QObject::tr("from Mon to Sat");
	else
	{
		/* No special name could be applied, just list all active days */
		for (int i = 0; i < 7; i++)
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
}

void CSchedule::SetAnalogUrl()
{
	QDate d = QDate::currentDate();
	int month = d.month();
	int year = 0;
	char season = 0;

	// transitions last sunday in March and October
	switch(month) {
	case 1:
	case 2:
		year = d.year()-1;
		season = 'b';
		break;
	case 3: {
		QDate s = d;
		s.setDate(d.year(), month+1, 1);
		s = s.addDays(0-s.dayOfWeek());
		if(d<s) {
			year = d.year()-1;
			season = 'b';
		} else {
			year = d.year();
			season = 'a';
		}
			}
			break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		year = d.year();
		season = 'a';
		break;
	case 10: {
		QDate s = d;
		s.setDate(d.year(), month+1, 1);
		int n = s.dayOfWeek();
		s = s.addDays(0-n);
		if(d<s) {
			year = d.year();
			season = 'a';
		} else {
			year = d.year();
			season = 'b';
		}
			 }
			 break;
	case 11:
	case 12:
		year = d.year();
		season = 'b';
	}
	if (season)
		qurlanalog = QUrl(QString("http://eibispace.de/dx/sked-%1%2.csv").arg(season).arg(year-2000,2));
}

