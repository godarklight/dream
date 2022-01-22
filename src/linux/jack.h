/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	Jack sound classes
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

#ifndef _JACK_H
#define _JACK_H

#include "../soundinterface.h"

#ifdef _WIN32
# include <windows.h>
	typedef HANDLE pthread_t;
#endif
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <map>

/* Classes ********************************************************************/

struct instance_data_t
{
    instance_data_t();
    ~instance_data_t();
    int num_channels;
    jack_port_t *left;
    jack_port_t *right;
    jack_ringbuffer_t* buff;
    int underruns;
    int overruns;
    std::string peer_left, peer_right;
};

class CJackPorts
{
public:
    CJackPorts():devices(),ports() {}
    std::vector<std::string> devices;
    void load(jack_client_t * client, unsigned long flags);
    pair< std::string, std::string>get_ports(int dev);
protected:
    map<std::string, pair< std::string, std::string> > ports;
};

class CSoundInJack : public CSoundInInterface
{
public:
    CSoundInJack();
    virtual ~CSoundInJack();
    CSoundInJack(const CSoundInJack& e);
    CSoundInJack& operator=(const CSoundInJack& e);

    virtual void		Init(int iNewBufferSize, bool bNewBlocking = true);
    virtual bool	Read(CVector<short>& psData);
    virtual void		Enumerate(std::vector<std::string>&);
    virtual int			GetDev();
    virtual void		SetDev(int iNewDev);
    virtual void		Close();
	virtual std::string		GetVersion() { return "JACK audio input"; }
protected:
    int iBufferSize;
    bool bBlocking;
    bool device_changed;
    instance_data_t capture_data;
    int dev;
    CJackPorts ports;
};

class CSoundOutJack : public CSoundOutInterface
{
public:
    CSoundOutJack();
    virtual ~CSoundOutJack();
    CSoundOutJack(const CSoundOutJack& e);
    CSoundOutJack& operator=(const CSoundOutJack& e);

    virtual void		Init(int iNewBufferSize, bool bNewBlocking = true);
    virtual bool	Write(CVector<short>& psData);
    virtual void		Enumerate(std::vector<std::string>&);
    virtual int			GetDev();
    virtual void		SetDev(int iNewDev);
    virtual void		Close();
	virtual std::string		GetVersion() { return "JACK audio output"; }
protected:
    int iBufferSize;
    bool bBlocking;
    bool device_changed;
    instance_data_t play_data;
    int dev;
    CJackPorts ports;
};

#endif
