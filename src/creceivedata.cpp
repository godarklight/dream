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

#include "creceivedata.h"
#include "Parameter.h"
#ifdef QT_MULTIMEDIA_LIB
# include <QSet>
# include <QThread>
# include <QTimer>
# include <QEventLoop>
#else
# include "sound/soundinterfacefactory.h"
#endif
#include "sound/audiofilein.h"
#include "util/FileTyper.h"
#include "IQInputFilter.h"
#include "UpsampleFilter.h"
#include "matlib/MatlibSigProToolbox.h"
#include "tuner.h"

using namespace std;

const static int SineTable[] = { 0, 1, 0, -1, 0 };
const static _REAL PSDWindowGain = 0.39638; /* power gain of the Hamming window */

//inline _REAL sample2real(_SAMPLE s) { return _REAL(s)/32768.0; }
inline _REAL sample2real(_SAMPLE s) {
    return _REAL(s);
}

CReceiveData::CReceiveData() :
#ifdef QT_MULTIMEDIA_LIB
    pIODevice(nullptr),
#endif
    pSound(nullptr),
    vecrInpData(INPUT_DATA_VECTOR_SIZE, 0.0),
    bFippedSpectrum(false), eInChanSelection(CS_MIX_CHAN), iPhase(0),spectrumAnalyser()
{}

CReceiveData::~CReceiveData()
{
}

void CReceiveData::Stop()
{
#ifdef QT_MULTIMEDIA_LIB
    if(pIODevice!=nullptr) {
        pIODevice->close();
        pIODevice = nullptr;
    }
    if(pAudioInput != nullptr) {
        pAudioInput->stop();
        delete pAudioInput;
        pAudioInput = nullptr;
    }
#endif
    if(pSound!=nullptr) {
        pSound->Close();
        pSound = nullptr;
    }
}

void CReceiveData::Enumerate(vector<string>& names, vector<string>& descriptions, string& defaultInput)
{
#ifdef QT_MULTIMEDIA_LIB
    QSet<QString> s;
    QString def = QAudioDeviceInfo::defaultInputDevice().deviceName();
    defaultInput = def.toStdString();
    foreach(const QAudioDeviceInfo& di, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        s.insert(di.deviceName());
    }
    names.clear(); descriptions.clear();
    foreach(const QString n, s) {
        names.push_back(n.toStdString());
        if(n == def) {
            descriptions.push_back("default");
        }
        else {
            descriptions.push_back("");
        }
    }
#else
    if(pSound==nullptr) pSound = CSoundInterfaceFactory::CreateSoundInInterface();
    pSound->Enumerate(names, descriptions, defaultInput);
#endif
}

