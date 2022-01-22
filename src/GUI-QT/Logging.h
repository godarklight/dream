/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2012
 *
 * Author(s):
 *	Volker Fischer, Julian Cable
 *
 * Description:
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

#ifndef __LOGGING_H
#define __LOGGING_H

#include <QTimer>
#include <QString>

#include "../GlobalDefinitions.h"
#include "../ReceptLog.h"

/* Classes ********************************************************************/
class CSettings;

class CLogging: public QObject
{
	Q_OBJECT

public:
	CLogging(CParameter&);
	virtual ~CLogging() {}
	void LoadSettings(CSettings&);
	void SaveSettings(CSettings&);
	bool enabled() { return state!=off; }

protected:
	QTimer			TimerLogFile;
	QTimer			TimerLogFileStart;

	CShortLog		shortLog;
	CLongLog		longLog;
	int			iLogDelay;
	int			iLogCount;
	enum {off,starting,on}  state;

signals:
	void subscribeRig();
	void unsubscribeRig();
public slots:
	void start();
	void stop();
	void OnTimerLogFile();
	void OnTimerLogFileStart();
};

#endif
