/******************************************************************************\
 * British Broadcasting Corporation * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable, Andrea Russo
 *
 * Description:
 * Settings for the receiver
 * Perhaps this should be Receiver Controls rather than Settings
 * since selections take effect immediately and there is no apply/cancel
 * feature. This makes sense, since one wants enable/disable GPS, Rig, Smeter
 * to be instant and mute/savetofile etc.
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

#include "RigDlg.h"
#include "DialogUtil.h"
#include <QTreeWidgetItem>

/* Implementation *************************************************************/

RigDlg::RigDlg(CRig& nrig, QWidget* parent) :
    QDialog(parent),
    rig(nrig), rigmap(), bComboBoxPortMutex(false)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    setupUi(this);
#if QWT_VERSION < 0x060100
    sMeter->setScalePosition(QwtThermo::TopScale);
#else
    sMeter->setScalePosition(QwtThermo::TrailingScale);
#endif

    map<rig_model_t,CHamlib::SDrRigCaps> r;

    rig.GetRigList(r);
    modified->setEnabled(false);
    //rigTypes->setColumnCount(2);
    rigTypes->setSortingEnabled(false);
    QTreeWidgetItem* none = new QTreeWidgetItem(rigTypes);
	none->setText(0, "None");
	none->setData(0, Qt::UserRole, RIG_MODEL_NONE);
    for(map<rig_model_t,CHamlib::SDrRigCaps>::const_iterator i=r.begin(); i!=r.end(); i++)
    {
		rig_model_t model_num = i->first;
        CHamlib::SDrRigCaps rc =  i->second;
        QTreeWidgetItem* mfr, *model;
        if(rc.strManufacturer=="" || rc.strModelName=="")
        {
            continue;
        }
        QList<QTreeWidgetItem *> l = rigTypes->findItems(rc.strManufacturer.c_str(), Qt::MatchFixedString);
        if(l.size()==0)
        {
            mfr = new QTreeWidgetItem(rigTypes);
            mfr->setText(0,rc.strManufacturer.c_str());
			mfr->setFlags(mfr->flags() & ~Qt::ItemIsSelectable);
        }
        else
        {
            mfr = l.first();
        }
        model = new QTreeWidgetItem(mfr);
        model->setText(0,rc.strModelName.c_str());
		model->setData(0, Qt::UserRole, model_num);
		rigmap[model_num] = rc.strModelName;
    }
    rigTypes->setSortingEnabled(false);
    rigTypes->sortItems(9, Qt::AscendingOrder);

   	InitSMeter(this, sMeter);
}

RigDlg::~RigDlg()
{
}

void RigDlg::showEvent(QShowEvent*)
{
	/* Port selection */
	bComboBoxPortMutex = true;
	map<string,string> ports;
	rig.GetPortList(ports);
	comboBoxPort->clear();
	prev_port = rig.GetComPort();
	int index = -1;
	for (map<string,string>::const_iterator i=ports.begin(); i!=ports.end(); i++)
	{
		comboBoxPort->addItem(i->first.c_str(), i->second.c_str());
		if (i->second.compare(prev_port) == 0)
			index = comboBoxPort->count() - 1; /* index is zero based */
	}
	if (index != -1)
	{
		comboBoxPort->setCurrentIndex(index);
	}
	else
	{	/* Add the port to the list if not found */
		comboBoxPort->addItem(prev_port.c_str(), prev_port.c_str());
		comboBoxPort->setCurrentIndex(comboBoxPort->findText(prev_port.c_str()));
	}
	bComboBoxPortMutex = false;

	/* Rig model selection */
    prev_rig_model = rig.GetHamlibModelID();
    if (prev_rig_model == RIG_MODEL_NONE)
    {
        rigTypes->setCurrentItem(rigTypes->topLevelItem(0)); /* None */
    }
    else
    {
        map<rig_model_t,string>::const_iterator m = rigmap.find(prev_rig_model);
        if(m!=rigmap.end())
        {
            QString name(m->second.c_str());
            QList<QTreeWidgetItem *> l = rigTypes->findItems(name, Qt::MatchExactly | Qt::MatchRecursive);
            if(l.size()>0) {
                rigTypes->setCurrentItem(l.front());
                selectedRigType->setText(name);
            }
        }
    }

	connect(&rig, SIGNAL(sigstr(double)), this, SLOT(onSigstr(double)));
}

void RigDlg::hideEvent(QHideEvent*)
{
	disconnect(&rig, SIGNAL(sigstr(double)), this, SLOT(onSigstr(double)));
}

void
RigDlg::on_rigTypes_itemSelectionChanged()
{
	QList<QTreeWidgetItem*> l = rigTypes->selectedItems();
	if(l.count()==1) {
		const QTreeWidgetItem* item = l.first();
		selectedRigType->setText(item->text(0));
		rig.SetHamlibModelID(item->data(0, Qt::UserRole).toInt());
	}
}

void
RigDlg::on_modified_stateChanged(int state)
{
	rig.SetEnableModRigSettings(state?false:true);
}

void
RigDlg::on_testRig_clicked()
{
	rig.SetComPort(comboBoxPort->itemData(comboBoxPort->currentIndex()).toString().toStdString());
	rig.SetHamlibModelID(rigTypes->currentItem()->data(0, Qt::UserRole).toInt());
	rig.subscribe();
}

void
RigDlg::on_buttonBox_accepted()
{
	rig.SetComPort(getComboBoxComPort().toStdString());
	rig.SetHamlibModelID(rigTypes->currentItem()->data(0, Qt::UserRole).toInt());
	rig.unsubscribe();
	close();
}

void
RigDlg::on_buttonBox_rejected()
{
	rig.SetComPort(prev_port);
	rig.SetHamlibModelID(prev_rig_model);
	rig.unsubscribe();
	close();
}

void
RigDlg::on_comboBoxPort_editTextChanged(const QString&)
{
	if (bComboBoxPortMutex == false)
		rig.SetComPort(getComboBoxComPort().toStdString());
}

QString
RigDlg::getComboBoxComPort()
{
	QString strPort;
	const int index = comboBoxPort->currentIndex();
	if (comboBoxPort->currentText().compare(comboBoxPort->itemText(index)))
		strPort = comboBoxPort->currentText();
	else
		strPort = comboBoxPort->itemData(index).toString();
	return strPort;
}

void
RigDlg::onSigstr(double r)
{
	sMeter->setValue(r);
}
