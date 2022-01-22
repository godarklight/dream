/******************************************************************************\
 *
 * Copyright (c) 2013
 *
 * Author(s):
 *	David Flamand
 *
 * Description:
 *  see CWindow.cpp
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

#ifndef _CWINDOW_H
#define _CWINDOW_H

#include "../util/Settings.h"
#include <QWidget>
#include <QMainWindow>
#if QT_VERSION >= 0x050000
# include <QEvent>
#endif
#include <QHideEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QMap>
#include <QString>


/* Definitions ****************************************************************/

#define EVENT_FILTER(e) do { if (!ef.isValid((QEvent*)e)) return; } while(0)


/* Classes ********************************************************************/

/* CEventFilter ------------------------------------------------------------- */

/* The purpose of this class is to prevent showEvent and
   hideEvent from spurious event like unmatched show/hide,
   which cause some problem for window save and restore.
   The class may be adapted for other type of filtering
   as well. The member isValid() return false when the
   event must be ignored. */

class CEventFilter
{
public:
	CEventFilter();
	virtual ~CEventFilter();
	bool isValid(const QEvent* event);

protected:
	QEvent::Type eLastEventType;
};


/* CWindow ------------------------------------------------------------------ */

class CWindow : public QMainWindow
{
	Q_OBJECT

public:
	CWindow(QWidget* parent, CSettings& Settings, const QString& windowName);
	CWindow(QMap<QWidget*,QString>& parents, CSettings& Settings, const QString& windowName);
	virtual ~CWindow();
	void toggleVisibility();
	QString getSetting(const QString& key, const QString& defvalue, const bool bCommon=false);
	bool getSetting(const QString& key, const bool defvalue, const bool bCommon=false);
	int getSetting(const QString& key, const int defvalue, const bool bCommon=false);
	_REAL getSetting(const QString& key, const _REAL defvalue, const bool bCommon=false);
	void putSetting(const QString& key, const QString& value, const bool bCommon=false);
	void putSetting(const QString& key, const bool value, const bool bCommon=false);
	void putSetting(const QString& key, const int value, const bool bCommon=false);
	void putSetting(const QString& key, const _REAL value, const bool bCommon=false);
	virtual void eventUpdate();

private:
	void init();
	virtual void closeEvent(QCloseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void hideEvent(QHideEvent*);
	virtual void contextMenuEvent(QContextMenuEvent*);
#if QT_VERSION >= 0x050000
	virtual bool event(QEvent*);
#endif
	void loadWindowGeometry();
	void saveWindowGeometry();
	std::string getSection(const bool bCommon=false) const;
	QMap <QWidget*, QString> parents;
	const QString windowName;
	CEventFilter ef;

protected:
	virtual void eventClose(QCloseEvent*);
	virtual void eventHide(QHideEvent*);
	virtual void eventShow(QShowEvent*);
	CSettings& Settings;

public slots:
	virtual void setVisible(bool visible);
	void OnShowWindow(QWidget* window, bool bVisible);
	void OnCloseWindow(QWidget* window);
	void OnDeleteWindow(QWidget* window);

signals:
	void showWindow(QWidget* window, bool bVisible);
	void closeWindow(QWidget* window);
	void deleteWindow(QWidget* window);
};

#endif
