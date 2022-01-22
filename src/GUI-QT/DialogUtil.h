/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2006
 *
 * Author(s):
 *	Volker Fischer, Andrea Russo
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

#if !defined(DIALOGUTIL_H__FD6B23452398345OIJ9453_804E1606C2AC__INCLUDED_)
#define DIALOGUTIL_H__FD6B23452398345OIJ9453_804E1606C2AC__INCLUDED_

#include "../Parameter.h"
#include "../DrmTransceiver.h"
#include "../sound/selectioninterface.h"

#include <map>

#include "ui_AboutDlgbase.h"
#include <QThread>
#include <QMenu>
#include <QDialog>
#include <QAction>
#include <QEvent>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QAction>

#include <qwt_thermo.h> /* S-Meter */

class CRig;
typedef int rig_model_t;

/* Definitions ****************************************************************/

/* Definition for Courier font */
#ifdef _WIN32
# define FONT_COURIER    "Courier New"
#elif defined(__linux__)
# define FONT_COURIER    "Monospace"
#else
# define FONT_COURIER    "Courier"
#endif
/* Classes ********************************************************************/

/* About dialog ------------------------------------------------------------- */
class CAboutDlg : public QDialog, public Ui_CAboutDlgBase
{
	Q_OBJECT

public:
	CAboutDlg(QWidget* parent = 0);
	virtual ~CAboutDlg() {}
};

/* Help Usage --------------------------------------------------------------- */
class CHelpUsage : public CAboutDlg
{
	Q_OBJECT

public:
	CHelpUsage(const char* usage, const char* argv0, QWidget* parent = 0);
	virtual ~CHelpUsage() {}
};

/* System Tray -------------------------------------------------------------- */
class CSysTray
{
public:
	static CSysTray* Create(QWidget* parent, const char* callbackIcon, const char* callbackTimer, const char* icon);
	static void Destroy(CSysTray** pSysTray);
	static void SetToolTip(CSysTray* pSysTray, const QString& Title, const QString& Message);
	static void Start(CSysTray* pSysTray);
	static void Stop(CSysTray* pSysTray, const QString& Message);
	static QAction* AddAction(CSysTray* pSysTray, const QString& text, const QObject* receiver, const char* member);
	static QAction* AddSeparator(CSysTray* pSysTray);

protected:
	~CSysTray();
	CSysTray(QWidget* parent, const char* callbackIcon, const char* callbackTimer, const char* icon);
	void CreateContextMenu();
	QString Title;
	QString Message;
	QSystemTrayIcon* pSystemTrayIcon;
	QTimer* pTimer;
	QMenu* pContextMenu;
};

/* GUI help functions ------------------------------------------------------- */
/* Converts from RGB to integer and back */
class CRGBConversion
{
public:
	static int RGB2int(const QColor newColor)
	{
		/* R, G and B are encoded as 8-bit numbers */
		int iReturn = newColor.red();
		iReturn <<= 8;
		iReturn |= newColor.green();
		iReturn <<= 8;
		iReturn |= newColor.blue();
		return iReturn;
	}

	static QColor int2RGB(const int iValue)
	{
		return QColor((iValue >> 16) & 255, (iValue >> 8) & 255, iValue & 255);
	}
};


/* s-meter thermo parameters */
#define S_METER_THERMO_MIN				((_REAL) -60.0) /* dB */
#define S_METER_THERMO_MAX				((_REAL) 60.0) /* dB */
#define S_METER_THERMO_ALARM			((_REAL) 0.0) /* dB */

void InitSMeter(QWidget* parent, QwtThermo* sMeter);

#endif // DIALOGUTIL_H__FD6B23452398345OIJ9453_804E1606C2AC__INCLUDED_
