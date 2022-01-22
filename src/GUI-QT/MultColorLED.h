/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
 *
 * Description:
 *
 * SetLight():
 *	0: Green
 *	1: Yellow
 *	2: Red
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

#if !defined(AFX_MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_)
#define AFX_MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_

#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>


/* Definitions ****************************************************************/
#define DEFAULT_UPDATE_TIME				600

/* The red and yellow light should be on at least this interval */
#define MIN_TIME_FOR_RED_LIGHT			100


/* Classes ********************************************************************/
class CMultColorLED : public QFrame
{
	Q_OBJECT

public:
	enum ELightColor {RL_GREY, RL_RED, RL_GREEN, RL_YELLOW};

	CMultColorLED(QWidget* parent);
	virtual ~CMultColorLED() {}

	void SetUpdateTime(int);
	void SetLight(ELightColor);
	void Reset();

protected:

	ELightColor		eColorFlag;

	QTimer			TimerRedLight;
	QTimer			TimerGreenLight;
	QTimer			TimerYellowLight;

	bool			bFlagRedLi;
	bool			bFlagGreenLi;
	bool			bFlagYellowLi;

	int				iUpdateTime;

	QColor			green;
	QColor			yellow;
	QColor			red;
	QColor			grey;

	void			UpdateColor();
	void			SetColor(const QColor& color);

protected slots:
	void OnTimerRedLight();
	void OnTimerGreenLight();
	void OnTimerYellowLight();
};


#endif // AFX_MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_
