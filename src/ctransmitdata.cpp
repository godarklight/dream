/******************************************************************************\
 * BBC and Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2019
 *
 * Author(s):
 * Volker Fischer, Julian Cable
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

#include "ctransmitdata.h"
#include "Parameter.h"
#include "IQInputFilter.h"
#ifdef QT_MULTIMEDIA_LIB
# include <QSet>
# include <QAudioOutput>
#else
# include "sound/soundinterfacefactory.h"
#endif
using namespace std;

CTransmitData::CTransmitData() : pFileTransmitter(nullptr),
#ifdef QT_MULTIMEDIA_LIB
    pIODevice(nullptr),
#endif
    pSound(nullptr),
    eOutputFormat(OF_REAL_VAL), rDefCarOffset(VIRTUAL_INTERMED_FREQ),
    strOutFileName("test/TransmittedData.txt"), bUseSoundcard(true),
    bAmplified(false), bHighQualityIQ(false)
{

}

void CTransmitData::Stop()
{
#ifdef QT_MULTIMEDIA_LIB
    if(pIODevice!=nullptr) pIODevice->close();
#endif
    if(pSound!=nullptr) pSound->Close();
}

void CTransmitData::Enumerate(vector<string>& names, vector<string>& descriptions, string& defaultOutput)
{
#ifdef QT_MULTIMEDIA_LIB
    QSet<QString> s;
    QString def = QAudioDeviceInfo::defaultOutputDevice().deviceName();
    defaultOutput = def.toStdString();
cerr << "default output device is " << defaultOutput << endl;
    foreach(const QAudioDeviceInfo& di, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        s.insert(di.deviceName());
    }
    names.clear(); descriptions.clear();
    foreach(const QString n, s) {
cerr << "have output device " << n.toStdString() << endl;
        names.push_back(n.toStdString());
        if(n == def) {
            descriptions.push_back("default");
        }
        else {
            descriptions.push_back("");
        }
    }
#else
    if(pSound==nullptr) pSound = CSoundInterfaceFactory::CreateSoundOutInterface();
    pSound->Enumerate(names, descriptions, defaultOutput);    
#endif
    cout << "default output is " << defaultOutput << endl;
}

void CTransmitData::SetSoundInterface(string device)
{
    soundDevice = device;
#ifdef QT_MULTIMEDIA_LIB
    QAudioFormat format;
    if(iSampleRate==0) iSampleRate = 48000; // TODO get initialisation order right
    format.setSampleRate(iSampleRate);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setChannelCount(2); // TODO
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    foreach(const QAudioDeviceInfo& di, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        if(device == di.deviceName().toStdString()) {
            QAudioFormat nearestFormat = di.nearestFormat(format);
            QAudioOutput* pAudioOutput = new QAudioOutput(di, nearestFormat);
            pAudioOutput->setBufferSize(1000000);
            pIODevice = pAudioOutput->start();
            if(pAudioOutput->error()!=QAudio::NoError)
            {
                qDebug("Can't open audio output");
            }
        }
    }
#else
    if(pSound != nullptr) {
        delete pSound;
        pSound = nullptr;
    }
    pSound = CSoundInterfaceFactory::CreateSoundOutInterface();
    pSound->SetDev(device);
#endif
}

void CTransmitData::ProcessDataInternal(CParameter&)
{
    int i;

    /* Apply bandpass filter */
    BPFilter.Process(*pvecInputData);

    /* Convert vector type. Fill vector with symbols (collect them) */
    const int iNs2 = iInputBlockSize * 2;
    for (i = 0; i < iNs2; i += 2)
    {
        const int iCurIndex = iBlockCnt * iNs2 + i;
        _COMPLEX cInputData = (*pvecInputData)[i / 2];

        if (bHighQualityIQ && eOutputFormat != OF_REAL_VAL)
            HilbertFilt(cInputData);

        /* Imaginary, real */
        const _SAMPLE sCurOutReal =
            Real2Sample(cInputData.real() * rNormFactor);
        const _SAMPLE sCurOutImag =
            Real2Sample(cInputData.imag() * rNormFactor);

        /* Envelope, phase */
        const _SAMPLE sCurOutEnv =
            Real2Sample(Abs(cInputData) * 256.0);
        const _SAMPLE sCurOutPhase = /* 2^15 / pi / 2 -> approx. 5000 */
            Real2Sample(Angle(cInputData) * 5000.0);

        switch (eOutputFormat)
        {
        case OF_REAL_VAL:
            /* Use real valued signal as output for both sound card channels */
            vecsDataOut[iCurIndex] = vecsDataOut[iCurIndex + 1] = sCurOutReal;
            break;

        case OF_IQ_POS:
            /* Send inphase and quadrature (I / Q) signal to stereo sound card
               output. I: left channel, Q: right channel */
            vecsDataOut[iCurIndex] = sCurOutReal;
            vecsDataOut[iCurIndex + 1] = sCurOutImag;
            break;

        case OF_IQ_NEG:
            /* Send inphase and quadrature (I / Q) signal to stereo sound card
               output. I: right channel, Q: left channel */
            vecsDataOut[iCurIndex] = sCurOutImag;
            vecsDataOut[iCurIndex + 1] = sCurOutReal;
            break;

        case OF_EP:
            /* Send envelope and phase signal to stereo sound card
               output. Envelope: left channel, Phase: right channel */
            vecsDataOut[iCurIndex] = sCurOutEnv;
            vecsDataOut[iCurIndex + 1] = sCurOutPhase;
            break;
        }
    }

    iBlockCnt++;
    if (iBlockCnt == iNumBlocks)
        FlushData();
}

