/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2006
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

#ifndef __MultSettingsDlg_H
#define __MultSettingsDlg_H

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qvalidator.h>

#include "../DrmReceiver.h"
#include "../datadecoding/epgutil.h"
#include "../util/Settings.h"
#include <QDialog>
#include "ui_MultSettingsDlgbase.h"

/* Definitions ****************************************************************/

/* Classes ********************************************************************/

class MultSettingsDlg : public QDialog, public Ui_CMultSettingsDlgBase
{
	Q_OBJECT

public:
	MultSettingsDlg(CParameter&, CSettings&, QWidget* parent = 0);
	virtual ~MultSettingsDlg();

protected:
	void ClearCache(QString sPath, QString sFilter, bool bDeleteDirs=false);

	virtual void	showEvent(QShowEvent* pEvent);
	virtual void	hideEvent(QHideEvent* pEvent);

	void			SetDataDirectoryControls();
	void			AddWhatsThisHelp();

	CParameter&		Parameters;
	CSettings&		Settings;

public slots:
	void OnbuttonChooseDir();
	void OnbuttonClearCacheMOT();
	void OnbuttonClearCacheEPG();
};

#endif
