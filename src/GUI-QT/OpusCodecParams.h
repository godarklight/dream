/******************************************************************************\
 *
 * Copyright (c) 2012-2013
 *
 * Author(s):
 *  David Flamand
 *
 * Description:
 *  See CodecParams.cpp
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

#include <QDialog>
#include "ui_OpusCodecParams.h"
#include "../Parameter.h"
#include "../util/Settings.h"

class OpusCodecParams : public QDialog, public Ui_OpusCodecParams
{
	Q_OBJECT

public:
    OpusCodecParams(CSettings&, CParameter&, int iShortID,
		QWidget* parent = 0);
    virtual ~OpusCodecParams();
	void Toggle(void);
	void Show(bool bShow);

protected:
	CSettings&		Settings;
	CParameter&		Parameters;
	const int		iShortID;
	bool			bWasVisible;
	int				iLastXPosition;
	int				iLastYPosition;
	bool			bLastPositionValid;
	void			GetDialogPosition(void);
	virtual void	reject();

public slots:
	void OnButtonGroupChannels(int iID);
	void OnButtonGroupBandwidth(int iID);
	void OnButtonGroupFEC(int iID);
	void OnButtonGroupSignal(int iID);
	void OnButtonGroupApplication(int iID);
};
