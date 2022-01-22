#include "soundout.h"
#include <SLES/OpenSLES.h>

extern SLObjectItf engineObject;

const int MAX_NUMBER_OUTPUT_DEVICES = 10;

COpenSLESOut::COpenSLESOut():currentDevice("")
{
}

COpenSLESOut::~COpenSLESOut()
{

}

void COpenSLESOut::Enumerate(vector<string>& names, vector<string>& descriptions)
{
    SLuint32 OutputDeviceIDs[MAX_NUMBER_OUTPUT_DEVICES];
    SLint32 numOutputs = 0;

    //(*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    /* Get the Audio IO DEVICE CAPABILITIES interface */
    SLAudioIODeviceCapabilitiesItf AudioIODeviceCapabilitiesItf;
    SLresult res = (*engineObject)->GetInterface(engineObject, SL_IID_AUDIOIODEVICECAPABILITIES, (void*)&AudioIODeviceCapabilitiesItf);
    if(res!=SL_RESULT_SUCCESS) {
        string em = "Can't get audio output device capabilities";
        //em += SLESErrorToString(res);
        qDebug(em.c_str());
        return;
    }
    numOutputs = MAX_NUMBER_OUTPUT_DEVICES;
    res = (*AudioIODeviceCapabilitiesItf)->GetAvailableAudioOutputs( AudioIODeviceCapabilitiesItf, &numOutputs, OutputDeviceIDs);
    /* Search for phone earpiece output and phone speaker device */
    for (SLint32 i=0;i<numOutputs; i++)
    {
        SLAudioOutputDescriptor AudioOutputDescriptor;
        res = (*AudioIODeviceCapabilitiesItf)->QueryAudioOutputCapabilities(
                    AudioIODeviceCapabilitiesItf, OutputDeviceIDs[i], &AudioOutputDescriptor
              );
        //CheckErr(res);
        names.push_back((char*)AudioOutputDescriptor.pDeviceName);
        descriptions.push_back("(tbd)");
    }
}

void COpenSLESOut::SetDev(string sNewDevice)
{
    currentDevice = sNewDevice;
}

string COpenSLESOut::GetDev()
{
    return currentDevice;
}

int COpenSLESOut::GetSampleRate()
{
    return 48000;
}

bool COpenSLESOut::Init(int iNewSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    (void)iNewSampleRate;
    (void)iNewBufferSize;
    (void)bNewBlocking;
    return true;
}

bool COpenSLESOut::Write(CVector<short>& psData)
{
    (void)psData;
    return true;
}

void COpenSLESOut::Close()
{

}
