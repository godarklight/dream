/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide Viewer
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

#ifndef _EPGDLG_H
#define _EPGDLG_H

#include "ui_EPGDlgbase.h"
#include "CWindow.h"
#include "../main-Qt/crx.h"
#include "../util-QT/EPG.h"
#include <QWidget>
#include <QDateTime>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QTreeWidget>
#include <map>


/* Definitions ****************************************************************/
#define COL_NAME	1

/* Define the timer interval of updating */
#define GUI_TIMER_EPG_UPDATE		1000 /* ms (1 second) */

/* list view columns */
#define COL_START		0
#define COL_NAME		1
#define	COL_GENRE		2
#define	COL_DESCRIPTION	3
#define COL_DURATION	4


/* Classes ********************************************************************/

class EPGDlg : public CWindow, public Ui_CEPGDlgbase
{
    Q_OBJECT

public:
    EPGDlg(CRx&, CSettings&, QWidget* parent = 0);
    virtual ~EPGDlg();

    void select();

protected:
    virtual void eventShow(QShowEvent*);
    virtual void eventHide(QHideEvent*);
    void setActive(QTreeWidgetItem*);
    bool isActive(QTreeWidgetItem*);

    virtual QString getFileName(const QDate& date, uint32_t sid, bool bAdvanced);
    virtual QString getFileName_etsi(const QDate& date, uint32_t sid, bool bAdvanced);
    virtual QDomDocument* getFile (const QString&);
    virtual QDomDocument* getFile (const QDate& date, uint32_t sid, bool bAdvanced);

    bool    do_updates;
    EPG     epg;
    CRx&    rx;
    QTimer	Timer;
    map<QString,uint32_t> sids;
    QIcon		greenCube;
    QTreeWidgetItem*	next;

signals:
    void NowNext(QString);

public slots:
    void on_channel_activated(const QString&);
    void on_dateEdit_dateChanged(const QDate&);
    void OnTimer();
};

#endif
