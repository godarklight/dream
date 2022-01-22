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
*/

#define _POSIX_C_SOURCE 199309
#include <time.h>
#include "jack.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

instance_data_t::instance_data_t():num_channels(2),
        left(NULL), right(NULL), buff(NULL), underruns(0), overruns(0),
        peer_left(),peer_right()
{
    buff =
        jack_ringbuffer_create(
            8 * sizeof(short) * 48 * 400 * num_channels
        );
}

instance_data_t::~instance_data_t()
{
    jack_ringbuffer_free(buff);
}

struct CJackCommon
{
    CJackCommon():client(NULL),is_active(false),capture_data(NULL),play_data(NULL) {}
    jack_client_t * client;
    volatile bool is_active;
    instance_data_t *capture_data;
    instance_data_t *play_data;
    void initialise();
    void terminate();
};

static CJackCommon data;

static int
capture_stereo(jack_nframes_t nframes, void *arg)
{
    instance_data_t& data = *(instance_data_t *) arg;
    if (arg==NULL)
    {
        return 0;
    }
    if (data.left==NULL || data.right == NULL)
    {
        return 0;
    }
    jack_default_audio_sample_t *in[2];
    in[0] = (jack_default_audio_sample_t *) jack_port_get_buffer(data.left, nframes);
    in[1] = (jack_default_audio_sample_t *) jack_port_get_buffer(data.right, nframes);
    if (in[0]==NULL || in[1]==NULL)
    {
        return 0;
    }
    if (data.buff==NULL)
    {
        return 0;
    }
    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        short sample[2];
        size_t bytes = sizeof(short)*2;
        /* interleave samples for encoder */
        for (int chn = 0; chn < 2; chn++)
            sample[chn] = short (32768.0 * in[chn][i]);
        if (jack_ringbuffer_write (data.buff, (char *) sample, bytes) < bytes)
            data.overruns++;
    }
    return 0;
}

pair< string, string>
CJackPorts::get_ports(int dev)
{
    const size_t n = devices.size();
    if (n==0)
        return pair<string,string>("","");
    if (dev<0 || dev>=int(n))
        return ports[devices[n-1]];
    return ports[devices[dev]];
}

void
CJackPorts::load(jack_client_t * client, unsigned long flags)
{
    const char **port_names = jack_get_ports(client, ".*", NULL, flags);
    string client_name = jack_get_client_name(client);
    ports.clear();
    devices.clear();
    map<string, vector<string> > device_ports;
    if (port_names)
    {
        for (size_t i=0; port_names[i]; i++)
        {
            string port = port_names[i];
            if (port.substr(0, client_name.length())!=client_name)
            {
                size_t p = port.find(':');
                string dev = port.substr(0, p);
                device_ports[dev].push_back(port);
            }
            //free(port_names[i]);
        }
    }
    free(port_names);
    for (map<string, vector<string> >::iterator i = device_ports.begin(); i!=device_ports.end(); i++)
    {
        switch (i->second.size())
        {
        case 0:
            break; // should not happen
        case 1:
            ports[i->first].first = i->second[0];
            ports[i->first].second = i->second[0];
            devices.push_back(i->first);
            break;
        case 2:
            ports[i->first].first = i->second[0];
            ports[i->first].second = i->second[1];
            devices.push_back(i->first);
            break;
        default:
            for (size_t j=0; j<i->second.size()/2; j++)
            {
                stringstream dev;
                dev << i->first << "(" << 2*j << "," << 2*j+1 << ")";
                ports[dev.str()].first = i->second[2*j];
                ports[dev.str()].second = i->second[2*j+1];
                devices.push_back(dev.str());
            }
            if (i->second.size() % 2 == 1)
            {
                size_t j = i->second.size()-1;
                stringstream dev;
                dev << i->first << "(" << j << ")";
                ports[dev.str()].first = i->second[j];
                ports[dev.str()].second = i->second[j];
                devices.push_back(dev.str());
            }
        }
    }
}

static int
play_stereo(jack_nframes_t nframes, void *arg)
{
    if (arg==NULL)
    {
        return 0;
    }
    instance_data_t& data = *(instance_data_t *) arg;
    if (data.left==NULL || data.right == NULL)
    {
        return 0;
    }
    jack_default_audio_sample_t *out[2];
    out[0] = (jack_default_audio_sample_t *) jack_port_get_buffer(data.left, nframes);
    out[1] = (jack_default_audio_sample_t *) jack_port_get_buffer(data.right, nframes);
    if (out[0]==NULL || out[1]==NULL)
    {
        return 0;
    }
    if (data.buff==NULL)
    {
        return 0;
    }
    for (jack_nframes_t i = 0; i < nframes; i++)
    {
        short buffer[2];
        size_t bytes = sizeof(short)*2;
        size_t n = jack_ringbuffer_read(data.buff, (char*)buffer, bytes);
        if (n < bytes)
            data.underruns++;
        /* de-interleave samples from decoder */
        for (int chn = 0; chn < 2; chn++)
            out[chn][i] = jack_default_audio_sample_t(double(buffer[chn])/32768.0);
    }
    return 0;
}

