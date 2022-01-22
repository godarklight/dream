#include "spectrumanalyser.h"
#include "creceivedata.h"
#include "Parameter.h"
#include "matlib/MatlibSigProToolbox.h"

#define RNIP_SEARCH_RANGE_NARROW 5100.0
#define RNIP_SEARCH_RANGE_WIDE 15100.0
#define RNIP_EXCLUDE_BINS 2 // either side of the peak

SpectrumAnalyser::SpectrumAnalyser():iSampleRate(48000),bNegativeFreq(false),bOffsetFreq(false),vecrSqMagSpect()
{

}

void SpectrumAnalyser::CalculateSpectrum(const CShiftRegister<_REAL>& vecrInpData, int n)
{
    /* Length of spectrum vector including Nyquist frequency */
    const int iLenSpecWithNyFreq = n / 2 + 1;

    /* Copy data from shift register to Matlib vector */
    CRealVector vecrFFTInput(n);
    for (int i = 0; i < n; i++) {
        vecrFFTInput[i] = vecrInpData[i];
    }

    /* Get squared magnitude of spectrum */
    vecrSqMagSpect.Init(iLenSpecWithNyFreq);
    CFftPlans FftPlans;
    vecrSqMagSpect = SqMag(rfft(vecrFFTInput * Hann(n), FftPlans));
}

void SpectrumAnalyser::CalculateLinearPSD(const CShiftRegister<_REAL>& vecrInpData,
                                 int iLenPSDAvEachBlock, int iNumAvBlocksPSD,
                                int iPSDOverlap)
{
    /* Length of spectrum vector including Nyquist frequency */
    const int iLenSpecWithNyFreq = iLenPSDAvEachBlock / 2 + 1;

    /* Init intermediate vectors */
    CRealVector vecrFFTInput(iLenPSDAvEachBlock);

    /* Init Hamming window */
    CRealVector vecrHammWin(Hamming(iLenPSDAvEachBlock));

    /* Init instance vector */
    vecrSqMagSpect.Init(iLenSpecWithNyFreq, 0.0);

    /* Calculate FFT of each small block and average results (estimation of PSD of input signal) */
    CFftPlans FftPlans;

    for (int i = 0; i < iNumAvBlocksPSD; i++)
    {
        /* Copy data from shift register in Matlib vector */
        for (int j = 0; j < iLenPSDAvEachBlock; j++) {
            vecrFFTInput[j] = vecrInpData[j + i * (iLenPSDAvEachBlock - iPSDOverlap)];
        }

        /* Apply Hamming window */
        vecrFFTInput *= vecrHammWin;

        /* Calculate squared magnitude of spectrum and average results */
        vecrSqMagSpect += SqMag(rfft(vecrFFTInput, FftPlans));
    }
}

void SpectrumAnalyser::PSD2LogPSD(_REAL rNormData, CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    const int iLenSpec = vecrSqMagSpect.GetSize();

    const int iOffsetScale = bNegativeFreq ? (iLenSpec / 2) : (bOffsetFreq ? (iLenSpec * int(VIRTUAL_INTERMED_FREQ) / (iSampleRate / 2)) : 0);

    const _REAL rFactorScale = _REAL(iSampleRate) / _REAL(iLenSpec) / 2000.0;

    /* Init output vectors */
    vecrData.Init(iLenSpec, 0.0);
    vecrScale.Init(iLenSpec, 0.0);

    /* Log power spectrum data */
    for (int i = 0; i <iLenSpec; i++)
    {
        const _REAL rNormSqMag = vecrSqMagSpect[i] / rNormData;

        if (rNormSqMag > 0)
            vecrData[i] = 10.0 * log10(rNormSqMag);
        else
            vecrData[i] = RET_VAL_LOG_0;

        vecrScale[i] = _REAL(i - iOffsetScale) * rFactorScale;
    }
}

/*
 * This function is called in a context where the Parameters structure is Locked.
 */
void SpectrumAnalyser::CalculateSigStrengthCorrection(CParameter &Parameters, CVector<_REAL> &vecrPSD)
{

    _REAL rCorrection = _REAL(0.0);

    /* Calculate signal power in measurement bandwidth */

    _REAL rFreqKmin, rFreqKmax;

    _REAL rIFCentreFrequency = Parameters.FrontEndParameters.rIFCentreFreq;

    if (Parameters.GetAcquiState() == AS_WITH_SIGNAL &&
            Parameters.FrontEndParameters.bAutoMeasurementBandwidth)
    {
        // Receiver is locked, so measure in the current DRM signal bandwidth Kmin to Kmax
        _REAL rDCFrequency = Parameters.GetDCFrequency();
        rFreqKmin = rDCFrequency + _REAL(Parameters.CellMappingTable.iCarrierKmin)/Parameters.CellMappingTable.iFFTSizeN * iSampleRate;
        rFreqKmax = rDCFrequency + _REAL(Parameters.CellMappingTable.iCarrierKmax)/Parameters.CellMappingTable.iFFTSizeN * iSampleRate;
    }
    else
    {
        // Receiver unlocked, or measurement is requested in fixed bandwidth
        _REAL rMeasBandwidth = Parameters.FrontEndParameters.rDefaultMeasurementBandwidth;
        rFreqKmin = rIFCentreFrequency - rMeasBandwidth/_REAL(2.0);
        rFreqKmax = rIFCentreFrequency + rMeasBandwidth/_REAL(2.0);
    }

    _REAL rSigPower = CalcTotalPower(vecrPSD, FreqToBin(rFreqKmin), FreqToBin(rFreqKmax));

    if (Parameters.FrontEndParameters.eSMeterCorrectionType == CFrontEndParameters::S_METER_CORRECTION_TYPE_AGC_ONLY)
    {
        /* Write it to the receiver params to help with calculating the signal strength */
        rCorrection += _REAL(10.0) * log10(rSigPower);
    }
    else if (Parameters.FrontEndParameters.eSMeterCorrectionType == CFrontEndParameters::S_METER_CORRECTION_TYPE_AGC_RSSI)
    {
        _REAL rSMeterBandwidth = Parameters.FrontEndParameters.rSMeterBandwidth;

        _REAL rFreqSMeterMin = _REAL(rIFCentreFrequency - rSMeterBandwidth / _REAL(2.0));
        _REAL rFreqSMeterMax = _REAL(rIFCentreFrequency + rSMeterBandwidth / _REAL(2.0));

        _REAL rPowerInSMeterBW = CalcTotalPower(vecrPSD, FreqToBin(rFreqSMeterMin), FreqToBin(rFreqSMeterMax));

        /* Write it to the receiver params to help with calculating the signal strength */

        rCorrection += _REAL(10.0) * log10(rSigPower/rPowerInSMeterBW);
    }

    /* Add on the calibration factor for the current mode */
    if (Parameters.GetReceiverMode() == RM_DRM)
        rCorrection += Parameters.FrontEndParameters.rCalFactorDRM;
    else if (Parameters.GetReceiverMode() == RM_AM)
        rCorrection += Parameters.FrontEndParameters.rCalFactorAM;

    Parameters.rSigStrengthCorrection = rCorrection;

    return;
}