void CTransmitData::FlushData()
{
    int i;

    /* Zero the remain of the buffer, if incomplete */
    if (iBlockCnt != iNumBlocks)
    {
        const int iSize = vecsDataOut.Size();
        const int iStart = iSize * iBlockCnt / iNumBlocks;
        for (i = iStart; i < iSize; i++)
            vecsDataOut[i] = 0;
    }

    iBlockCnt = 0;

    if (bUseSoundcard)
    {
        /* Write data to sound card. Must be a blocking function */

#ifdef QT_MULTIMEDIA_LIB
        bool bBad = true;
        if(pIODevice)
        {
            qint64 n = 2*vecsDataOut.Size();
            qint64 m = pIODevice->write(reinterpret_cast<char*>(&vecsDataOut[0]), n);
            if(m==n)
                bBad = false;
        }
	if(bBad) {
		cerr << "problem writing to sound card" << endl;
	}
#else
        pSound->Write(vecsDataOut);
#endif
    }
    else
    {
        /* Write data to file */
        for (i = 0; i < iBigBlockSize; i++)
        {
#ifdef FILE_DRM_USING_RAW_DATA
            const short sOut = vecsDataOut[i];

            /* Write 2 bytes, 1 piece */
            fwrite((const void*) &sOut, size_t(2), size_t(1),
                   pFileTransmitter);
#else
            /* This can be read with Matlab "load" command */
            fprintf(pFileTransmitter, "%d\n", vecsDataOut[i]);
#endif
        }

        /* Flush the file buffer */
        fflush(pFileTransmitter);
    }
}

void CTransmitData::InitInternal(CParameter& Parameters)
{
    /*
        float*	pCurFilt;
        int		iNumTapsTransmFilt;
        CReal	rNormCurFreqOffset;
    */
    /* Get signal sample rate */
    iSampleRate = Parameters.GetSigSampleRate();
    /* Define symbol block-size */
    const int iSymbolBlockSize = Parameters.CellMappingTable.iSymbolBlockSize;

    /* Init vector for storing a complete DRM frame number of OFDM symbols */
    iBlockCnt = 0;
    Parameters.Lock();
    iNumBlocks = Parameters.CellMappingTable.iNumSymPerFrame;
    ESpecOcc eSpecOcc = Parameters.GetSpectrumOccup();
    Parameters.Unlock();
    iBigBlockSize = iSymbolBlockSize * 2 /* Stereo */ * iNumBlocks;

    /* Init I/Q history */
    vecrReHist.Init(NUM_TAPS_IQ_INPUT_FILT_HQ, 0.0);

    vecsDataOut.Init(iBigBlockSize);

    if (pFileTransmitter != nullptr)
    {
        fclose(pFileTransmitter);
    }

    if (bUseSoundcard)
    {
        /* Init sound interface */
        if(pSound!=nullptr) pSound->Init(iSampleRate, iBigBlockSize, true);
    }
    else
    {

        /* Open file for writing data for transmitting */
#ifdef FILE_DRM_USING_RAW_DATA
        pFileTransmitter = fopen(strOutFileName.c_str(), "wb");
#else
        pFileTransmitter = fopen(strOutFileName.c_str(), "w");
#endif

        /* Check for error */
        if (pFileTransmitter == nullptr)
            throw CGenErr("The file " + strOutFileName + " cannot be created.");
    }


    /* Init bandpass filter object */
    BPFilter.Init(iSampleRate, iSymbolBlockSize, rDefCarOffset, eSpecOcc, CDRMBandpassFilt::FT_TRANSMITTER);

    /* All robustness modes and spectrum occupancies should have the same output
       power. Calculate the normaization factor based on the average power of
       symbol (the number 3000 was obtained through output tests) */
    rNormFactor = 3000.0 / Sqrt(Parameters.CellMappingTable.rAvPowPerSymbol);

    /* Apply amplification factor, 4.0 = +12dB
       (the maximum without clipping, obtained through output tests) */
    rNormFactor *= bAmplified ? 4.0 : 1.0;

    /* Define block-size for input */
    iInputBlockSize = iSymbolBlockSize;
}

CTransmitData::~CTransmitData()
{
    /* Close file */
    if (pFileTransmitter != nullptr)
        fclose(pFileTransmitter);
}

void CTransmitData::HilbertFilt(_COMPLEX& vecData)
{
    int i;

    /* Move old data */
    for (i = 0; i < NUM_TAPS_IQ_INPUT_FILT_HQ - 1; i++)
        vecrReHist[i] = vecrReHist[i + 1];

    vecrReHist[NUM_TAPS_IQ_INPUT_FILT_HQ - 1] = vecData.real();

    /* Filter */
    _REAL rSum = 0.0;
    for (i = 1; i < NUM_TAPS_IQ_INPUT_FILT_HQ; i += 2)
        rSum += _REAL(fHilFiltIQ_HQ[i]) * vecrReHist[i];

    vecData = _COMPLEX(vecrReHist[IQ_INP_HIL_FILT_DELAY_HQ], -rSum);
}
