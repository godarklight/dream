/******************************************************************************\
 *
 * Copyright (c) 2013
 *
 * Author(s):
 *	David Flamand
 *
 * Description:
 *  Base class for gui window derived from QMainWindow.
 *  The window position is remembered, visibility state by parent is also
 *  remembered for child window.
 *  Children are automaticaly deleted when the last parent is deleted.
 *  A child can have more than one parent.
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

#include "CWindow.h"
#include <QRect>
#if QT_VERSION >= 0x050000
# include <QTimer>
#endif
#include <QWhatsThis>
using namespace std;

/* Implementation *************************************************************/


/* CEventFilter ------------------------------------------------------------- */

CEventFilter::CEventFilter() : eLastEventType(QEvent::Hide)
{
}

CEventFilter::~CEventFilter()
{
}

bool CEventFilter::isValid(const QEvent* event)
{
	bool bValid = false;
	QEvent::Type eEventType = event->type();
	switch (eEventType)
	{
	case QEvent::Hide:
		bValid = eLastEventType == QEvent::Show;
		eLastEventType = eEventType;
		break;
	case QEvent::Show:
		bValid = eLastEventType == QEvent::Hide;
		eLastEventType = eEventType;
		break;
	default:
		break;
	}
	return bValid;
}


/* CWindow ------------------------------------------------------------------ */

CWindow::CWindow(QWidget* parent, CSettings& Settings, const QString& windowName) :
	QMainWindow(), parents(), windowName(windowName), Settings(Settings)
{
	if (parent != nullptr)
		parents[parent] = "";
	init();
}
CWindow::CWindow(QMap<QWidget*,QString>& parents, CSettings& Settings, const QString& windowName) :
	QMainWindow(), parents(parents), windowName(windowName), Settings(Settings)
{
	init();
}
void CWindow::init()
{
//	printf("################################ CWindow::Init(%s) parents %i\n", getSection().c_str(), parents.count());
	setAttribute(Qt::WA_QuitOnClose, parents.count() == 0);
	setAttribute(Qt::WA_DeleteOnClose, false);
	loadWindowGeometry();
	foreach(QWidget* parent, parents.keys())
	{
		connect(parent, SIGNAL(showWindow(QWidget*, bool)), this, SLOT(OnShowWindow(QWidget*, bool)));
		connect(parent, SIGNAL(closeWindow(QWidget*)), this, SLOT(OnCloseWindow(QWidget*)));
		connect(parent, SIGNAL(deleteWindow(QWidget*)), this, SLOT(OnDeleteWindow(QWidget*)));
	}
}

CWindow::~CWindow()
{
//	printf("################################ CWindow::~CWindow(%s)\n", getSection().c_str());
	emit deleteWindow(this);
}

void CWindow::closeEvent(QCloseEvent* pEvent)
{
//	printf("################################ CWindow::closeEvent(%s)\n", getSection().c_str());
	eventClose(pEvent);
	if (pEvent->isAccepted())
		emit closeWindow(this);
}
void CWindow::showEvent(QShowEvent* pEvent)
{
	EVENT_FILTER(pEvent);
//	printf("################################ CWindow::showEvent(%s)\n", getSection().c_str());
	loadWindowGeometry();
	eventShow(pEvent);
	emit showWindow(this, true);
}
void CWindow::hideEvent(QHideEvent* pEvent)
{
	EVENT_FILTER(pEvent);
//	printf("################################ CWindow::hideEvent(%s)\n", getSection().c_str());
	saveWindowGeometry();
	eventHide(pEvent);
	emit showWindow(this, false);
}
void CWindow::contextMenuEvent(QContextMenuEvent *event)
{
	/* TODO popup a "What's This?" context menu */
	QWidget *child = childAt(event->pos());
	while (child)
	{
		if (!child->whatsThis().isEmpty())
		{
			QWhatsThis::enterWhatsThisMode();
			break;
		}
		child = child->parentWidget();
	}
}
#if QT_VERSION >= 0x050000
	/* On Qt 5 hideEvent is not generated on window minimize,
	   so instead we force an hide event, this also have a
	   side effect: the button is removed from the task bar.
	   TODO behave like Qt 4 */
bool CWindow::event(QEvent* pEvent)
{
	bool result = QWidget::event(pEvent);
	if(pEvent->type() == QEvent::WindowStateChange && isMinimized())
		QTimer::singleShot(0, this, SLOT(hide()));
	return result;
}
#endif