/*
 * This function is called in a context where the Parameters structure is Locked.
 */
void SpectrumAnalyser::CalculatePSDInterferenceTag(CParameter &Parameters, CVector<_REAL> &vecrPSD)
{

    /* Interference tag (rnip) */

    // Calculate search range: defined as +/-5.1kHz except if locked and in 20k
    _REAL rIFCentreFrequency = Parameters.FrontEndParameters.rIFCentreFreq;

    _REAL rFreqSearchMin = rIFCentreFrequency - _REAL(RNIP_SEARCH_RANGE_NARROW);
    _REAL rFreqSearchMax = rIFCentreFrequency + _REAL(RNIP_SEARCH_RANGE_NARROW);

    ESpecOcc eSpecOcc = Parameters.GetSpectrumOccup();

    if (Parameters.GetAcquiState() == AS_WITH_SIGNAL &&
            (eSpecOcc == SO_4 || eSpecOcc == SO_5) )
    {
        rFreqSearchMax = rIFCentreFrequency + _REAL(RNIP_SEARCH_RANGE_WIDE);
    }
    int iSearchStartBin = FreqToBin(rFreqSearchMin);
    int iSearchEndBin = FreqToBin(rFreqSearchMax);

    if (iSearchStartBin < 0) iSearchStartBin = 0;
    if (iSearchEndBin > LEN_PSD_AV_EACH_BLOCK_RSI/2)
        iSearchEndBin = LEN_PSD_AV_EACH_BLOCK_RSI/2;

    _REAL rMaxPSD = _REAL(-1000.0);
    int iMaxPSDBin = 0;

    for (int i=iSearchStartBin; i<=iSearchEndBin; i++)
    {
        _REAL rPSD = _REAL(2.0) * pow(_REAL(10.0), vecrPSD[i]/_REAL(10.0));
        if (rPSD > rMaxPSD)
        {
            rMaxPSD = rPSD;
            iMaxPSDBin = i;
        }
    }

    // For total signal power, exclude the biggest one and e.g. 2 either side
    int iExcludeStartBin = iMaxPSDBin - RNIP_EXCLUDE_BINS;
    int iExcludeEndBin = iMaxPSDBin + RNIP_EXCLUDE_BINS;

    // Calculate power. TotalPower() function will deal with start>end correctly
    _REAL rSigPowerExcludingInterferer = CalcTotalPower(vecrPSD, iSearchStartBin, iExcludeStartBin-1) +
                                         CalcTotalPower(vecrPSD, iExcludeEndBin+1, iSearchEndBin);

    /* interferer level wrt signal power */
    Parameters.rMaxPSDwrtSig = _REAL(10.0) * log10(rMaxPSD / rSigPowerExcludingInterferer);

    /* interferer frequency */
    Parameters.rMaxPSDFreq = _REAL(iMaxPSDBin) * _REAL(iSampleRate) / _REAL(LEN_PSD_AV_EACH_BLOCK_RSI) - rIFCentreFrequency;

}

int SpectrumAnalyser::FreqToBin(_REAL rFreq)
{
    return int(rFreq/iSampleRate * LEN_PSD_AV_EACH_BLOCK_RSI);
}

_REAL SpectrumAnalyser::CalcTotalPower(CVector<_REAL> &vecrData, int iStartBin, int iEndBin)
{
    if (iStartBin < 0) iStartBin = 0;
    if (iEndBin > LEN_PSD_AV_EACH_BLOCK_RSI/2)
        iEndBin = LEN_PSD_AV_EACH_BLOCK_RSI/2;

    _REAL rSigPower = _REAL(0.0);
    for (int i=iStartBin; i<=iEndBin; i++)
    {
        _REAL rPSD = pow(_REAL(10.0), vecrData[i]/_REAL(10.0));
        // The factor of 2 below is needed because half of the power is in the negative frequencies
        rSigPower += rPSD * _REAL(2.0);
    }

    return rSigPower;
}
