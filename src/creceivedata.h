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

#ifndef CRECEIVEDATA_H
#define CRECEIVEDATA_H

#include "util/Modul.h"
#include "sound/soundinterface.h"
#include "util/Utilities.h"
#include "spectrumanalyser.h"
#ifdef QT_MULTIMEDIA_LIB
# include <QAudioInput>
# include <QIODevice>
#else
# ifdef QT_CORE_LIB
  class QIODevice;
# endif
#endif

  /* Length of vector for input spectrum. We use approx. 0.2 sec
     of sampled data for spectrum calculation, this is 2^13 = 8192 to
     make the FFT work more efficient. Make sure that this number is not smaller
     than the symbol length in 48 kHz domain of longest mode (which is mode A/B:
     1280) */
#define NUM_SMPLS_4_INPUT_SPECTRUM (NUM_AV_BLOCKS_PSD * LEN_PSD_AV_EACH_BLOCK)

/* Number of FFT blocks used for averaging. See next definition
 ("NUM_SMPLS_4_INPUT_SPECTRUM") for how to set the parameters */
#define NUM_AV_BLOCKS_PSD			16
#define LEN_PSD_AV_EACH_BLOCK		512

/* same but for the rpsd tag */
#define NUM_AV_BLOCKS_PSD_RSI	150
#define LEN_PSD_AV_EACH_BLOCK_RSI		256
#define PSD_OVERLAP_RSI	128
/* The RSI output needs 400ms with a 50% overlap, so this needs more space
   I think the RSCI spec is slightly wrong - using 150 windows consumes just over 400ms, 149 would be exact */
#define INPUT_DATA_VECTOR_SIZE (NUM_AV_BLOCKS_PSD_RSI * (LEN_PSD_AV_EACH_BLOCK_RSI-PSD_OVERLAP_RSI)+PSD_OVERLAP_RSI)

class CTuner;

enum EInChanSel {CS_LEFT_CHAN, CS_RIGHT_CHAN, CS_MIX_CHAN, CS_SUB_CHAN, CS_IQ_POS,
                   CS_IQ_NEG, CS_IQ_POS_ZERO, CS_IQ_NEG_ZERO, CS_IQ_POS_SPLIT, CS_IQ_NEG_SPLIT
                  };

class CReceiveData : public CReceiverModul<_REAL, _REAL>
{
public:
    CReceiveData();
    virtual ~CReceiveData();

    _REAL ConvertFrequency(_REAL rFrequency, bool bInvert=false) const;

    void GetInputSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);

    void SetFlippedSpectrum(const bool bNewF) {
        bFippedSpectrum = bNewF;
    }

    bool GetFlippedSpectrum() {
        return bFippedSpectrum;
    }

    void ClearInputData() {
        mutexInpData.Lock();
        vecrInpData.Init(INPUT_DATA_VECTOR_SIZE, 0.0);
        mutexInpData.Unlock();
    }

    void SetSoundInterface(std::string);
    std::string GetSoundInterface() { return soundDevice; }
    void Enumerate(std::vector<string>& names, std::vector<string>& descriptions, std::string& defaultInput);
    void Stop();
#ifdef QT_MULTIMEDIA_LIB
    std::string GetSoundInterfaceVersion() { return "QtMultimedia"; }
#else
    std::string GetSoundInterfaceVersion() { return pSound->GetVersion(); }
#endif
    void SetInChanSel(const EInChanSel eNS) {
        eInChanSelection = eNS;
    }
    EInChanSel GetInChanSel() {
        return eInChanSelection;
    }

    void GetInputPSD(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale,
                     const int iLenPSDAvEachBlock = LEN_PSD_AV_EACH_BLOCK,
                     const int iNumAvBlocksPSD = NUM_AV_BLOCKS_PSD,
                     const int iPSDOverlap = 0);

    CTuner *GetTuner(void);

protected:
    CSignalLevelMeter		SignalLevelMeter;

#ifdef QT_MULTIMEDIA_LIB
    QAudioInput*            pAudioInput;
    QIODevice*              pIODevice;
#endif
    CSoundInInterface*		pSound;
    CVector<_SAMPLE>		vecsSoundBuffer;
    std::string             soundDevice;

    /* Access to vecrInpData buffer must be done inside a mutex */
    CShiftRegister<_REAL>	vecrInpData;
    CMutex                  mutexInpData;

    int                     iSampleRate;
    bool                    bFippedSpectrum;

    int                     iUpscaleRatio;
    int                     iDownscaleRatio;
    std::vector<float>		vecf_B, vecf_YL, vecf_YR, vecf_ZL, vecf_ZR;

    EInChanSel			eInChanSelection;

    CVector<_REAL>		vecrReHist;
    CVector<_REAL>		vecrImHist;
    _COMPLEX			cCurExp;
    _COMPLEX			cExpStep;
    int					iPhase;
    SpectrumAnalyser            spectrumAnalyser;

    _REAL HilbertFilt(const _REAL rRe, const _REAL rIm);

    /* OPH: counter to count symbols within a frame in order to generate */
    /* RSCI output */
    int							iFreeSymbolCounter;

    virtual void InitInternal(CParameter& Parameters);
    virtual void ProcessDataInternal(CParameter& Parameters);

    void InterpFIR_2X(const int channels, _SAMPLE* X, std::vector<float>& Z, std::vector<float>& Y, std::vector<float>& B);
    void DecimFIR_2X(const int channels, _SAMPLE* X, std::vector<float>& Z, std::vector<float>& Y, std::vector<float>& B);
    void emitRSCIData(CParameter& Parameters);
};


#endif // CRECEIVEDATA_H
