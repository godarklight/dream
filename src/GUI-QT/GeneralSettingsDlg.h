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

#ifndef __GENERAL_SETTINGS_DLG_H
#define __GENERAL_SETTINGS_DLG_H

#include "../Parameter.h"
#include "../util/Settings.h"

#include <qglobal.h>
#include <QDialog>
#include "ui_GeneralSettingsDlgbase.h"

/* Definitions ****************************************************************/

/* Classes ********************************************************************/
class GeneralSettingsDlg : public QDialog, public Ui_CGeneralSettingsDlgBase
{
	Q_OBJECT

public:
	GeneralSettingsDlg(CParameter& NParam, CSettings& NSettings, 
		QWidget* parent = 0);
	virtual ~GeneralSettingsDlg();

protected:
	virtual void	showEvent(QShowEvent* pEvent);
	virtual void	hideEvent(QHideEvent* pEvent);

	bool 	ValidInput(const QLineEdit* le);
	QString 	ExtractDigits(const QString strS, const int iStart, const int iDigits);
	void		ExtractReceiverCoordinates();

	void			AddWhatsThisHelp();

	CParameter&		Parameters;
	CSettings&		Settings;

public slots:
	void CheckSN(const QString& NewText);
	void CheckEW(const QString& NewText);
	void ButtonOkClicked();
	void OnCheckBoxUseGPS();
};

#endif
