#ifndef INPUTPSD_H
#define INPUTPSD_H
#include "util/Vector.h"
#include "matlib/Matlib.h"

class CParameter;

class SpectrumAnalyser
{
public:
    SpectrumAnalyser();
    void SetSampleRate(int sr) {
        iSampleRate = sr;
    }
    void setNegativeFrequency(bool b) { bNegativeFreq = b; }
    void setOffsetFrequency(bool b) { bOffsetFreq = b; }
    void CalculateSpectrum(const CShiftRegister<_REAL>& vecrInpData, int n);
    void CalculateLinearPSD(const CShiftRegister<_REAL>& vecrInpData,
                             int iLenPSDAvEachBlock, int iNumAvBlocksPSD,
                             int iPSDOverlap);
    void CalculateSigStrengthCorrection(CParameter &Parameters, CVector<_REAL> &vecrPSD);
    void CalculatePSDInterferenceTag(CParameter &Parameters, CVector<_REAL> &vecrPSD);
    void PSD2LogPSD(_REAL rNormData, CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);

private:
    _REAL CalcTotalPower(CVector<_REAL> &vecrData, int iStartBin, int iEndBin);
    int FreqToBin(_REAL rFreq);

    int iSampleRate;
    bool bNegativeFreq;
    bool bOffsetFreq;
    CRealVector vecrSqMagSpect;
};

#endif // INPUTPSD_H