void
CReceiveData::SetSoundInterface(string device)
{
    soundDevice = device;
    if(pSound != nullptr) {
        pSound->Close();
        delete pSound;
        pSound = nullptr;
    }
    if(FileTyper::resolve(device) != FileTyper::unrecognised) {
        CAudioFileIn* pAudioFileIn = new CAudioFileIn();
        pAudioFileIn->SetFileName(device);
        int sr = pAudioFileIn->GetSampleRate();
        if(iSampleRate!=sr) {
            // TODO
            cerr << "file sample rate is " << sr << endl;
            iSampleRate = sr;
        }
        pSound = pAudioFileIn;
#ifdef QT_MULTIMEDIA_LIB
        if(pIODevice!=nullptr) {
            pIODevice->close();
            pIODevice = nullptr;
        }
        if(pAudioInput != nullptr) {
            pAudioInput->stop();
            delete pAudioInput;
            pAudioInput = nullptr;
        }
#endif
    }
    else {
#ifdef QT_MULTIMEDIA_LIB
        QAudioFormat format;
        if(iSampleRate==0) iSampleRate = 48000; // TODO get order of initialisation correct
        format.setSampleRate(iSampleRate);
        format.setSampleSize(16);
        format.setSampleType(QAudioFormat::SignedInt);
        format.setChannelCount(2); // TODO
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setCodec("audio/pcm");
        foreach(const QAudioDeviceInfo& di, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
        {
            if(device == di.deviceName().toStdString()) {
                QAudioFormat nearestFormat = di.nearestFormat(format);
                pAudioInput = new QAudioInput(di, nearestFormat);
                break;
            }
        }
        if(pAudioInput == nullptr) {
            qDebug("can't find audio input %s", device.c_str());
        }
#else
        pSound = CSoundInterfaceFactory::CreateSoundInInterface();
        pSound->SetDev(device);
#endif
    }
}

void CReceiveData::ProcessDataInternal(CParameter& Parameters)
{
    int i;

    /* OPH: update free-running symbol counter */
    Parameters.Lock();

    iFreeSymbolCounter++;
    if (iFreeSymbolCounter >= Parameters.CellMappingTable.iNumSymPerFrame * 2) /* x2 because iOutputBlockSize=iSymbolBlockSize/2 */
    {
        iFreeSymbolCounter = 0;
        /* calculate the PSD once per frame for the RSI output */
        if (Parameters.bMeasurePSD) {
            emitRSCIData(Parameters);
        }
    }
    Parameters.Unlock();


    /* Get data from sound interface. The read function must be a
       blocking function! */

#ifdef QT_MULTIMEDIA_LIB
    bool bBad = false;
    if(pIODevice)
    {
#if 0

        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        QObject::connect(pIODevice,  SIGNAL(readyRead()), &loop, SLOT(quit()) );
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start(1000);
        loop.exec();

        if(timer.isActive()) {
            //qDebug("data from sound card");
            qint64 n = 2*vecsSoundBuffer.Size();
            qint64 r = pIODevice->read(reinterpret_cast<char*>(&vecsSoundBuffer[0]), n);
            if(r!=n) {
                cerr << "short read" << endl;
            }
        }
        else {
            qDebug("timeout");
        }

#else
        qint64 n = 2*vecsSoundBuffer.Size();
        char *p = reinterpret_cast<char*>(&vecsSoundBuffer[0]);
        do {
            qint64 r = pIODevice->read(p, n);
            if(r>0) {
                p += r;
                n -= r;
            }
            else {
                QThread::msleep(100);
            }
        } while (n>0);
#endif
    }
    else if (pSound != nullptr) { // for audio files
        bBad = pSound->Read(vecsSoundBuffer, Parameters);
    }
    else {
      bBad = true;
    }
#else
    bool bBad = true;
    if (pSound != nullptr)
    {
        bBad = pSound->Read(vecsSoundBuffer, Parameters);
    }
#endif

    Parameters.Lock();
    Parameters.ReceiveStatus.InterfaceI.SetStatus(bBad ? CRC_ERROR : RX_OK); /* Red light */
    Parameters.Unlock();

    if(bBad)
        return;

    /* Upscale if ratio greater than one */
    if (iUpscaleRatio > 1)
    {
        /* The actual upscaling, currently only 2X is supported */
        InterpFIR_2X(2, &vecsSoundBuffer[0], vecf_ZL, vecf_YL, vecf_B);
        InterpFIR_2X(2, &vecsSoundBuffer[1], vecf_ZR, vecf_YR, vecf_B);
    }
    else if (iDownscaleRatio > 1)
    {
        /* The actual downscaling, currently only 2X is supported */
        DecimFIR_2X(2, &vecsSoundBuffer[0], vecf_ZL, vecf_YL, vecf_B);
        DecimFIR_2X(2, &vecsSoundBuffer[1], vecf_ZR, vecf_YR, vecf_B);
    }
    if (iUpscaleRatio > 1 || iDownscaleRatio >1)
    {

        /* Write data to output buffer. Do not set the switch command inside
           the for-loop for efficiency reasons */
        switch (eInChanSelection)
        {
        case CS_LEFT_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
                (*pvecOutputData)[i] = _REAL(vecf_YL[unsigned(i)]);
            break;

        case CS_RIGHT_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
                (*pvecOutputData)[i] = _REAL(vecf_YR[unsigned(i)]);
            break;

        case CS_MIX_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Mix left and right channel together */
                (*pvecOutputData)[i] = _REAL(vecf_YL[unsigned(i)] + vecf_YR[unsigned(i)]) / 2.0;
            }
            break;

        case CS_SUB_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Subtract right channel from left */
                (*pvecOutputData)[i] = _REAL(vecf_YL[unsigned(i)] - vecf_YR[unsigned(i)]) / 2.0;
            }
            break;

        /* I / Q input */
        case CS_IQ_POS:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                (*pvecOutputData)[i] = HilbertFilt(_REAL(vecf_YL[unsigned(i)]), _REAL(vecf_YR[unsigned(i)]));
            }
            break;

        case CS_IQ_NEG:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                (*pvecOutputData)[i] = HilbertFilt(_REAL(vecf_YR[unsigned(i)]), _REAL(vecf_YL[unsigned(i)]));
            }
            break;

        case CS_IQ_POS_ZERO:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Shift signal to vitual intermediate frequency before applying
                   the Hilbert filtering */
                _COMPLEX cCurSig = _COMPLEX(_REAL(vecf_YL[unsigned(i)]), _REAL(vecf_YR[unsigned(i)]));

                cCurSig *= cCurExp;

                /* Rotate exp-pointer on step further by complex multiplication
                   with precalculated rotation vector cExpStep */
                cCurExp *= cExpStep;

                (*pvecOutputData)[i] =
                    HilbertFilt(cCurSig.real(), cCurSig.imag());
            }
            break;

        case CS_IQ_NEG_ZERO:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Shift signal to vitual intermediate frequency before applying
                   the Hilbert filtering */
                _COMPLEX cCurSig = _COMPLEX(_REAL(vecf_YR[unsigned(i)]), _REAL(vecf_YL[unsigned(i)]));

                cCurSig *= cCurExp;

                /* Rotate exp-pointer on step further by complex multiplication
                   with precalculated rotation vector cExpStep */
                cCurExp *= cExpStep;

                (*pvecOutputData)[i] =
                    HilbertFilt(cCurSig.real(), cCurSig.imag());
            }
            break;

        case CS_IQ_POS_SPLIT:
            for (i = 0; i < iOutputBlockSize; i += 4)
            {
                (*pvecOutputData)[i + 0] =  _REAL(vecf_YL[unsigned(i + 0)]);
                (*pvecOutputData)[i + 1] = _REAL(-vecf_YR[unsigned(i + 1)]);
                (*pvecOutputData)[i + 2] = _REAL(-vecf_YL[unsigned(i + 2)]);
                (*pvecOutputData)[i + 3] =  _REAL(vecf_YR[unsigned(i + 3)]);
            }
            break;

        case CS_IQ_NEG_SPLIT:
            for (i = 0; i < iOutputBlockSize; i += 4)
            {
                (*pvecOutputData)[i + 0] =  _REAL(vecf_YR[unsigned(i + 0)]);
                (*pvecOutputData)[i + 1] = _REAL(-vecf_YL[unsigned(i + 1)]);
                (*pvecOutputData)[i + 2] = _REAL(-vecf_YR[unsigned(i + 2)]);
                (*pvecOutputData)[i + 3] =  _REAL(vecf_YL[unsigned(i + 3)]);
            }
            break;
        }
    }

    /* Upscale ratio equal to one */
    else {
        /* Write data to output buffer. Do not set the switch command inside
           the for-loop for efficiency reasons */
        switch (eInChanSelection)
        {
        case CS_LEFT_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
                (*pvecOutputData)[i] = sample2real(vecsSoundBuffer[2 * i]);
            break;

        case CS_RIGHT_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
                (*pvecOutputData)[i] = sample2real(vecsSoundBuffer[2 * i + 1]);
            break;

        case CS_MIX_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Mix left and right channel together */
                const _REAL rLeftChan = sample2real(vecsSoundBuffer[2 * i]);
                const _REAL rRightChan = sample2real(vecsSoundBuffer[2 * i + 1]);
                (*pvecOutputData)[i] = (rLeftChan + rRightChan) / 2;
            }
            break;

        case CS_SUB_CHAN:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Subtract right channel from left */
                const _REAL rLeftChan = sample2real(vecsSoundBuffer[2 * i]);
                const _REAL rRightChan = sample2real(vecsSoundBuffer[2 * i + 1]);
                (*pvecOutputData)[i] = (rLeftChan - rRightChan) / 2;
            }
            break;

        /* I / Q input */
        case CS_IQ_POS:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                (*pvecOutputData)[i] =
                    HilbertFilt(sample2real(vecsSoundBuffer[2 * i]),
                                sample2real(vecsSoundBuffer[2 * i + 1]));
            }
            break;

        case CS_IQ_NEG:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                (*pvecOutputData)[i] =
                    HilbertFilt(sample2real(vecsSoundBuffer[2 * i + 1]),
                                sample2real(vecsSoundBuffer[2 * i]));
            }
            break;

        case CS_IQ_POS_ZERO:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Shift signal to vitual intermediate frequency before applying
                   the Hilbert filtering */
                _COMPLEX cCurSig = _COMPLEX(sample2real(vecsSoundBuffer[2 * i]),
                                            sample2real(vecsSoundBuffer[2 * i + 1]));

                cCurSig *= cCurExp;

                /* Rotate exp-pointer on step further by complex multiplication
                   with precalculated rotation vector cExpStep */
                cCurExp *= cExpStep;

                (*pvecOutputData)[i] =
                    HilbertFilt(cCurSig.real(), cCurSig.imag());
            }
            break;

        case CS_IQ_NEG_ZERO:
            for (i = 0; i < iOutputBlockSize; i++)
            {
                /* Shift signal to vitual intermediate frequency before applying
                   the Hilbert filtering */
                _COMPLEX cCurSig = _COMPLEX(sample2real(vecsSoundBuffer[2 * i + 1]),
                                            sample2real(vecsSoundBuffer[2 * i]));

                cCurSig *= cCurExp;

                /* Rotate exp-pointer on step further by complex multiplication
                   with precalculated rotation vector cExpStep */
                cCurExp *= cExpStep;

                (*pvecOutputData)[i] =
                    HilbertFilt(cCurSig.real(), cCurSig.imag());
            }
            break;

        case CS_IQ_POS_SPLIT: /* Require twice the bandwidth */
            for (i = 0; i < iOutputBlockSize; i++)
            {
                iPhase = (iPhase + 1) & 3;
                _REAL rValue = vecsSoundBuffer[2 * i]     * /*COS*/SineTable[iPhase + 1] -
                               vecsSoundBuffer[2 * i + 1] * /*SIN*/SineTable[iPhase];
                (*pvecOutputData)[i] = rValue;
            }
            break;

        case CS_IQ_NEG_SPLIT: /* Require twice the bandwidth */
            for (i = 0; i < iOutputBlockSize; i++)
            {
                iPhase = (iPhase + 1) & 3;
                _REAL rValue = vecsSoundBuffer[2 * i + 1] * /*COS*/SineTable[iPhase + 1] -
                               vecsSoundBuffer[2 * i]     * /*SIN*/SineTable[iPhase];
                (*pvecOutputData)[i] = rValue;
            }
            break;
        }
    }

    /* Flip spectrum if necessary ------------------------------------------- */
    if (bFippedSpectrum)
    {
        /* Since iOutputBlockSize is always even we can do some opt. here */
        for (i = 0; i < iOutputBlockSize; i+=2)
        {
            /* We flip the spectrum by using the mirror spectrum at the negative
               frequencys. If we shift by half of the sample frequency, we can
               do the shift without the need of a Hilbert transformation */
            (*pvecOutputData)[i] = -(*pvecOutputData)[i];
        }
    }

    /* Copy data in buffer for spectrum calculation */
    mutexInpData.Lock();
    vecrInpData.AddEnd((*pvecOutputData), iOutputBlockSize);
    mutexInpData.Unlock();

    /* Update level meter */
    SignalLevelMeter.Update((*pvecOutputData));
    Parameters.Lock();
    Parameters.SetIFSignalLevel(SignalLevelMeter.Level());
    Parameters.Unlock();

}