static int
process_callback(jack_nframes_t nframes, void *arg)
{
    CJackCommon *This = (CJackCommon *) arg;
    if (This->capture_data)
        capture_stereo(nframes, This->capture_data);
    if (This->play_data)
        play_stereo(nframes, This->play_data);
    return 0;
}

static void
jack_shutdown(void *arg)
{
    cout << "jack shut down" << endl;
}

void CJackCommon::initialise()
{
    jack_options_t options = jack_options_t(JackNullOption | JackServerName);
    jack_status_t status;
    string server = "default";

    /* open a client connection to the Jack server */

    client = jack_client_open("dream", options, &status, server.c_str());
    if (client == NULL)
    {
        cerr << "jack_client_open() failed, status = " << ((long unsigned)
                status) << endl;
        if (status & JackServerFailed)
        {
            throw "Unable to connect to Jack server";
        }
    }
    if (status & JackServerStarted)
    {
        cout << "Jack server started" << endl;
    }
    if (status & JackNameNotUnique)
    {
        char *client_name = jack_get_client_name(client);
        cout << "Jack unique name `" << client_name << "' assigned" << endl;
    }

    /* tell the CSoundInJack server to call `process()' whenever
       there is work to be done.
     */

    jack_set_process_callback(client, process_callback, &data);

    /* tell the Jack server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
     */

    jack_on_shutdown(client, jack_shutdown, 0);

    if (jack_get_sample_rate(client) != Parameters.GetSampleRate())
    {
        throw "Jack: jack is running with the wrong sample rate";
    }
}

void CJackCommon::terminate()
{
    jack_client_close(client);
    client = NULL;
}

CSoundInJack::CSoundInJack():iBufferSize(0), bBlocking(true), capture_data(), dev(-1),ports()
{
    if (data.client==NULL)
        data.initialise();

    if (data.is_active && jack_deactivate(data.client))
    {
        throw "Jack: cannot deactivate client";
    }
    data.is_active = false;

    capture_data.left =
        jack_port_register(data.client, "input_0", JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsInput, 0);
    capture_data.right =
        jack_port_register(data.client, "input_1", JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsInput, 0);

    if ((capture_data.left == NULL) || (capture_data.right == NULL))
    {
        throw "Jack: no more ports available";
    }

    jack_ringbuffer_reset(capture_data.buff);

    data.capture_data = &capture_data;

    ports.load(data.client, JackPortIsOutput);

    if (jack_activate(data.client))
    {
        throw "Jack: cannot activate client";
    }
    data.is_active = true;
}

CSoundInJack::CSoundInJack(const CSoundInJack & e):
        iBufferSize(e.iBufferSize), bBlocking(e.bBlocking), device_changed(true),
        capture_data(e.capture_data), dev(e.dev), ports(e.ports)
{
}

CSoundInJack & CSoundInJack::operator=(const CSoundInJack & e)
{
    iBufferSize = e.iBufferSize;
    bBlocking = e.bBlocking;
    capture_data = e.capture_data;
    dev = e.dev;
    ports = e.ports;
    return *this;
}

CSoundInJack::~CSoundInJack()
{
    Close();
    if (data.client==NULL)
        return;

    if (data.is_active && jack_deactivate(data.client))
    {
        throw "Jack: cannot deactivate client";
    }
    data.is_active = false;
    data.capture_data = NULL;
    if (jack_activate(data.client))
    {
        throw "Jack: cannot activate client";
    }
    data.is_active = true;
}

void
CSoundInJack::Enumerate(vector<string>& choices)
{
    ports.load(data.client, JackPortIsOutput);
    choices = ports.devices;
}

int
CSoundInJack::GetDev()
{
    return dev;
}

void
CSoundInJack::SetDev(int iNewDevice)
{
    if (dev != iNewDevice)
    {
        dev = iNewDevice;
        device_changed = true;
    }
}

void
CSoundInJack::Init(int iNewBufferSize, bool bNewBlocking)
{
    if (device_changed == false)
        return;

    iBufferSize = iNewBufferSize;
    bBlocking = bNewBlocking;

    const char ** l = jack_port_get_connections(capture_data.left);
    const char ** r = jack_port_get_connections(capture_data.right);
    if (l || r)
        Close();

    if (l) free(l);
    if (r) free(r);

    pair<string,string> source = ports.get_ports(dev);

    int err = jack_connect (data.client, source.first.c_str(), jack_port_name(capture_data.left));
    if (err)
    {
        cout << "err " << err << " can't connect " << source.first << " to " << jack_port_name(capture_data.left) << endl;
    }

    err = jack_connect (data.client, source.second.c_str(), jack_port_name(capture_data.right));
    if (err)
    {
        cout << "err " << err << " can't connect " << source.second << " to " << jack_port_name(capture_data.right) << endl;
    }
    device_changed = false;
}

