/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
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

#include "EvaluationDlg.h"
#include "DialogUtil.h"
#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif
#include <QMessageBox>
#include <QLayout>
#include <QDateTime>
#include <QFileDialog>
#include <QHideEvent>
#include <QShowEvent>

/* Implementation *************************************************************/
systemevalDlg::systemevalDlg(CRx& nrx, CSettings& Settings,
                             QWidget* parent) :
    CWindow(parent, Settings, "System Evaluation"),
    rx(nrx),
    eNewCharType(CDRMPlot::NONE_OLD)
{
    setupUi(this);

    /* Set help text for the controls */
    AddWhatsThisHelp();

    /* Init controls -------------------------------------------------------- */

    /* Init main plot */
    iPlotStyle = getSetting("plotstyle", 0, true);
    putSetting("plotstyle", iPlotStyle, true);
    MainPlot = new CDRMPlot(nullptr, plot);
    MainPlot->SetRecObj(&rx);
    MainPlot->SetPlotStyle(iPlotStyle);

    /* Init slider control */
    SliderNoOfIterations->setRange(0, 4);
    SliderNoOfIterations->setValue(rx.GetMSCMLInitNumIterations());
    TextNumOfIterations->setText(tr("MLC: Number of Iterations: ") + QString().setNum(SliderNoOfIterations->value()));

    /* Update times for colour LEDs */
    LEDFAC->SetUpdateTime(1500);
    LEDSDC->SetUpdateTime(1500);
    LEDMSC->SetUpdateTime(600);
    LEDFrameSync->SetUpdateTime(600);
    LEDTimeSync->SetUpdateTime(600);
    LEDIOInterface->SetUpdateTime(2000); /* extra long -> red light stays long */

    /* Update controls */
    UpdateControls();

    /* Set the Char Type of each selectable item */
    QTreeWidgetItemIterator it(chartSelector, QTreeWidgetItemIterator::NoChildren);
    for (; *it; it++)
    {
        if ((*it)->text(0) == tr("SNR Spectrum"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::SNR_SPECTRUM);
        if ((*it)->text(0) == tr("Audio Spectrum"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::AUDIO_SPECTRUM);
        if ((*it)->text(0) == tr("Shifted PSD"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::POWER_SPEC_DENSITY);
        if ((*it)->text(0) == tr("Waterfall Input Spectrum"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::INP_SPEC_WATERF);
        if ((*it)->text(0) == tr("Input Spectrum"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::INPUTSPECTRUM_NO_AV);
        if ((*it)->text(0) == tr("Input PSD"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::INPUT_SIG_PSD);
        if ((*it)->text(0) == tr("MSC"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::MSC_CONSTELLATION);
        if ((*it)->text(0) == tr("SDC"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::SDC_CONSTELLATION);
        if ((*it)->text(0) == tr("FAC"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::FAC_CONSTELLATION);
        if ((*it)->text(0) == tr("FAC / SDC / MSC"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::ALL_CONSTELLATION);
        if ((*it)->text(0) == tr("Frequency / Sample Rate"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::FREQ_SAM_OFFS_HIST);
        if ((*it)->text(0) == tr("Delay / Doppler"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::DOPPLER_DELAY_HIST);
        if ((*it)->text(0) == tr("SNR / Audio"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::SNR_AUDIO_HIST);
        if ((*it)->text(0) == tr("Transfer Function"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::TRANSFERFUNCTION);
        if ((*it)->text(0) == tr("Impulse Response"))
            (*it)->setData(0,  Qt::UserRole, CDRMPlot::AVERAGED_IR);
    }

    /* Expand all items */
    chartSelector->expandAll();

    /* Load saved main plot type */
    eCurCharType = PlotNameToECharType(string(getSetting("plottype", QString()).toLocal8Bit()));

    /* If RSCI in is enabled, disable some of the controls and use different
       initialization for the chart and chart selector */
    if (rx.inputIsRSCI())
    {
        SliderNoOfIterations->setEnabled(false);

        ButtonGroupChanEstFreqInt->setEnabled(false);
        ButtonGroupChanEstTimeInt->setEnabled(false);
        ButtonGroupTimeSyncTrack->setEnabled(false);
        CheckBoxFlipSpec->setEnabled(false);
        GroupBoxInterfRej->setEnabled(false);

        /* Only audio spectrum makes sence for MDI in */
        eCurCharType = CDRMPlot::AUDIO_SPECTRUM;
    }

    /* Init context menu for tree widget */
    pTreeWidgetContextMenu = new QMenu(tr("Chart Selector context menu"), this);
    pTreeWidgetContextMenu->addAction(tr("&Open in separate window"),
            this, SLOT(OnTreeWidgetContMenu(bool)));

    /* Connect controls ----------------------------------------------------- */
    connect(SliderNoOfIterations, SIGNAL(valueChanged(int)),
            this, SLOT(OnSliderIterChange(int)));

    /* Radio buttons */
    connect(RadioButtonTiLinear, SIGNAL(clicked()),
            this, SLOT(OnRadioTimeLinear()));
    connect(RadioButtonTiWiener, SIGNAL(clicked()),
            this, SLOT(OnRadioTimeWiener()));
    connect(RadioButtonFreqLinear, SIGNAL(clicked()),
            this, SLOT(OnRadioFrequencyLinear()));
    connect(RadioButtonFreqDFT, SIGNAL(clicked()),
            this, SLOT(OnRadioFrequencyDft()));
    connect(RadioButtonFreqWiener, SIGNAL(clicked()),
            this, SLOT(OnRadioFrequencyWiener()));
    connect(RadioButtonTiSyncEnergy, SIGNAL(clicked()),
            this, SLOT(OnRadioTiSyncEnergy()));
    connect(RadioButtonTiSyncFirstPeak, SIGNAL(clicked()),
            this, SLOT(OnRadioTiSyncFirstPeak()));

    /* Char selector list view */
    connect(chartSelector, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(OnListSelChanged( QTreeWidgetItem *, QTreeWidgetItem *)));
	chartSelector->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(chartSelector, SIGNAL(customContextMenuRequested ( const QPoint&  )),
		this, SLOT(OnCustomContextMenuRequested(const QPoint&)));

    /* Buttons */
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(close()));

    /* Check boxes */
    connect(CheckBoxFlipSpec, SIGNAL(clicked()),
            this, SLOT(OnCheckFlipSpectrum()));
    connect(CheckBoxMuteAudio, SIGNAL(clicked()),
            this, SLOT(OnCheckBoxMuteAudio()));
    connect(CheckBoxWriteLog, SIGNAL(stateChanged(int)),
            this, SLOT(OnCheckWriteLog(int)));
    connect(CheckBoxSaveAudioWave, SIGNAL(clicked()),
            this, SLOT(OnCheckSaveAudioWAV()));
    connect(CheckBoxRecFilter, SIGNAL(clicked()),
            this, SLOT(OnCheckRecFilter()));
    connect(CheckBoxModiMetric, SIGNAL(clicked()),
            this, SLOT(OnCheckModiMetric()));
    connect(CheckBoxReverb, SIGNAL(clicked()),
            this, SLOT(OnCheckBoxReverb()));

    /* Timer */
    connect(&Timer, SIGNAL(timeout()),
            this, SLOT(OnTimer()));

    /* Select chart type */
    chartSelector->setCurrentItem(FindItemByECharType(eCurCharType), 0);

    /* Force update */
    OnTimer();
}

systemevalDlg::~systemevalDlg()
{
    if(rx.isWriteWaveFile())
        rx.StopWriteWaveFile();
    delete MainPlot;
}

void systemevalDlg::UpdateControls()
{
    /* Slider for MLC number of iterations */
    const int iNumIt = rx.GetMSCMLInitNumIterations();
    if (SliderNoOfIterations->value() != iNumIt)
    {
        /* Update slider and label */
        SliderNoOfIterations->setValue(iNumIt);
        TextNumOfIterations->setText(tr("MLC: Number of Iterations: ") +
                                     QString().setNum(iNumIt));
    }

    /* Update for channel estimation and time sync switches */
    switch (rx.GetTimeInterpolationAlgorithm())
    {
    case TLINEAR:
        if (!RadioButtonTiLinear->isChecked())
            RadioButtonTiLinear->setChecked(true);
        break;

    case TWIENER:
        if (!RadioButtonTiWiener->isChecked())
            RadioButtonTiWiener->setChecked(true);
        break;
    }

    switch (rx.GetFrequencyInterpolationAlgorithm())
    {
    case FLINEAR:
        if (!RadioButtonFreqLinear->isChecked())
            RadioButtonFreqLinear->setChecked(true);
        break;

    case FDFTFILTER:
        if (!RadioButtonFreqDFT->isChecked())
            RadioButtonFreqDFT->setChecked(true);
        break;

    case FWIENER:
        if (!RadioButtonFreqWiener->isChecked())
            RadioButtonFreqWiener->setChecked(true);
        break;
    }

    switch (rx.GetTimeSyncTrackingType())
    {
    case TSFIRSTPEAK:
        if (!RadioButtonTiSyncFirstPeak->isChecked())
            RadioButtonTiSyncFirstPeak->setChecked(true);
        break;

    case TSENERGY:
        if (!RadioButtonTiSyncEnergy->isChecked())
            RadioButtonTiSyncEnergy->setChecked(true);
        break;
    }

    /* Update settings checkbuttons */
    CheckBoxReverb->setChecked(rx.GetReverbEffect());
    CheckBoxRecFilter->setChecked(rx.isFrequencySyncAcquisitionFilterEnabled());
    CheckBoxModiMetric->setChecked(rx.isIntefererConsiderationEnabled());
    CheckBoxMuteAudio->setChecked(rx.isAudioMuted());
    CheckBoxFlipSpec->setChecked(rx.isSpectrumFlipped());

    CheckBoxSaveAudioWave->setChecked(rx.isWriteWaveFile());
}

void systemevalDlg::eventShow(QShowEvent*)
{
    /* Restore chart windows */
    const int iNumChartWin = getSetting("numchartwin", 0);
    for (int i = 0; i < iNumChartWin; i++)
    {
        stringstream s;

        /* create the section key for this window */
        s << "Chart Window " << i;

        /* get the chart type */
        const CDRMPlot::ECharType eNewType = PlotNameToECharType(Settings.Get(s.str(), "plottype", string()));

        /* get window geometry data */
        CWinGeom c;
        Settings.Get(s.str(), c);
        const QRect WinGeom(c.iXPos, c.iYPos, c.iWSize, c.iHSize);

        /* Open the new chart window */
        CDRMPlot* pNewChartWin = OpenChartWin(eNewType);

        /* and restore its geometry */
        if (WinGeom.isValid() && !WinGeom.isEmpty() && !WinGeom.isNull())
            pNewChartWin->setGeometry(WinGeom);

        /* Add window pointer in vector (needed for closing the windows) */
        vecpDRMPlots.push_back(pNewChartWin);

		/* Show new window */
		pNewChartWin->show();
    }

    /* Update controls */
    UpdateControls();

    /* Activate real-time timer */
    Timer.start(GUI_CONTROL_UPDATE_TIME);

    /* Notify the MainPlot of showEvent */
    MainPlot->activate();
}

void systemevalDlg::eventHide(QHideEvent*)
{
    /* Notify the MainPlot of hideEvent */
    MainPlot->deactivate();

    /* Stop the real-time timer */
    Timer.stop();

    /* Store size and position of all additional chart windows */
    int iNumOpenCharts = 0;

    for (size_t i = 0; i < vecpDRMPlots.size(); i++)
    {
        /* Check, if window wasn't closed by the user */
        if (vecpDRMPlots[i]->isVisible())
        {
            stringstream s;
            CWinGeom c;
            const QRect CWGeom = vecpDRMPlots[i]->geometry();

            /* Set parameters */
            c.iXPos = CWGeom.x();
            c.iYPos = CWGeom.y();
            c.iHSize = CWGeom.height();
            c.iWSize = CWGeom.width();

            s << "Chart Window " << iNumOpenCharts;
            Settings.Put(s.str(), c);
            Settings.Put(s.str(), "plottype", ECharTypeToPlotName(vecpDRMPlots[i]->GetChartType()));

            iNumOpenCharts++;
        }
        /* Close window afterwards */
        vecpDRMPlots[i]->close();
    }
    putSetting("numchartwin", iNumOpenCharts);

    /* We do not need the pointers anymore, reset vector */
    vecpDRMPlots.clear();

    /* Store current plot type */
    putSetting("plottype", QString::fromLocal8Bit(ECharTypeToPlotName(eCurCharType).c_str()));
}

void systemevalDlg::UpdatePlotStyle(int iPlotStyle)
{
    /* Save the new style */
    putSetting("plotstyle", iPlotStyle, true);
    this->iPlotStyle = iPlotStyle;

    /* Update chart windows */
    for (size_t i = 0; i < vecpDRMPlots.size(); i++)
        vecpDRMPlots[i]->SetPlotStyle(iPlotStyle);

    /* Update main plot window */
    MainPlot->SetPlotStyle(iPlotStyle);
}

void systemevalDlg::OnTreeWidgetContMenu(bool)
{
    if (eNewCharType != CDRMPlot::NONE_OLD)
    {
        /* Open the new chart */
		CDRMPlot* pNewChartWin = OpenChartWin(eNewCharType);
        vecpDRMPlots.push_back(pNewChartWin);

		/* Show new window */
		pNewChartWin->show();

        eNewCharType = CDRMPlot::NONE_OLD;
    }
}

void systemevalDlg::OnCustomContextMenuRequested(const QPoint& p)
{
	QModelIndex index = chartSelector->indexAt(p);
    /* Make sure we have a non root item */
    if (index.parent() != QModelIndex())
    {
        /* Popup the context menu */
        eNewCharType = CDRMPlot::ECharType(index.data(Qt::UserRole).toInt());
        pTreeWidgetContextMenu->exec(QCursor::pos());
    }
}

CDRMPlot* systemevalDlg::OpenChartWin(CDRMPlot::ECharType eNewType)
{
    /* Create new chart window */
    CDRMPlot* pNewChartWin = new CDRMPlot(this, nullptr);
    pNewChartWin->setCaption(tr("Chart Window"));

    /* Set correct icon (use the same as this dialog) */
    const QIcon& icon = windowIcon();
    pNewChartWin->setIcon(icon);

    /* Set receiver object and correct chart type */
    pNewChartWin->SetRecObj(&rx);
    pNewChartWin->SetupChart(eNewType);

    /* Set plot style*/
    pNewChartWin->SetPlotStyle(iPlotStyle);

    return pNewChartWin;
}

QTreeWidgetItem* systemevalDlg::FindItemByECharType(CDRMPlot::ECharType eCharType)
{
    for (int i = 0;; i++)
    {
        QTreeWidgetItem* item = chartSelector->topLevelItem(i);
        if (item == nullptr)
            return nullptr;
        for (int j = 0; j < item->childCount(); j++)
        {
            QTreeWidgetItem* subitem = item->child(j);
            CDRMPlot::ECharType eCurCharType = CDRMPlot::ECharType(subitem->data(0, Qt::UserRole).toInt());
            if (eCurCharType == eCharType)
                return subitem;
        }
    }
}

CDRMPlot::ECharType systemevalDlg::PlotNameToECharType(const string& PlotName)
{
    QString plotName(PlotName.c_str());
    for (int i = 0;; i++)
    {
        QTreeWidgetItem* item = chartSelector->topLevelItem(i);
        if (item == nullptr)
            return CDRMPlot::AUDIO_SPECTRUM; /* safe value */
        for (int j = 0; j < item->childCount(); j++)
        {
            QTreeWidgetItem* subitem = item->child(j);
            if (plotName == subitem->text(0))
                return CDRMPlot::ECharType(subitem->data(0, Qt::UserRole).toInt());
        }
    }
}

string systemevalDlg::ECharTypeToPlotName(CDRMPlot::ECharType eCharType)
{
    QTreeWidgetItem* item = FindItemByECharType(eCharType);
    if (item != nullptr)
        return item->text(0).toStdString();
    return string();
}

void systemevalDlg::SetStatus(CMultColorLED* LED, ETypeRxStatus state)
{
    switch(state)
    {
    case NOT_PRESENT:
        LED->Reset(); /* GREY */
        break;

    case CRC_ERROR:
        LED->SetLight(CMultColorLED::RL_RED);
        break;

    case DATA_ERROR:
        LED->SetLight(CMultColorLED::RL_YELLOW);
        break;

    case RX_OK:
        LED->SetLight(CMultColorLED::RL_GREEN);
        break;
    }
}

void systemevalDlg::OnTimer()
{
    CParameter& Parameters = *(rx.GetParameters());

    Parameters.Lock();

        SetStatus(LEDFAC, Parameters.ReceiveStatus.FAC.GetStatus());
        SetStatus(LEDSDC, Parameters.ReceiveStatus.SDC.GetStatus());
		// TODO Data Broadcasts
		int iShortID = Parameters.GetCurSelAudioService();
        SetStatus(LEDMSC, Parameters.AudioComponentStatus[iShortID].GetStatus());
        SetStatus(LEDFrameSync, Parameters.ReceiveStatus.FSync.GetStatus());
        SetStatus(LEDTimeSync, Parameters.ReceiveStatus.TSync.GetStatus());
        ETypeRxStatus soundCardStatusI = Parameters.ReceiveStatus.InterfaceI.GetStatus(); /* Input */
        ETypeRxStatus soundCardStatusO = Parameters.ReceiveStatus.InterfaceO.GetStatus(); /* Output */
        SetStatus(LEDIOInterface, soundCardStatusO == NOT_PRESENT || (soundCardStatusI != NOT_PRESENT && soundCardStatusI != RX_OK) ? soundCardStatusI : soundCardStatusO);

        /* Show SNR if receiver is in tracking mode */
        if (rx.GetAcquisitionState() == AS_WITH_SIGNAL)
        {
            /* Get a consistant snapshot */

            /* We only get SNR from a local DREAM Front-End */
            _REAL rSNR = Parameters.GetSNR();
            if (rSNR >= 0.0)
            {
                /* SNR */
                ValueSNR->setText("<b>" +
                                  QString().setNum(rSNR, 'f', 1) + " dB</b>");
            }
            else
            {
                ValueSNR->setText("<b>---</b>");
            }
            /* We get MER from a local DREAM Front-End or an RSCI input but not an MDI input */
            _REAL rMER = Parameters.rMER;
            if (rMER >= 0.0 )
            {
                ValueMERWMER->setText(QString().
                                      setNum(Parameters.rWMERMSC, 'f', 1) + " dB / "
                                      + QString().setNum(rMER, 'f', 1) + " dB");
            }
            else
            {
                ValueMERWMER->setText("---");
            }

            /* Doppler estimation (assuming Gaussian doppler spectrum) */
            if (Parameters.rSigmaEstimate >= 0.0)
            {
                /* Plot delay and Doppler values */
                ValueWiener->setText(
                    QString().setNum(Parameters.rSigmaEstimate, 'f', 2) + " Hz / "
                    + QString().setNum(Parameters.rMinDelay, 'f', 2) + " ms");
            }
            else
            {
                /* Plot only delay, Doppler not available */
                ValueWiener->setText("--- / "
                                     + QString().setNum(Parameters.rMinDelay, 'f', 2) + " ms");
            }

            /* Sample frequency offset estimation */
            const _REAL rCurSamROffs = Parameters.rResampleOffset;

            /* Display value in [Hz] and [ppm] (parts per million) */
            ValueSampFreqOffset->setText(
                QString().setNum(rCurSamROffs, 'f', 2) + " Hz (" +
                QString().setNum((int) (rCurSamROffs / Parameters.GetSigSampleRate() * 1e6))
                + " ppm)");

        }
        else
        {
            ValueSNR->setText("<b>---</b>");
            ValueMERWMER->setText("---");
            ValueWiener->setText("--- / ---");
            ValueSampFreqOffset->setText("---");
        }

#ifdef _DEBUG_
        TextFreqOffset->setText("DC: " +
                                QString().setNum(rx.GetReceiveData()->
                                        ConvertFrequency(Parameters.GetDCFrequency()), 'f', 3) + " Hz ");

        /* Metric values */
        ValueFreqOffset->setText(tr("Metrics [dB]: MSC: ") +
                                 QString().setNum(
                                     rx.GetMSCMLC()->GetAccMetric(), 'f', 2) +	"\nSDC: " +
                                 QString().setNum(
                                     rx.GetSDCMLC()->GetAccMetric(), 'f', 2) +	" / FAC: " +
                                 QString().setNum(
                                     rx.GetFACMLC()->GetAccMetric(), 'f', 2));
#else
        /* DC frequency */
        ValueFreqOffset->setText(QString().setNum(rx.ConvertFrequency(Parameters.GetDCFrequency()), 'f', 2) + " Hz");
#endif

        /* If MDI in is enabled, do not show any synchronization parameter */
        if (rx.inputIsRSCI())
        {
            ValueSNR->setText("<b>---</b>");
            if (Parameters.vecrRdelThresholds.GetSize() > 0)
                ValueWiener->setText(QString().setNum(Parameters.rRdop, 'f', 2) + " Hz / "
                                     + QString().setNum(Parameters.vecrRdelIntervals[0], 'f', 2) + " ms ("
                                     + QString().setNum(Parameters.vecrRdelThresholds[0]) + "%)");
            else
                ValueWiener->setText(QString().setNum(Parameters.rRdop, 'f', 2) + " Hz / ---");

            ValueSampFreqOffset->setText("---");
            ValueFreqOffset->setText("---");
        }

        /* FAC info static ------------------------------------------------------ */
        QString strFACInfo;

        /* Robustness mode #################### */
        strFACInfo = GetRobModeStr() + " / " + GetSpecOccStr();

        //FACDRMModeBWL->setText(tr("DRM Mode / Bandwidth:")); /* Label */
        FACDRMModeBWV->setText(strFACInfo); /* Value */


        /* Interleaver Depth #################### */
        switch (Parameters.eSymbolInterlMode)
        {
        case CParameter::SI_LONG:
            strFACInfo = tr("2 s (Long Interleaving)");
            break;

        case CParameter::SI_SHORT:
            strFACInfo = tr("400 ms (Short Interleaving)");
            break;

        default:
            strFACInfo = "?";
        }

        //FACInterleaverDepthL->setText(tr("Interleaver Depth:")); /* Label */
        FACInterleaverDepthV->setText(strFACInfo); /* Value */


        /* SDC, MSC mode #################### */
        /* SDC */
        switch (Parameters.eSDCCodingScheme)
        {
        case CS_1_SM:
            strFACInfo = "4-QAM / ";
            break;

        case CS_2_SM:
            strFACInfo = "16-QAM / ";
            break;

        default:
            strFACInfo = "? / ";
        }

        /* MSC */
        switch (Parameters.eMSCCodingScheme)
        {
        case CS_2_SM:
            strFACInfo += "SM 16-QAM";
            break;

        case CS_3_SM:
            strFACInfo += "SM 64-QAM";
            break;

        case CS_3_HMSYM:
            strFACInfo += "HMsym 64-QAM";
            break;

        case CS_3_HMMIX:
            strFACInfo += "HMmix 64-QAM";
            break;

        default:
            strFACInfo += "?";
        }

        //FACSDCMSCModeL->setText(tr("SDC / MSC Mode:")); /* Label */
        FACSDCMSCModeV->setText(strFACInfo); /* Value */


        /* Code rates #################### */
        strFACInfo = QString().setNum(Parameters.MSCPrLe.iPartB);
        strFACInfo += " / ";
        strFACInfo += QString().setNum(Parameters.MSCPrLe.iPartA);

        //FACCodeRateL->setText(tr("Prot. Level (B / A):")); /* Label */
        FACCodeRateV->setText(strFACInfo); /* Value */


        /* Number of services #################### */
        strFACInfo = tr("Audio: ");
        strFACInfo += QString().setNum(Parameters.iNumAudioService);
        strFACInfo += tr(" / Data: ");
        strFACInfo += QString().setNum(Parameters.iNumDataService);

        //FACNumServicesL->setText(tr("Number of Services:")); /* Label */
        FACNumServicesV->setText(strFACInfo); /* Value */


        /* Time, date #################### */
        if ((Parameters.iUTCHour == 0) &&
                (Parameters.iUTCMin == 0) &&
                (Parameters.iDay == 0) &&
                (Parameters.iMonth == 0) &&
                (Parameters.iYear == 0))
        {
            /* No time service available */
            strFACInfo = tr("Service not available");
        }
        else
        {
#ifdef GUI_QT_DATE_TIME_TYPE
            /* QT type of displaying date and time */
            QDateTime DateTime;
            DateTime.setDate(QDate(Parameters.iYear,
                                   Parameters.iMonth,
                                   Parameters.iDay));
            DateTime.setTime(QTime(Parameters.iUTCHour,
                                   Parameters.iUTCMin));

            strFACInfo = DateTime.toString();
#else
            /* Set time and date */
            QString strMin;
            const int iMin = Parameters.iUTCMin;

            /* Add leading zero to number smaller than 10 */
            if (iMin < 10)
                strMin = "0";
            else
                strMin = "";

            strMin += QString().setNum(iMin);

            strFACInfo =
                /* Time */
                QString().setNum(Parameters.iUTCHour) + ":" +
                strMin + "  -  " +
                /* Date */
                QString().setNum(Parameters.iMonth) + "/" +
                QString().setNum(Parameters.iDay) + "/" +
                QString().setNum(Parameters.iYear);
#endif
            /* Add UTC offset if available */
            if (Parameters.bValidUTCOffsetAndSense)
                strFACInfo += QString(" %1%2%3%4")
                    .arg(tr("UTC"))
                    .arg(Parameters.iUTCSense ? "-" : "+")
                    .arg(Parameters.iUTCOff / 2, 0, 10)
                    .arg(Parameters.iUTCOff & 1 ? ".5" : "");
        }

        //FACTimeDateL->setText(tr("Received time - date:")); /* Label */
        FACTimeDateV->setText(strFACInfo); /* Value */

        UpdateGPS(Parameters);

        UpdateControls();

    Parameters.Unlock();
}

void systemevalDlg::UpdateGPS(CParameter& Parameters)
{
    gps_data_t& gps = Parameters.gps_data;

    if((gps.set&STATUS_SET)==0) {
        LEDGPS->SetLight(CMultColorLED::RL_RED);
    } else {

        if(gps.status==0)
            LEDGPS->SetLight(CMultColorLED::RL_YELLOW);
        else
            LEDGPS->SetLight(CMultColorLED::RL_GREEN);
    }

    QString qStrPosition;
    if (gps.set&LATLON_SET)
//      Wrong char on Qt 5
//      qStrPosition = QString(tr("Lat: %1\260  Long: %2\260")).arg(gps.fix.latitude, 0, 'f', 4).arg(gps.fix.longitude,0, 'f',4);
        qStrPosition = QString(trUtf8("Lat: %1°  Long: %2°")).arg(gps.fix.latitude, 0, 'f', 4).arg(gps.fix.longitude,0, 'f',4);
    else
        qStrPosition = tr("Lat: ?  Long: ?");

    QString qStrAltitude;
    if (gps.set&ALTITUDE_SET)
        qStrAltitude = QString(tr("  Alt: %1 m")).arg(gps.fix.altitude, 0, 'f', 0);
    else
        qStrAltitude = tr("  Alt: ?");
    QString qStrSpeed;
    if (gps.set&SPEED_SET)
        qStrSpeed = QString(tr("Speed: %1 m/s")).arg(gps.fix.speed, 0, 'f', 1);
    else
        qStrSpeed = tr("Speed: ?");
    QString qStrTrack;
    if (gps.set&TRACK_SET)
        qStrTrack =  QString(tr("  Track: %1\260")).arg(gps.fix.track);
    else
        qStrTrack =  tr("  Track: ?");
    QString qStrTime;
    if (gps.set&TIME_SET)
    {
        struct tm * p_ts;
        time_t tt = time_t(gps.fix.time);
        p_ts = gmtime(&tt);
        QChar fill('0');
        qStrTime = QString("UTC: %1/%2/%3 %4:%5:%6  ")
                .arg(1900 + p_ts->tm_year)
                .arg(1 + p_ts->tm_mon, 2, 10, fill)
                .arg(p_ts->tm_mday, 2, 10, fill)
                .arg(p_ts->tm_hour, 2, 10, fill)
                .arg(p_ts->tm_min, 2, 10, fill)
                .arg(p_ts->tm_sec,2, 10, fill);
    }
    else
	qStrTime = "UTC: ?";
    QString qStrSat;
    if (gps.set&SATELLITE_SET)
        qStrSat = tr("  Satellites: ") + QString().setNum(gps.satellites_used);
    else
        qStrSat = tr("  Satellites: ?");

    TextLabelGPSPosition->setText(qStrPosition+qStrAltitude);
    TextLabelGPSSpeedHeading->setText(qStrSpeed+qStrTrack);
    TextLabelGPSTime->setText(qStrTime+qStrSat);
}

void systemevalDlg::OnRadioTimeLinear()
{
    if (rx.GetTimeInterpolationAlgorithm() != TLINEAR)
        rx.SetTimeInterpolationAlgorithm(TLINEAR);
}

void systemevalDlg::OnRadioTimeWiener()
{
    if (rx.GetTimeInterpolationAlgorithm() != TWIENER)
        rx.SetTimeInterpolationAlgorithm(TWIENER);
}

void systemevalDlg::OnRadioFrequencyLinear()
{
    if (rx.GetFrequencyInterpolationAlgorithm() != FLINEAR)
        rx.SetFrequencyInterpolationAlgorithm(FLINEAR);
}

void systemevalDlg::OnRadioFrequencyDft()
{
    if (rx.GetFrequencyInterpolationAlgorithm() != FDFTFILTER)
        rx.SetFrequencyInterpolationAlgorithm(FDFTFILTER);
}

void systemevalDlg::OnRadioFrequencyWiener()
{
    if (rx.GetFrequencyInterpolationAlgorithm() != FWIENER)
        rx.SetFrequencyInterpolationAlgorithm(FWIENER);
}

void systemevalDlg::OnRadioTiSyncFirstPeak()
{
    if (rx.GetTimeSyncTrackingType() != TSFIRSTPEAK)
    {
        rx.SetTimeSyncTrackingType(TSFIRSTPEAK);
    }
}

void systemevalDlg::OnRadioTiSyncEnergy()
{
    if (rx.GetTimeSyncTrackingType() != TSENERGY)
    {
        rx.SetTimeSyncTrackingType(TSENERGY);
    }
}

void systemevalDlg::OnSliderIterChange(int value)
{
    /* Set new value in working thread module */
    rx.SetNumMSCMLCIterations(value);

    /* Show the new value in the label control */
    TextNumOfIterations->setText(tr("MLC: Number of Iterations: ") + QString().setNum(value));
}

void systemevalDlg::OnListSelChanged(QTreeWidgetItem *curr, QTreeWidgetItem *)
{
    /* Make sure we have a non root item */
    if (curr && curr->parent())
    {
        /* Get chart type from selected item */
        eCurCharType = CDRMPlot::ECharType(curr->data(0, Qt::UserRole).toInt());
        /* Setup chart */
        MainPlot->SetupChart(eCurCharType);
    }
}

void systemevalDlg::OnCheckFlipSpectrum()
{
    /* Set parameter in working thread module */
    rx.SetFlipSpectrum(CheckBoxFlipSpec->isChecked());
}

void systemevalDlg::OnCheckRecFilter()
{
    /* Set parameter in working thread module */
    rx.SetFrequencySyncAcquisitionFilter(CheckBoxRecFilter->isChecked());
}

void systemevalDlg::OnCheckModiMetric()
{
    /* Set parameter in working thread module */
    rx.SetConsiderInterferer(CheckBoxModiMetric->isChecked());
}

void systemevalDlg::OnCheckBoxMuteAudio()
{
    /* Set parameter in working thread module */
    rx.MuteAudio(CheckBoxMuteAudio->isChecked());
}

void systemevalDlg::OnCheckBoxReverb()
{
    /* Set parameter in working thread module */
    rx.SetReverberationEffect(CheckBoxReverb->isChecked());
}

void systemevalDlg::OnCheckSaveAudioWAV()
{
    /*
    	This code is copied in AnalogDemDlg.cpp. If you do changes here, you should
    	apply the changes in the other file, too
    */
    if (CheckBoxSaveAudioWave->isChecked())
    {
        /* Show "save file" dialog */
        QString strFileName =
            QFileDialog::getSaveFileName(this, "*.wav", tr("DreamOut.wav"));

        /* Check if user not hit the cancel button */
        if (!strFileName.isEmpty())
        {
			string s = strFileName.toUtf8().constData();
            rx.StartWriteWaveFile(s);
        }
        else
        {
            /* User hit the cancel button, uncheck the button */
            CheckBoxSaveAudioWave->setChecked(false);
        }
    }
    else
        rx.StopWriteWaveFile();
}


void systemevalDlg::OnCheckWriteLog(int state)
{
    if (state == Qt::Checked)
    {
		emit startLogging();
    }
    else
    {
		emit stopLogging();
    }
}

QString	systemevalDlg::GetRobModeStr()
{
    CParameter& Parameters = *rx.GetParameters();
    switch (Parameters.GetWaveMode())
    {
    case RM_ROBUSTNESS_MODE_A:
        return "A";
        break;

    case RM_ROBUSTNESS_MODE_B:
        return "B";
        break;

    case RM_ROBUSTNESS_MODE_C:
        return "C";
        break;

    case RM_ROBUSTNESS_MODE_D:
        return "D";
        break;

    default:
        return "?";
    }
}

QString	systemevalDlg::GetSpecOccStr()
{
    switch (rx.GetParameters()->GetSpectrumOccup())
    {
    case SO_0:
        return "4.5 kHz";
        break;

    case SO_1:
        return "5 kHz";
        break;

    case SO_2:
        return "9 kHz";
        break;

    case SO_3:
        return "10 kHz";
        break;

    case SO_4:
        return "18 kHz";
        break;

    case SO_5:
        return "20 kHz";
        break;

    default:
        return "?";
    }
}

void systemevalDlg::AddWhatsThisHelp()
{
    /*
    	This text was taken from the only documentation of Dream software
    */
    /* DC Frequency Offset */
    const QString strDCFreqOffs =
        tr("<b>DC Frequency Offset:</b> This is the "
           "estimation of the DC frequency offset. This offset corresponds "
           "to the resulting sound card intermedia frequency of the front-end. "
           "This frequency is not restricted to a certain value. The only "
           "restriction is that the DRM spectrum must be completely inside the "
           "bandwidth of the sound card.");

    TextFreqOffset->setWhatsThis(strDCFreqOffs);
    ValueFreqOffset->setWhatsThis(strDCFreqOffs);

    /* Sample Frequency Offset */
    const QString strFreqOffset =
        tr("<b>Sample Frequency Offset:</b> This is the "
           "estimation of the sample rate offset between the sound card sample "
           "rate of the local computer and the sample rate of the D / A (digital "
           "to analog) converter in the transmitter. Usually the sample rate "
           "offset is very constant for a given sound card. Therefore it is "
           "useful to inform the Dream software about this value at application "
           "startup to increase the acquisition speed and reliability.");

    TextSampFreqOffset->setWhatsThis(strFreqOffset);
    ValueSampFreqOffset->setWhatsThis(strFreqOffset);

    /* Doppler / Delay */
    const QString strDopplerDelay =
        tr("<b>Doppler / Delay:</b> The Doppler frequency "
           "of the channel is estimated for the Wiener filter design of channel "
           "estimation in time direction. If linear interpolation is set for "
           "channel estimation in time direction, this estimation is not updated. "
           "The Doppler frequency is an indication of how fast the channel varies "
           "with time. The higher the frequency, the faster the channel changes "
           "are.<br>The total delay of the Power Delay Spectrum "
           "(PDS) is estimated from the impulse response estimation derived from "
           "the channel estimation. This delay corresponds to the range between "
           "the two vertical dashed black lines in the Impulse Response (IR) "
           "plot.");

	TextWiener->setWhatsThis(strDopplerDelay);
    ValueWiener->setWhatsThis(strDopplerDelay);

    /* I / O Interface LED */
    const QString strLEDIOInterface =
        tr("<b>I / O Interface LED:</b> This LED shows the "
           "current status of the sound card interface. The yellow light shows "
           "that the audio output was corrected. Since the sample rate of the "
           "transmitter and local computer are different, from time to time the "
           "audio buffers will overflow or under run and a correction is "
           "necessary. When a correction occurs, a \"click\" sound can be heard. "
           "The red light shows that a buffer was lost in the sound card input "
           "stream. This can happen if a thread with a higher priority is at "
           "100% and the Dream software cannot read the provided blocks fast "
           "enough. In this case, the Dream software will instantly loose the "
           "synchronization and has to re-synchronize. Another reason for red "
           "light is that the processor is too slow for running the Dream "
           "software.");

    TextLabelLEDIOInterface->setWhatsThis(strLEDIOInterface);
    LEDIOInterface->setWhatsThis(strLEDIOInterface);

    /* Time Sync Acq LED */
    const QString strLEDTimeSyncAcq =
        tr("<b>Time Sync Acq LED:</b> This LED shows the "
           "state of the timing acquisition (search for the beginning of an OFDM "
           "symbol). If the acquisition is done, this LED will stay green.");

    TextLabelLEDTimeSyncAcq->setWhatsThis(strLEDTimeSyncAcq);
    LEDTimeSync->setWhatsThis(strLEDTimeSyncAcq);

    /* Frame Sync LED */
    const QString strLEDFrameSync =
        tr("<b>Frame Sync LED:</b> The DRM frame "
           "synchronization status is shown with this LED. This LED is also only "
           "active during acquisition state of the Dream receiver. In tracking "
           "mode, this LED is always green.");

    TextLabelLEDFrameSync->setWhatsThis(strLEDFrameSync);
    LEDFrameSync->setWhatsThis(strLEDFrameSync);

    /* FAC CRC LED */
    const QString strLEDFACCRC =
        tr("<b>FAC CRC LED:</b> This LED shows the Cyclic "
           "Redundancy Check (CRC) of the Fast Access Channel (FAC) of DRM. FAC "
           "is one of the three logical channels and is always modulated with a "
           "4-QAM. If the FAC CRC check was successful, the receiver changes to "
           "tracking mode. The FAC LED is the indication whether the receiver "
           "is synchronized to a DRM transmission or not.<br>"
           "The bandwidth of the DRM signal, the constellation scheme of MSC and "
           "SDC channels and the interleaver depth are some of the parameters "
           "which are provided by the FAC.");

    TextLabelLEDFACCRC->setWhatsThis(strLEDFACCRC);
    LEDFAC->setWhatsThis(strLEDFACCRC);

    /* SDC CRC LED */
    const QString strLEDSDCCRC =
        tr("<b>SDC CRC LED:</b> This LED shows the CRC "
           "check result of the Service Description Channel (SDC) which is one "
           "logical channel of the DRM stream. This data is transmitted in "
           "approx. 1 second intervals and contains information about station "
           "label, audio and data format, etc. The error protection is normally "
           "lower than the protection of the FAC. Therefore this LED will turn "
           "to red earlier than the FAC LED in general.<br>If the CRC check "
           "is ok but errors in the content were detected, the LED turns "
           "yellow.");

    TextLabelLEDSDCCRC->setWhatsThis(strLEDSDCCRC);
    LEDSDC->setWhatsThis(strLEDSDCCRC);

    /* MSC CRC LED */
    const QString strLEDMSCCRC =
        tr("<b>MSC CRC LED:</b> This LED shows the status "
           "of the Main Service Channel (MSC). This channel contains the actual "
           "audio and data bits. The LED shows the CRC check of the AAC core "
           "decoder. The SBR has a separate CRC, but this status is not shown "
           "with this LED. If SBR CRC is wrong but the AAC CRC is ok one can "
           "still hear something (of course, the high frequencies are not there "
           "in this case). If this LED turns red, interruptions of the audio are "
           "heard. The yellow light shows that only one 40 ms audio frame CRC "
           "was wrong. This causes usually no hearable artifacts.");

    TextLabelLEDMSCCRC->setWhatsThis(strLEDMSCCRC);
    LEDMSC->setWhatsThis(strLEDMSCCRC);

    /* MLC, Number of Iterations */
    const QString strNumOfIterations =
        tr("<b>MLC, Number of Iterations:</b> In DRM, a "
           "multilevel channel coder is used. With this code it is possible to "
           "iterate the decoding process in the decoder to improve the decoding "
           "result. The more iterations are used the better the result will be. "
           "But switching to more iterations will increase the CPU load. "
           "Simulations showed that the first iteration (number of "
           "iterations = 1) gives the most improvement (approx. 1.5 dB at a "
           "BER of 10-4 on a Gaussian channel, Mode A, 10 kHz bandwidth). The "
           "improvement of the second iteration will be as small as 0.3 dB."
           "<br>The recommended number of iterations given in the DRM "
           "standard is one iteration (number of iterations = 1).");

    TextNumOfIterations->setWhatsThis(strNumOfIterations);
    SliderNoOfIterations->setWhatsThis(strNumOfIterations);

    /* Flip Input Spectrum */
    CheckBoxFlipSpec->setWhatsThis(
                     tr("<b>Flip Input Spectrum:</b> Checking this box "
                        "will flip or invert the input spectrum. This is necessary if the "
                        "mixer in the front-end uses the lower side band."));

    /* Mute Audio */
    CheckBoxMuteAudio->setWhatsThis(
                     tr("<b>Mute Audio:</b> The audio can be muted by "
                        "checking this box. The reaction of checking or unchecking this box "
                        "is delayed by approx. 1 second due to the audio buffers."));

    /* Reverberation Effect */
    CheckBoxReverb->setWhatsThis(
                     tr("<b>Reverberation Effect:</b> If this check box is checked, a "
                        "reverberation effect is applied each time an audio drop-out occurs. "
                        "With this effect it is possible to mask short drop-outs."));

    /* Log File */
    CheckBoxWriteLog->setWhatsThis(
                     tr("<b>Log File:</b> Checking this box brings the "
                        "Dream software to write a log file about the current reception. "
                        "Every minute the average SNR, number of correct decoded FAC and "
                        "number of correct decoded MSC blocks are logged including some "
                        "additional information, e.g. the station label and bit-rate. The "
                        "log mechanism works only for audio services using AAC source coding. "
#ifdef _WIN32
                        "During the logging no Dream windows "
                        "should be moved or re-sized. This can lead to incorrect log files "
                        "(problem with QT timer implementation under Windows). This problem "
                        "does not exist in the Linux version of Dream."
#endif
                        "<br>The log file will be "
                        "written in the directory were the Dream application was started and "
                        "the name of this file is always DreamLog.txt"));

    /* Wiener */
    const QString strWienerChanEst =
        tr("<b>Channel Estimation Settings:</b> With these "
           "settings, the channel estimation method in time and frequency "
           "direction can be selected. The default values use the most powerful "
           "algorithms. For more detailed information about the estimation "
           "algorithms there are a lot of papers and books available.<br>"
           "<b>Wiener:</b> Wiener interpolation method "
           "uses estimation of the statistics of the channel to design an optimal "
           "filter for noise reduction.");

    RadioButtonFreqWiener->setWhatsThis(strWienerChanEst);
    RadioButtonTiWiener->setWhatsThis(strWienerChanEst);

    /* Linear */
    const QString strLinearChanEst =
        tr("<b>Channel Estimation Settings:</b> With these "
           "settings, the channel estimation method in time and frequency "
           "direction can be selected. The default values use the most powerful "
           "algorithms. For more detailed information about the estimation "
           "algorithms there are a lot of papers and books available.<br>"
           "<b>Linear:</b> Simple linear interpolation "
           "method to get the channel estimate. The real and imaginary parts "
           "of the estimated channel at the pilot positions are linearly "
           "interpolated. This algorithm causes the lowest CPU load but "
           "performs much worse than the Wiener interpolation at low SNRs.");

    RadioButtonFreqLinear->setWhatsThis(strLinearChanEst);
    RadioButtonTiLinear->setWhatsThis(strLinearChanEst);

    /* DFT Zero Pad */
    RadioButtonFreqDFT->setWhatsThis(
                     tr("<b>Channel Estimation Settings:</b> With these "
                        "settings, the channel estimation method in time and frequency "
                        "direction can be selected. The default values use the most powerful "
                        "algorithms. For more detailed information about the estimation "
                        "algorithms there are a lot of papers and books available.<br>"
                        "<b>DFT Zero Pad:</b> Channel estimation method "
                        "for the frequency direction using Discrete Fourier Transformation "
                        "(DFT) to transform the channel estimation at the pilot positions to "
                        "the time domain. There, a zero padding is applied to get a higher "
                        "resolution in the frequency domain -> estimates at the data cells. "
                        "This algorithm is very speed efficient but has problems at the edges "
                        "of the OFDM spectrum due to the leakage effect."));

    /* Guard Energy */
    RadioButtonTiSyncEnergy->setWhatsThis(
                     tr("<b>Guard Energy:</b> Time synchronization "
                        "tracking algorithm utilizes the estimation of the impulse response. "
                        "This method tries to maximize the energy in the guard-interval to set "
                        "the correct timing."));

    /* First Peak */
    RadioButtonTiSyncFirstPeak->setWhatsThis(
                     tr("<b>First Peak:</b> This algorithms searches for "
                        "the first peak in the estimated impulse response and moves this peak "
                        "to the beginning of the guard-interval (timing tracking algorithm)."));

    /* SNR */
    const QString strSNREst =
        tr("<b>SNR:</b> Signal to Noise Ratio (SNR) "
           "estimation based on FAC cells. Since the FAC cells are only "
           "located approximately in the region 0-5 kHz relative to the DRM DC "
           "frequency, it may happen that the SNR value is very high "
           "although the DRM spectrum on the left side of the DRM DC frequency "
           "is heavily distorted or disturbed by an interferer so that the true "
           "overall SNR is lower as indicated by the SNR value. Similarly, "
           "the SNR value might show a very low value but audio can still be "
           "decoded if only the right side of the DRM spectrum is degraded "
           "by an interferer.");

    ValueSNR->setWhatsThis(strSNREst);
    TextSNRText->setWhatsThis(strSNREst);

    /* MSC WMER / MSC MER */
    const QString strMERWMEREst =
        tr("<b>MSC WMER / MSC MER:</b> Modulation Error Ratio (MER) and "
           "weighted MER (WMER) calculated on the MSC cells is shown. The MER is "
           "calculated as follows: For each equalized MSC cell (only MSC cells, "
           "no FAC cells, no SDC cells, no pilot cells), the error vector from "
           "the nearest ideal point of the constellation diagram is measured. The "
           "squared magnitude of this error is found, and a mean of the squared "
           "errors is calculated (over one frame). The MER is the ratio in [dB] "
           "of the mean of the squared magnitudes of the ideal points of the "
           "constellation diagram to the mean squared error. This gives an "
           "estimate of the ratio of the total signal power to total noise "
           "power at the input to the equalizer for channels with flat frequency "
           "response.<br> In case of the WMER, the calculations of the means are "
           "multiplied by the squared magnitude of the estimated channel "
           "response.<br>For more information see ETSI TS 102 349.");

    ValueMERWMER->setWhatsThis(strMERWMEREst);
    TextMERWMER->setWhatsThis(strMERWMEREst);

    /* DRM Mode / Bandwidth */
    const QString strRobustnessMode =
        tr("<b>DRM Mode / Bandwidth:</b> In a DRM system, "
           "four possible robustness modes are defined to adapt the system to "
           "different channel conditions. According to the DRM standard:<ul>"
           "<li><i>Mode A:</i> Gaussian channels, with "
           "minor fading</li><li><i>Mode B:</i> Time "
           "and frequency selective channels, with longer delay spread</li>"
           "<li><i>Mode C:</i> As robustness mode B, but "
           "with higher Doppler spread</li>"
           "<li><i>Mode D:</i> As robustness mode B, but "
           "with severe delay and Doppler spread</li></ul>The "
           "bandwith is the gross bandwidth of the current DRM signal");

    FACDRMModeBWL->setWhatsThis(strRobustnessMode);
    FACDRMModeBWV->setWhatsThis(strRobustnessMode);

    /* Interleaver Depth */
    const QString strInterleaver =
        tr("<b>Interleaver Depth:</b> The symbol "
           "interleaver depth can be either short (approx. 400 ms) or long "
           "(approx. 2 s). The longer the interleaver the better the channel "
           "decoder can correct errors from slow fading signals. But the "
           "longer the interleaver length the longer the delay until (after a "
           "re-synchronization) audio can be heard.");

    FACInterleaverDepthL->setWhatsThis(strInterleaver);
    FACInterleaverDepthV->setWhatsThis(strInterleaver);

    /* SDC / MSC Mode */
    const QString strSDCMSCMode =
        tr("<b>SDC / MSC Mode:</b> Shows the modulation "
           "type of the SDC and MSC channel. For the MSC channel, some "
           "hierarchical modes are defined which can provide a very strong "
           "protected service channel.");

    FACSDCMSCModeL->setWhatsThis(strSDCMSCMode);
    FACSDCMSCModeV->setWhatsThis(strSDCMSCMode);

    /* Prot. Level (B/A) */
    const QString strProtLevel =
        tr("<b>Prot. Level (B/A):</b> The error protection "
           "level of the channel coder. For 64-QAM, there are four protection "
           "levels defined in the DRM standard. Protection level 0 has the "
           "highest protection whereas level 3 has the lowest protection. The "
           "letters A and B are the names of the higher and lower protected parts "
           "of a DRM block when Unequal Error Protection (UEP) is used. If Equal "
           "Error Protection (EEP) is used, only the protection level of part B "
           "is valid.");

    FACCodeRateL->setWhatsThis(strProtLevel);
    FACCodeRateV->setWhatsThis(strProtLevel);

    /* Number of Services */
    const QString strNumServices =
        tr("<b>Number of Services:</b> This shows the "
           "number of audio and data services transmitted in the DRM stream. "
           "The maximum number of streams is four.");

    FACNumServicesL->setWhatsThis(strNumServices);
    FACNumServicesV->setWhatsThis(strNumServices);

    /* Received time - date */
    const QString strTimeDate =
        tr("<b>Received time - date:</b> This label shows "
           "the received time and date in UTC. This information is carried in "
           "the SDC channel.");

    FACTimeDateL->setWhatsThis(strTimeDate);
    FACTimeDateV->setWhatsThis(strTimeDate);

    /* Save audio as wave */
    CheckBoxSaveAudioWave->setWhatsThis(
                     tr("<b>Save Audio as WAV:</b> Save the audio signal "
                        "as stereo, 16-bit, 48 kHz sample rate PCM wave file. Checking this "
                        "box will let the user choose a file name for the recording."));

    /* Interferer Rejection */
    const QString strInterfRej =
        tr("<b>Interferer Rejection:</b> There are two "
           "algorithms available to reject interferers:<ul>"
           "<li><b>Bandpass Filter (BP-Filter):</b>"
           " The bandpass filter is designed to have the same bandwidth as "
           "the DRM signal. If, e.g., a strong signal is close to the border "
           "of the actual DRM signal, under some conditions this signal will "
           "produce interference in the useful bandwidth of the DRM signal "
           "although it is not on the same frequency as the DRM signal. "
           "The reason for that behaviour lies in the way the OFDM "
           "demodulation is done. Since OFDM demodulation is a block-wise "
           "operation, a windowing has to be applied (which is rectangular "
           "in case of OFDM). As a result, the spectrum of a signal is "
           "convoluted with a Sinc function in the frequency domain. If a "
           "sinusoidal signal close to the border of the DRM signal is "
           "considered, its spectrum will not be a distinct peak but a "
           "shifted Sinc function. So its spectrum is broadened caused by "
           "the windowing. Thus, it will spread in the DRM spectrum and "
           "act as an in-band interferer.<br>"
           "There is a special case if the sinusoidal signal is in a "
           "distance of a multiple of the carrier spacing of the DRM signal. "
           "Since the Sinc function has zeros at certain positions it happens "
           "that in this case the zeros are exactly at the sub-carrier "
           "frequencies of the DRM signal. In this case, no interference takes "
           "place. If the sinusoidal signal is in a distance of a multiple of "
           "the carrier spacing plus half of the carrier spacing away from the "
           "DRM signal, the interference reaches its maximum.<br>"
           "As a result, if only one DRM signal is present in the 20 kHz "
           "bandwidth, bandpass filtering has no effect. Also,  if the "
           "interferer is far away from the DRM signal, filtering will not "
           "give much improvement since the squared magnitude of the spectrum "
           "of the Sinc function is approx -15 dB down at 1 1/2 carrier "
           "spacing (approx 70 Hz with DRM mode B) and goes down to approx "
           "-30 dB at 10 times the carrier spacing plus 1 / 2 of the carrier "
           "spacing (approx 525 Hz with DRM mode B). The bandpass filter must "
           "have very sharp edges otherwise the gain in performance will be "
           "very small.</li>"
           "<li><b>Modified Metrics:</b> Based on the "
           "information from the SNR versus sub-carrier estimation, the metrics "
           "for the Viterbi decoder can be modified so that sub-carriers with "
           "high noise are attenuated and do not contribute too much to the "
           "decoding result. That can improve reception under bad conditions but "
           "may worsen the reception in situations where a lot of fading happens "
           "and no interferer are present since the SNR estimation may be "
           "not correct.</li></ul>");

    GroupBoxInterfRej->setWhatsThis(strInterfRej);
    CheckBoxRecFilter->setWhatsThis(strInterfRej);
    CheckBoxModiMetric->setWhatsThis(strInterfRej);
}