void CWindow::eventClose(QCloseEvent*)
{
//	printf("################################ CWindow::eventClose(%s)\n", getSection().c_str());
}
void CWindow::eventHide(QHideEvent*)
{
//	printf("################################ CWindow::eventHide(%s)\n", getSection().c_str());
}
void CWindow::eventShow(QShowEvent*)
{
//	printf("################################ CWindow::eventShow(%s)\n", getSection().c_str());
}
void CWindow::eventUpdate()
{
//	printf("################################ CWindow::eventUpdate(%s)\n", getSection().c_str());
}

void CWindow::toggleVisibility()
{
//	printf("################################ CWindow::toggleVisibility(%s)\n", getSection().c_str());
	setVisible(!isVisible());
}

void CWindow::loadWindowGeometry()
{
	CWinGeom s;
	Settings.Get(getSection(), s);
	const QRect WinGeom(s.iXPos, s.iYPos, s.iWSize, s.iHSize);
	if (WinGeom.isValid() && !WinGeom.isEmpty() && !WinGeom.isNull())
		setGeometry(WinGeom);
}
void CWindow::saveWindowGeometry()
{
	CWinGeom s;
	QRect WinGeom(geometry());
	s.iXPos = WinGeom.x();
	s.iYPos = WinGeom.y();
	s.iHSize = WinGeom.height();
	s.iWSize = WinGeom.width();
	Settings.Put(getSection(), s);
}

void CWindow::OnShowWindow(QWidget* window, bool bVisible)
{
//	printf("################################ CWindow::OnShowWindow(%s) bVisible=%i\n", getSection().c_str(), bVisible);
	if (parents.contains(window))
	{
		const QString key = QString("visible") + parents[window];
		if (!bVisible)
			putSetting(key, isVisible());
		if (!bVisible || getSetting(key, false))
		{
			if (bVisible)
				loadWindowGeometry();
			QMainWindow::setVisible(bVisible);
		}
	}
}

void CWindow::OnCloseWindow(QWidget* window)
{
//	printf("################################ CWindow::OnCloseWindow(%s) %p\n", getSection().c_str(), window);
	if (parents.contains(window))
	{
		QCloseEvent event;
		eventClose(&event);
	}
}

void CWindow::OnDeleteWindow(QWidget* window)
{
//	printf("################################ CWindow::OnDeleteWindow(%s) %p\n", getSection().c_str(), window);
	parents.remove(window);
//	if (parents.remove(window) >= 1)
//	{
//		if (parents.count() == 0)
//			delete this;
//	}
}

void CWindow::setVisible(bool visible)
{
	QMainWindow::setVisible(visible);
	if (visible)
	{
		QMainWindow::raise();
		QMainWindow::activateWindow();
	}
}

string CWindow::getSection(const bool bCommon) const
{
	return bCommon ? string("Dialog") : string(windowName.toLocal8Bit() + " Dialog");
}
QString CWindow::getSetting(const QString& key, const QString& defvalue, const bool bCommon)
{
	return QString::fromLocal8Bit(Settings.Get(getSection(bCommon), string(key.toLocal8Bit()), string(defvalue.toLocal8Bit())).c_str());
}
bool CWindow::getSetting(const QString& key, const bool defvalue, const bool bCommon)
{
	return Settings.Get(getSection(bCommon), string(key.toLocal8Bit()), defvalue);
}
int CWindow::getSetting(const QString& key, const int defvalue, const bool bCommon)
{
	return Settings.Get(getSection(bCommon), string(key.toLocal8Bit()), defvalue);
}
_REAL CWindow::getSetting(const QString& key, const _REAL defvalue, const bool bCommon)
{
	return Settings.Get(getSection(bCommon), string(key.toLocal8Bit()), defvalue);
}
void CWindow::putSetting(const QString& key, const QString& value, const bool bCommon)
{
	Settings.Put(getSection(bCommon), string(key.toLocal8Bit()), string(value.toLocal8Bit()));
}
void CWindow::putSetting(const QString& key, const bool value, const bool bCommon)
{
	Settings.Put(getSection(bCommon), string(key.toLocal8Bit()), value);
}
void CWindow::putSetting(const QString& key, const int value, const bool bCommon)
{
	Settings.Put(getSection(bCommon), string(key.toLocal8Bit()), value);
}
void CWindow::putSetting(const QString& key, const _REAL value, const bool bCommon)
{
	Settings.Put(getSection(bCommon), string(key.toLocal8Bit()), value);
}

