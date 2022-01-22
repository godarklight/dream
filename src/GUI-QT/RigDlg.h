/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable, Andrea Russo
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

#ifndef __RIGDLG_H
#define __RIGDLG_H

#include "../util/Utilities.h"
#include "../util-QT/Rig.h"
#include "ui_RigDlg.h"
#include <QDialog>
#include <QTimer>
#include <set>
#include <map>

/* Definitions ****************************************************************/

/* Classes ********************************************************************/

class RigDlg : public QDialog, public Ui_RigDlg
{
	Q_OBJECT

public:
    RigDlg(CRig&, QWidget* parent = 0);
    virtual ~RigDlg();

protected:
    void		showEvent(QShowEvent* pEvent);
    void		hideEvent(QHideEvent* pEvent);
    QString		getComboBoxComPort();

    CRig&		rig;
    rig_model_t		prev_rig_model;
    std::string		prev_port;
    map<rig_model_t,std::string> rigmap;
    bool	bComboBoxPortMutex;

public slots:
    void		on_rigTypes_itemSelectionChanged(); 
    void		on_modified_stateChanged(int);
    void		on_testRig_clicked();
    void		on_buttonBox_accepted();
    void		on_buttonBox_rejected();
    void		on_comboBoxPort_editTextChanged(const QString &);
    void		onSigstr(double);
};

#endif