void CReceiveData::InitInternal(CParameter& Parameters)
{
    /* Init sound interface. Set it to one symbol. The sound card interface
           has to taken care about the buffering data of a whole MSC block.
           Use stereo input (* 2) */

#ifdef QT_MULTIMEDIA_LIB
    if (pSound == nullptr && pAudioInput == nullptr)
        return;
#else
    if (pSound == nullptr)
        return;
#endif

    Parameters.Lock();
    /* We define iOutputBlockSize as half the iSymbolBlockSize because
       if a positive frequency offset is present in drm signal,
       after some time a buffer overflow occur in the output buffer of
       InputResample.ProcessData() */
    /* Define output block-size */
    iOutputBlockSize = Parameters.CellMappingTable.iSymbolBlockSize / 2;
    iMaxOutputBlockSize = iOutputBlockSize * 2;
    /* Get signal sample rate */
    iSampleRate = Parameters.GetSigSampleRate();
    iUpscaleRatio = Parameters.GetSigUpscaleRatio();
    iDownscaleRatio = 2; // TODO read from params
    Parameters.Unlock();

    const int iOutputBlockAlignment = iOutputBlockSize & 3;
    if (iOutputBlockAlignment) {
        fprintf(stderr, "CReceiveData::InitInternal(): iOutputBlockAlignment = %i\n", iOutputBlockAlignment);
    }

    try {


        bool bChanged = false;
        int wantedBufferSize = iOutputBlockSize * 2 * iDownscaleRatio / iUpscaleRatio; // samples

#ifdef QT_MULTIMEDIA_LIB
        if(pSound) { // must be sound file
            bChanged = (pSound==nullptr)?true:pSound->Init(iSampleRate / iUpscaleRatio, wantedBufferSize, true);
        }
        else {
            pAudioInput->setBufferSize(2*wantedBufferSize); // bytes * expected frame imput size
            pIODevice = pAudioInput->start();
            cerr << "sound card buffer size requested " << 2*wantedBufferSize << " actual " << pAudioInput->bufferSize() << endl;
            if(pAudioInput->error()!=QAudio::NoError)
            {
                qDebug("Can't open audio input");
            }
            bChanged = true; // TODO
        }

#else
        bChanged = (pSound==nullptr)?true:pSound->Init(iSampleRate * iDownscaleRatio / iUpscaleRatio, wantedBufferSize, true);
#endif
        /* Clear input data buffer on change samplerate change */
        if (bChanged)
            ClearInputData();

        /* Init 2X upscaler if enabled */
        if (iUpscaleRatio > 1)
        {
            const int taps = (NUM_TAPS_UPSAMPLE_FILT + 3) & ~3;
            vecf_B.resize(taps, 0.0f);
            for (unsigned i = 0; i < NUM_TAPS_UPSAMPLE_FILT; i++)
                vecf_B[i] = float(dUpsampleFilt[i] * iUpscaleRatio);
            if (bChanged)
            {
                vecf_ZL.resize(0);
                vecf_ZR.resize(0);
            }
            vecf_ZL.resize(unsigned(iOutputBlockSize + taps) / 2, 0.0f);
            vecf_ZR.resize(unsigned(iOutputBlockSize + taps) / 2, 0.0f);
            vecf_YL.resize(unsigned(iOutputBlockSize));
            vecf_YR.resize(unsigned(iOutputBlockSize));
        }
        else if (iDownscaleRatio > 1)
        {
            const int taps = (NUM_TAPS_DOWNSAMPLE_FILT + 3) & ~3;
            vecf_B.resize(taps, 0.0f);
            for (unsigned i = 0; i < NUM_TAPS_DOWNSAMPLE_FILT; i++)
                vecf_B[i] = float(dDownsampleFilt[i] / iDownscaleRatio);
            if (bChanged)
            {
                vecf_ZL.resize(0);
                vecf_ZR.resize(0);
            }
            vecf_ZL.resize(unsigned(iOutputBlockSize * 2 + taps), 0.0f);
            vecf_ZR.resize(unsigned(iOutputBlockSize *2 + taps), 0.0f);
            vecf_YL.resize(unsigned(iOutputBlockSize));
            vecf_YR.resize(unsigned(iOutputBlockSize));
        }
        else
        {
            vecf_B.resize(0);
            vecf_YL.resize(0);
            vecf_YR.resize(0);
            vecf_ZL.resize(0);
            vecf_ZR.resize(0);
        }

        /* Init buffer size for taking stereo input */
        vecsSoundBuffer.Init(iOutputBlockSize * 2 * iDownscaleRatio / iUpscaleRatio);

        /* Init signal meter */
        SignalLevelMeter.Init(0);

        /* Inits for I / Q input, only if it is not already
           to keep the history intact */
        if (vecrReHist.Size() != NUM_TAPS_IQ_INPUT_FILT || bChanged)
        {
            vecrReHist.Init(NUM_TAPS_IQ_INPUT_FILT, 0.0);
            vecrImHist.Init(NUM_TAPS_IQ_INPUT_FILT, 0.0);
        }

        /* Start with phase null (can be arbitrarily chosen) */
        cCurExp = 1.0;

        /* Set rotation vector to mix signal from zero frequency to virtual
           intermediate frequency */
        const _REAL rNormCurFreqOffsetIQ = 2.0 * crPi * VIRTUAL_INTERMED_FREQ / _REAL(iSampleRate);

        cExpStep = _COMPLEX(cos(rNormCurFreqOffsetIQ), sin(rNormCurFreqOffsetIQ));


        /* OPH: init free-running symbol counter */
        iFreeSymbolCounter = 0;

    }
    catch (CGenErr GenErr)
    {
        pSound = nullptr;
    }
    catch (string strError)
    {
        pSound = nullptr;
    }
}