bool
CSoundInJack::Read(CVector<short>& psData)
{
    if (device_changed)
        Init(iBufferSize, bBlocking);

    size_t bytes = iBufferSize * sizeof(short);
    const int delay_ms = 100;
#ifdef _WIN32
    while (jack_ringbuffer_read_space(capture_data.buff) < bytes)
    {
        Sleep(delay_ms);
    }
#else
    timespec delay;
    delay.tv_sec = 0;
    const long one_ms = 1000000L; // nanoseconds
    delay.tv_nsec = delay_ms*one_ms;
    while (jack_ringbuffer_read_space(capture_data.buff) < bytes)
    {
        nanosleep(&delay, NULL);
    }
#endif
    char *buffer = (char *) &psData[0];
    //short buffer[16384];
    size_t n = jack_ringbuffer_read(capture_data.buff, (char*)&buffer[0], bytes);
    /*
    short smax=-32767, smin=32767;
    for(int i=0; i<iBufferSize; i++)
    {
    	if(smax<buffer[i]) smax = buffer[i];
    	if(smin>buffer[i]) smin = buffer[i];
    }
    cout << iBufferSize << " " << setw(8) << smin << " " << setw(8) << smax << endl;
    */
    if (n != bytes)
    {
        capture_data.underruns++;
        cerr << "jack read " << n << " wanted " << bytes << endl;
        return true;
    }
    return false;
}

void
CSoundInJack::Close()
{
    jack_port_disconnect(data.client, capture_data.left);
    jack_port_disconnect(data.client, capture_data.right);
    device_changed = true;
}

CSoundOutJack::CSoundOutJack():iBufferSize(0), bBlocking(true), device_changed(true),
        play_data(), dev(-1), ports()
{
    if (data.client==NULL)
        data.initialise();

    if (data.is_active && jack_deactivate(data.client))
    {
        throw "Jack: cannot deactivate client";
    }
    data.is_active = false;

    play_data.left =
        jack_port_register(data.client, "output_0", JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput, 0);
    play_data.right =
        jack_port_register(data.client, "output_1", JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput, 0);

    if ((play_data.left == NULL) || (play_data.right == NULL))
    {
        throw "Jack: no more ports available";
    }

    data.play_data = &play_data;
    ports.load(data.client, JackPortIsInput);

    if (jack_activate(data.client))
    {
        throw "Jack: cannot activate client";
    }
    data.is_active = true;
}

CSoundOutJack::~CSoundOutJack()
{
    Close();
    if (data.is_active && jack_deactivate(data.client))
    {
        throw "Jack: cannot deactivate client";
    }
    data.is_active = false;
    data.capture_data = NULL;
    if (jack_activate(data.client))
    {
        throw "Jack: cannot activate client";
    }
    data.is_active = true;
}

CSoundOutJack::CSoundOutJack(const CSoundOutJack & e):
        iBufferSize(e.iBufferSize), bBlocking(e.bBlocking),dev(e.dev)
{
}

CSoundOutJack & CSoundOutJack::operator=(const CSoundOutJack & e)
{
    iBufferSize = e.iBufferSize;
    bBlocking = e.bBlocking;
    play_data = e.play_data;
    dev = e.dev;
    ports = e.ports;
    return *this;
}

void
CSoundOutJack::Enumerate(vector<string>& choices)
{
    ports.load(data.client, JackPortIsInput);
    choices = ports.devices;
}

int
CSoundOutJack::GetDev()
{
    return dev;
}

void
CSoundOutJack::SetDev(int iNewDevice)
{
    if (dev != iNewDevice)
    {
        dev = iNewDevice;
        device_changed = true;
    }
}

void
CSoundOutJack::Init(int iNewBufferSize, bool bNewBlocking)
{
    if (device_changed == false)
        return;

    iBufferSize = iNewBufferSize;
    bBlocking = bNewBlocking;

    const char ** l = jack_port_get_connections(play_data.left);
    const char ** r = jack_port_get_connections(play_data.right);
    if (l || r)
        Close();

    if (l) free(l);

    pair<string,string> sink = ports.get_ports(dev);

    int err = jack_connect (data.client, jack_port_name(play_data.left), sink.first.c_str());
    if (err)
    {
        cout << "err " << err << " can't connect " << jack_port_name(play_data.left) << " to " << sink.first << endl;
    }
    err = jack_connect (data.client, jack_port_name(play_data.right), sink.second.c_str());
    if (err)
    {
        cout << "err " << err << " can't connect " << jack_port_name(play_data.right) << " to " << sink.second << endl;
    }
    device_changed = false;
}

bool
CSoundOutJack::Write(CVector<short>& psData)
{
    if (device_changed)
        Init(iBufferSize, bBlocking);

    size_t bytes = psData.Size()*sizeof(short);
    if (jack_ringbuffer_write (play_data.buff, (char *) &psData[0], bytes) < bytes)
    {
        play_data.overruns++;
        return true;
    }
    return false;
}

void
CSoundOutJack::Close()
{
    jack_port_disconnect(data.client, play_data.left);
    jack_port_disconnect(data.client, play_data.right);
    device_changed = true;
}
