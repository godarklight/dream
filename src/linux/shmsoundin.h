/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Decription:
 * Winradio G313 sound interface
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

#ifndef _SHMSOUNDIN_H
#define _SHMSOUNDIN_H

#include "../soundinterface.h"
#include "pa_shm_ringbuffer.h"

class CShmSoundIn: public CSoundInInterface
{
public:
    CShmSoundIn();
    virtual 			~CShmSoundIn();
    virtual void		Enumerate(std::vector<std::string>& choices);
    virtual void		SetDev(int);
    virtual int			GetDev();

    virtual void		Init(int iNewBufferSize, bool bNewBlocking = true);
    virtual void		Close();
	virtual std::string		GetVersion() { return "linux shared memory audio input"; }
    virtual bool	Read(CVector<short>& psData);

    void				SetShmPath(const std::string& p) {
        shm_path = p;
    }
    void				SetShmChannels(int n) {
        shmChannels = n;
    }
    void				SetWantedChannels(int n) {
        wantedChannels = n;
    }
    void				SetName(const std::string& n) {
        name = n;
    }

protected:

    PaUtilShmRingBuffer *ringBuffer;
    int		shmid;
    void*	shm;
    std::string	shm_path;
    std::string	name;
    int		shmChannels;
    int		wantedChannels;
};

#endif
