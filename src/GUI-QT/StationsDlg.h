/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
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
#ifndef __StationsDlg_H
#define __StationsDlg_H

#include <QSignalMapper>
#include <QNetworkAccessManager>
#include <QTimer>
#include "ui_StationsDlgbase.h"

#include "DialogUtil.h"
#include "CWindow.h"
#include "../main-Qt/crx.h"
#include "Schedule.h"

/* Definitions ****************************************************************/

class StationsDlg : public CWindow, public Ui_StationsDlgbase
{
	Q_OBJECT

public:
#ifdef HAVE_LIBHAMLIB
    StationsDlg(CRx&, CSettings&, CRig&, QMap<QWidget*,QString>&);
#else
    StationsDlg(CRx&, CSettings&, QMap<QWidget*,QString>&);
#endif
	virtual ~StationsDlg();

protected:
	virtual void	eventClose(QCloseEvent* pEvent);
	virtual void	eventHide(QHideEvent* pEvent);
	virtual void	eventShow(QShowEvent* pEvent);
	void			LoadSettings();
	void			SaveSettings();
    void			LoadSchedule();
    void			LoadScheduleView();
    void			UpdateTransmissionStatus();
    void			LoadFilters();
	void			SetFrequencyFromGUI(int);
	void			AddWhatsThisHelp();
	void			EnableSMeter();
	void			DisableSMeter();
	bool		showAll();
	bool		GetSortAscending();
	void			SetSortAscending(bool b);
	void			ColumnParamFromStr(const QString& strColumnParam);
	void			ColumnParamToStr(QString& strColumnParam);
	int				currentSortColumn();
	bool		bCurrentSortAscendingdrm;
	bool		bCurrentSortAscendinganalog;
	int				iSortColumndrm;
	int				iSortColumnanalog;
	QString			strColumnParamdrm;
	QString			strColumnParamanalog;

    CRx&            rx;
#ifdef HAVE_LIBHAMLIB
	CRig&			Rig;
#endif
	CSchedule		schedule;
	QIcon			greenCube;
	QIcon			redCube;
	QIcon			orangeCube;
	QIcon			pinkCube;
	QSignalMapper*	previewMapper;
	QActionGroup*	previewGroup;
	QSignalMapper*	showMapper;
	QActionGroup*	showGroup;
	QNetworkAccessManager *manager;
	QTimer			Timer;
	bool		bReInitOnFrequencyChange;

	QMutex			ListItemsMutex;

	QString			okMessage, badMessage;

signals:
	void subscribeRig();
	void unsubscribeRig();
public slots:
	void OnSigStr(double);
	void OnTimer();
	void OnSMeterMenu(int iID);
	void OnSMeterMenu();
	void OnUrlFinished(QNetworkReply*);
	void OnShowStationsMenu(int iID);
	void OnShowPreviewMenu(int iID);
	void OnFreqCntNewValue(double dVal);
	void OnHeaderClicked(int c);
	void on_actionGetUpdate_triggered();
	void on_ListViewStations_itemSelectionChanged();
	void on_ComboBoxFilterTarget_activated(const QString&);
	void on_ComboBoxFilterCountry_activated(const QString&);
	void on_ComboBoxFilterLanguage_activated(const QString&);
};
#endif