_REAL CReceiveData::HilbertFilt(const _REAL rRe, const _REAL rIm)
{
    /*
        Hilbert filter for I / Q input data. This code is based on code written
        by Cesco (HB9TLK)
    */

    /* Move old data */
    for (int i = 0; i < NUM_TAPS_IQ_INPUT_FILT - 1; i++)
    {
        vecrReHist[i] = vecrReHist[i + 1];
        vecrImHist[i] = vecrImHist[i + 1];
    }

    vecrReHist[NUM_TAPS_IQ_INPUT_FILT - 1] = rRe;
    vecrImHist[NUM_TAPS_IQ_INPUT_FILT - 1] = rIm;

    /* Filter */
    _REAL rSum = 0.0;
    for (unsigned i = 1; i < NUM_TAPS_IQ_INPUT_FILT; i += 2)
        rSum += _REAL(fHilFiltIQ[i]) * vecrImHist[int(i)];

    return (rSum + vecrReHist[IQ_INP_HIL_FILT_DELAY]) / 2;
}

void CReceiveData::InterpFIR_2X(const int channels, _SAMPLE* X, vector<float>& Z, vector<float>& Y, vector<float>& B)
{
    /*
        2X interpolating filter. When combined with CS_IQ_POS_SPLIT or CS_IQ_NEG_SPLIT
        input data mode, convert I/Q input to full bandwidth, code by David Flamand
    */
    int i, j;
    const int B_len = int(B.size());
    const int Z_len = int(Z.size());
    const int Y_len = int(Y.size());
    const int Y_len_2 = Y_len / 2;
    float *B_beg_ptr = &B[0];
    float *Z_beg_ptr = &Z[0];
    float *Y_ptr = &Y[0];
    float *B_end_ptr, *B_ptr, *Z_ptr;
    float y0, y1, y2, y3;

    /* Check for size and alignment requirement */
    if ((B_len & 3) || (Z_len != (B_len/2 + Y_len_2)) || (Y_len & 1))
        return;

    /* Copy the old history at the end */
    for (i = B_len/2-1; i >= 0; i--)
        Z_beg_ptr[Y_len_2 + i] = Z_beg_ptr[i];

    /* Copy the new sample at the beginning of the history */
    for (i = 0, j = 0; i < Y_len_2; i++, j+=channels)
        Z_beg_ptr[Y_len_2 - i - 1] = X[j];

    /* The actual lowpass filtering using FIR */
    for (i = Y_len_2-1; i >= 0; i--)
    {
        B_end_ptr  = B_beg_ptr + B_len;
        B_ptr      = B_beg_ptr;
        Z_ptr      = Z_beg_ptr + i;
        y0 = y1 = y2 = y3 = 0.0f;
        while (B_ptr != B_end_ptr)
        {
            y0 = y0 + B_ptr[0] * Z_ptr[0];
            y1 = y1 + B_ptr[1] * Z_ptr[0];
            y2 = y2 + B_ptr[2] * Z_ptr[1];
            y3 = y3 + B_ptr[3] * Z_ptr[1];
            B_ptr += 4;
            Z_ptr += 2;
        }
        *Y_ptr++ = y0 + y2;
        *Y_ptr++ = y1 + y3;
    }
}

