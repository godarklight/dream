/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Decription:
 * PortAudio sound interface
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

#include "drm_portaudio.h"
#include <iostream>
#include <cstring>
using namespace std;

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */
static int
captureCallback(const void *inputBuffer, void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo *,
                PaStreamCallbackFlags statusFlags, void *userData)
{
    /* Cast data passed through stream to our structure. */
    CPaCommon *This = (CPaCommon *) userData;
    (void) outputBuffer;		/* Prevent unused variable warning. */
    long bytes = long(framesPerBuffer*2*sizeof(short));
    long avail = PaUtil_GetRingBufferWriteAvailable(&This->ringBuffer);
    PaUtil_WriteRingBuffer(&This->ringBuffer, inputBuffer, (avail<bytes)?avail:bytes);
    if (statusFlags&paInputOverflow)
        This->xruns++;
    return 0;
}

static int
playbackCallback(const void *inputBuffer, void *outputBuffer,
                 unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo *,
                 PaStreamCallbackFlags statusFlags, void *userData)
{
    CPaCommon *This = (CPaCommon *) userData;
    (void) inputBuffer;			/* Prevent unused variable warning. */
    long bytes = framesPerBuffer*2*sizeof(short);
    long avail = PaUtil_GetRingBufferReadAvailable(&This->ringBuffer);
    PaUtil_ReadRingBuffer(&This->ringBuffer, outputBuffer, (avail<bytes)?avail:bytes);
    if (statusFlags&paOutputUnderflow)
        This->xruns++;
    return 0;
}

int CPaCommon::pa_count = 0;

CPaCommon::CPaCommon(bool cap):ringBuffer(),xruns(0),stream(nullptr),
    names(), devices(),
    is_capture(cap), blocking(true), device_changed(true), xrun(false),
    framesPerBuffer(0), ringBufferData(nullptr)
{
    if (pa_count == 0)
    {
        int
        err = Pa_Initialize();
        if (err != paNoError)
            throw string("PortAudio error: ") + Pa_GetErrorText(err);
    }
    pa_count++;
    vector < string > choices;
    vector < string > descriptions;
    string def;
    Enumerate(choices, descriptions, def);
}

CPaCommon::~CPaCommon()
{
    Close();
    if (ringBufferData)
        delete[] ringBufferData;

    pa_count--;
    if (pa_count == 0)
    {
        int err = Pa_Terminate();
        if (err != paNoError)
            throw string("PortAudio error: ") + Pa_GetErrorText(err);
    }
}

void
CPaCommon::Enumerate(vector < string > &choices, vector < string > &descriptions, string& defaultDevice)
{
    vector < string > tmp;

    names.clear();
    descriptions.clear();
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
        throw string("PortAudio error: ") + Pa_GetErrorText(numDevices);
    PaHostApiIndex nApis = Pa_GetHostApiCount();

    for (int i = 0; i < numDevices; i++)
    {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (( is_capture && deviceInfo->maxInputChannels > 1)
                || ( (!is_capture) && deviceInfo->maxOutputChannels > 1))
        {
            string api="";
            if (nApis>1)
            {
                const PaHostApiInfo* info = Pa_GetHostApiInfo(deviceInfo->hostApi);
                if (info)
                    api = string(info->name)+":";
            }
            cerr<< api+deviceInfo->name << endl;
            names.push_back(api+deviceInfo->name);
            descriptions.push_back("");
            devices.push_back(i);
        }
    }
    choices = names;
}

void
CPaCommon::SetDev(string sNewDevice)
{
    if (dev != sNewDevice)
    {
        dev = sNewDevice;
        device_changed = true;
    }
}

string
CPaCommon::GetDev()
{
    return dev;
}

/* buffer_size is in samples - frames would be better */
bool
CPaCommon::Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    if (device_changed == false && double(iSampleRate) == samplerate)
        return false;

    unsigned long channels=2;

    samplerate = double(iSampleRate);

    if (is_capture)
        framesPerBuffer = iNewBufferSize / channels;
    else
        framesPerBuffer = 256;

    blocking = bNewBlocking; /* TODO honour this */
    iBufferSize = iNewBufferSize;

    ReInit();

    if (stream)
    {
        const PaStreamInfo* info = Pa_GetStreamInfo( stream );
        if (is_capture)
            cout << "init capture ";
        else
            cout << "init play ";
        cout << iNewBufferSize;
        if (info)
            cout << " latency " << info->outputLatency;
        else
            cout << " can't read latency";
        cout << endl;
    }
    else
    {
        cerr << "portaudio can't open stream" << endl;
        //throw "portaudio open error";
    }

    return true;
}

