/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2005
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

#include "Rig.h"

CRig::CRig(CParameter* np):
#ifdef HAVE_LIBHAMLIB
    Hamlib(), timer(new QTimer()),
#endif
    subscribers(0),pParameters(np)
{
#ifdef HAVE_LIBHAMLIB
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
#endif
}

void CRig::subscribe()
{
#ifdef HAVE_LIBHAMLIB
    if(Hamlib.GetHamlibModelID()==RIG_MODEL_NONE)
    {
        cerr << "can't subscribe - no rig set! " << subscribers << endl;
        return;
    }
#endif
    subscribers++;
    if(subscribers>0)
	{
#ifdef HAVE_LIBHAMLIB
        timer->start(1000);
#endif
	}
}

void CRig::unsubscribe()
{
#ifdef HAVE_LIBHAMLIB
    if(Hamlib.GetHamlibModelID()==RIG_MODEL_NONE)
    {
        cerr << "can't unsubscribe - no rig set! " << subscribers << endl;
        return;
    }
#endif
    if(subscribers>0)
    {
        subscribers--;
        cerr << "unsubscribe " << subscribers << endl;
    }
    if(subscribers<=0)
    {
#ifdef HAVE_LIBHAMLIB
        timer->stop();
        emit sigstr(-1000.0);
#endif
    }
}

void CRig::onTimer()
{
#ifdef HAVE_LIBHAMLIB
    _REAL r;
    if (Hamlib.GetSMeter(r) == CHamlib::SS_VALID)
    {
        pParameters->Lock();
        r += pParameters->rSigStrengthCorrection;
        pParameters->SigStrstat.addSample(r);
        pParameters->Unlock();
        emit sigstr(r);
    }
    else
        emit sigstr(-1000.0);
#endif
}

void CRig::LoadSettings(CSettings& s)
{
#ifdef HAVE_LIBHAMLIB
    Hamlib.LoadSettings(s);
#else
    (void)s;
#endif
}

void CRig::SaveSettings(CSettings& s)
{
#ifdef HAVE_LIBHAMLIB
    Hamlib.SaveSettings(s);
#else
    (void)s;
#endif
}

void CRig::SetFrequency(int f)
{
#ifdef HAVE_LIBHAMLIB
    Hamlib.SetFrequency(f);
#else
    (void)f;
#endif
}