void CReceiveData::DecimFIR_2X(const int channels, _SAMPLE* X, vector<float>& Z, vector<float>& Y, vector<float>& B)
{
    /*
        2X decimating filter.
    */
    int i, j;
    const int B_len = int(B.size());
    const int Z_len = int(Z.size());
    const int Y_len = int(Y.size());
    const int Y_len_2 = Y_len * 2;
    float *B_beg_ptr = &B[0];
    float *Z_beg_ptr = &Z[0];
    float *Y_ptr = &Y[0];
    float *B_end_ptr, *B_ptr, *Z_ptr;
    float y0, y1, y2, y3;

    /* Check for size and alignment requirement */
    if ((B_len & 3) || Z_len != (B_len + Y_len_2))
        return;

    /* Copy the old history at the end */
    for (i = B_len-1; i >= 0; i--)
        Z_beg_ptr[Y_len_2 + i] = Z_beg_ptr[i];

    /* Copy the new sample at the beginning of the history */
    for (i = 0, j = 0; i < Y_len_2; i++, j+=channels)
        Z_beg_ptr[Y_len_2 - i - 1] = X[j];

    /* The actual lowpass filtering using FIR */
    for (i = Y_len_2-2; i >= 0; i-=2)
    {
        B_end_ptr  = B_beg_ptr + B_len;
        B_ptr      = B_beg_ptr;
        Z_ptr      = Z_beg_ptr + i;
        y0 = y1 = y2 = y3 = 0.0f;
        while (B_ptr != B_end_ptr)
        {
            y0 = y0 + B_ptr[0] * Z_ptr[0];
            y1 = y1 + B_ptr[1] * Z_ptr[1];
            y2 = y2 + B_ptr[2] * Z_ptr[2];
            y3 = y3 + B_ptr[3] * Z_ptr[3];

            B_ptr += 4;
            Z_ptr += 4;
        }
        *Y_ptr++ = y0 + y1 + y2 + y3;
    }
}