void
CPaCommon::ReInit()
{
    Close();

    PaStreamParameters pParameters;

    memset(&pParameters, 0, sizeof(pParameters));
    pParameters.channelCount = 2;
    pParameters.hostApiSpecificStreamInfo = nullptr;
    pParameters.sampleFormat = paInt16;

    int idev = -1;
    for (int i = 0; i < int(names.size()); i++)
    {
        if (names[unsigned(i)] == dev)
        {
            idev = i;
            break;
        }
    }

    if (idev < 0 || idev >= int(devices.size()))
    {
        if (is_capture)
            pParameters.device = Pa_GetDefaultInputDevice();
        else
            pParameters.device = Pa_GetDefaultOutputDevice();
    }
    else
    {
        cout << "opening " << names[unsigned(idev)] << endl;
        pParameters.device = devices[unsigned(idev)];
    }

    if (pParameters.device == paNoDevice)
        return;

    unsigned long minRingBufferSize;
    int err;

    if (is_capture)
    {
        pParameters.suggestedLatency = Pa_GetDeviceInfo(pParameters.device)->defaultLowInputLatency;
        minRingBufferSize = 2*iBufferSize*sizeof(short);
    }
    else
    {
        pParameters.suggestedLatency = 0.8;
        minRingBufferSize = 4*iBufferSize*sizeof(short);
    }

    /* See the specific host's API docs for info on using this field */
    pParameters.hostApiSpecificStreamInfo = nullptr;

    /* flags that can be used to define dither, clip settings and more */
    if (is_capture)
    {
        err = Pa_OpenStream(&stream, &pParameters, nullptr, samplerate,
                            framesPerBuffer, paNoFlag, captureCallback,
                            this);

        if (err != paNoError) {
            //throw string("PortAudio error: ") + Pa_GetErrorText(err);
        }
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            //throw string("PortAudio error: ") + Pa_GetErrorText(err);
        }
    }
    else
    {
        err = Pa_OpenStream(&stream, nullptr, &pParameters, samplerate,
                            framesPerBuffer, paNoFlag, playbackCallback,
                            (void *) this);
        if (err != paNoError) {
            //throw string("PortAudio error: ") + Pa_GetErrorText(err);
        }
    }

    unsigned long n = 2;
    while (n < minRingBufferSize)
        n <<= 2;				/* smallest power of 2 >= requested */

    if (ringBufferData)
        delete[] ringBufferData;
    ringBufferData = new char[n];
    PaUtil_InitializeRingBuffer(&ringBuffer, n, ringBufferData);

    device_changed = false;
    xrun = false;
}

void
CPaCommon::Close()
{
    if (stream)
    {
        int err = Pa_AbortStream(stream);
        if (err != paNoError)
        {
            cout << "PortAudio error: " << Pa_GetErrorText(err) << endl;
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError)
        {
            cout << "PortAudio error: " << Pa_GetErrorText(err) << endl;
        }

        stream = nullptr;
        device_changed = true;
    }
}

bool
CPaCommon::Read(CVector < short >&psData)
{
    if (device_changed)
        ReInit();

    if (stream==nullptr)
        return true;

    size_t bytes = psData.Size() * sizeof(short);

    while (PaUtil_GetRingBufferReadAvailable(&ringBuffer)<int(bytes))
    {
        //cout << "Read: want " << bytes << " avail " << PaUtil_GetRingBufferReadAvailable(&ringBuffer) << endl;
        Pa_Sleep(10);
    }

    PaUtil_ReadRingBuffer(&ringBuffer, &psData[0], bytes);
    if (xruns==0)
        return false;
    else
        cout << "overrun" << endl;
    xruns = 0;
    return true;
}

bool
CPaCommon::Write(CVector < short >&psData)
{
    if (device_changed)
        ReInit();

    if (stream==nullptr)
        return true;

    size_t bytes = psData.Size() * sizeof(short);

    //cout << "Write: got " << bytes << " can put " << PaUtil_GetRingBufferWriteAvailable(&ringBuffer) << endl;
    if (PaUtil_GetRingBufferWriteAvailable(&ringBuffer) < int(bytes))
        return false;			/* TODO use newer data in preference to draining old */

    PaUtil_WriteRingBuffer(&ringBuffer, &psData[0], bytes);
    if (Pa_IsStreamStopped( stream ))
    {
        int err = Pa_StartStream(stream);
        if (err != paNoError) {
            //throw string("PortAudio error: ") + Pa_GetErrorText(err);
        }
    }
    if (xruns==0)
        return false;
    else
        cout << "underrun" << endl;
    xruns = 0;
    return true;
}

CPaIn::CPaIn():hw(true)
{
}

CPaIn::~CPaIn()
{
    Close();
}

bool
CPaIn::Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    return hw.Init(iSampleRate, iNewBufferSize, bNewBlocking);
}

bool
CPaIn::Read(CVector<short>& psData)
{
    return hw.Read(psData);
}

void
CPaIn::Close()
{
    hw.Close();
    cout << "capture close" << endl;
}

CPaOut::CPaOut():hw(false)
{
}

CPaOut::~CPaOut()
{
    Close();
}

bool
CPaOut::Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    return hw.Init(iSampleRate, iNewBufferSize, bNewBlocking);
}

bool
CPaOut::Write(CVector<short>& psData)
{
    return hw.Write(psData);
}

void
CPaOut::Close()
{
    hw.Close();
    cout << "play close" << endl;
}