/*
    Convert Real to I/Q frequency when bInvert is false
    Convert I/Q to Real frequency when bInvert is true
*/
_REAL CReceiveData::ConvertFrequency(_REAL rFrequency, bool bInvert) const
{
    const int iInvert = bInvert ? -1 : 1;

    if (eInChanSelection == CS_IQ_POS_SPLIT ||
            eInChanSelection == CS_IQ_NEG_SPLIT)
        rFrequency -= iSampleRate / 4 * iInvert;
    else if (eInChanSelection == CS_IQ_POS_ZERO ||
             eInChanSelection == CS_IQ_NEG_ZERO)
        rFrequency -= VIRTUAL_INTERMED_FREQ * iInvert;

    return rFrequency;
}

void CReceiveData::GetInputSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    spectrumAnalyser.setNegativeFrequency(eInChanSelection == CS_IQ_POS_SPLIT || eInChanSelection == CS_IQ_NEG_SPLIT);
    spectrumAnalyser.setOffsetFrequency((eInChanSelection == CS_IQ_POS_ZERO) || (eInChanSelection == CS_IQ_NEG_ZERO));
    mutexInpData.Lock();
    spectrumAnalyser.CalculateSpectrum(vecrInpData, NUM_SMPLS_4_INPUT_SPECTRUM);
    mutexInpData.Unlock();
    /* The calibration factor of 18.49 was determined experimentaly,
       give 0 dB for a full scale sine wave input (0 dBFS) */

    const _REAL rNormData = pow(_REAL(_MAXSHORT) * _REAL(NUM_SMPLS_4_INPUT_SPECTRUM),2) / 18.49;
    spectrumAnalyser.PSD2LogPSD(rNormData, vecrData, vecrScale);
}

void CReceiveData::GetInputPSD(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale,
                 const int iLenPSDAvEachBlock,
                 const int iNumAvBlocksPSD,
                 const int iPSDOverlap)
{
    spectrumAnalyser.setNegativeFrequency(eInChanSelection == CS_IQ_POS_SPLIT || eInChanSelection == CS_IQ_NEG_SPLIT);
    spectrumAnalyser.setOffsetFrequency((eInChanSelection == CS_IQ_POS_ZERO) || (eInChanSelection == CS_IQ_NEG_ZERO));
    mutexInpData.Lock();
    spectrumAnalyser.CalculateLinearPSD(vecrInpData, iLenPSDAvEachBlock, iNumAvBlocksPSD, iPSDOverlap);
    mutexInpData.Unlock();

    const _REAL rNormData =  pow(_REAL(_MAXSHORT) * _REAL(iLenPSDAvEachBlock), 2) * _REAL(iNumAvBlocksPSD) * PSDWindowGain;

    spectrumAnalyser.PSD2LogPSD(rNormData, vecrData, vecrScale);
}

void CReceiveData::emitRSCIData(CParameter& Parameters)
{
    /* Init the constants for scale and normalization */
    spectrumAnalyser.setNegativeFrequency(eInChanSelection == CS_IQ_POS_SPLIT || eInChanSelection == CS_IQ_NEG_SPLIT);
    spectrumAnalyser.setOffsetFrequency((eInChanSelection == CS_IQ_POS_ZERO) || (eInChanSelection == CS_IQ_NEG_ZERO));
    mutexInpData.Lock();
    spectrumAnalyser.CalculateLinearPSD(vecrInpData, LEN_PSD_AV_EACH_BLOCK_RSI, NUM_AV_BLOCKS_PSD_RSI, PSD_OVERLAP_RSI);
    mutexInpData.Unlock();


    const _REAL rNormData =  pow(_REAL(_MAXSHORT) * _REAL(LEN_PSD_AV_EACH_BLOCK_RSI), 2) * _REAL(NUM_AV_BLOCKS_PSD_RSI) * PSDWindowGain;

    CVector<_REAL>		vecrData;
    CVector<_REAL>		vecrScale;
    spectrumAnalyser.PSD2LogPSD(rNormData, vecrData, vecrScale);

    /* Data required for rpsd tag */
    /* extract the values from -8kHz to +8kHz/18kHz relative to 12kHz, i.e. 4kHz to 20kHz */
    /*const int startBin = 4000.0 * LEN_PSD_AV_EACH_BLOCK_RSI /iSampleRate;
    const int endBin = 20000.0 * LEN_PSD_AV_EACH_BLOCK_RSI /iSampleRate;*/
    /* The above calculation doesn't round in the way FhG expect. Probably better to specify directly */

    /* For 20k mode, we need -8/+18, which is more than the Nyquist rate of 24kHz. */
    /* Assume nominal freq = 7kHz (i.e. 2k to 22k) and pad with zeroes (roughly 1kHz each side) */

    int iStartBin = 22;
    int iEndBin = 106;
    int iVecSize = iEndBin - iStartBin + 1; //85

    //_REAL rIFCentreFrequency = Parameters.FrontEndParameters.rIFCentreFreq;

    ESpecOcc eSpecOcc = Parameters.GetSpectrumOccup();
    if (eSpecOcc == SO_4 || eSpecOcc == SO_5)
    {
        iStartBin = 0;
        iEndBin = 127;
        iVecSize = 139;
    }
    /* Line up the the middle of the vector with the quarter-Nyquist bin of FFT */
    int iStartIndex = iStartBin - (LEN_PSD_AV_EACH_BLOCK_RSI/4) + (iVecSize-1)/2;

    /* Fill with zeros to start with */
    Parameters.vecrPSD.Init(iVecSize, 0.0);

    for (int i=iStartIndex, j=iStartBin; j<=iEndBin; i++,j++)
        Parameters.vecrPSD[i] = vecrData[j];

    spectrumAnalyser.CalculateSigStrengthCorrection(Parameters, vecrData);

    spectrumAnalyser.CalculatePSDInterferenceTag(Parameters, vecrData);
}

CTuner * CReceiveData::GetTuner()
{
    fprintf(stderr, "CReceiveData::GetTuner() called, pSound=%x\n", pSound);
    return dynamic_cast<CTuner *>(pSound);
}


