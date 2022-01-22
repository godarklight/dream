QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase
INCLUDEPATH += /usr/local/include
TEMPLATE = app
LIBS += -lfftw3 -lz -lspeexdsp
LIBS += -L/usr/local/lib
unix:SOURCES += ../src/linux/Pacer.cpp
win32:SOURCES += ../src/windows/Pacer.cpp
SOURCES +=  tst_receiverinputchangetest.cpp
SOURCES += \
    ../src/AMDemodulation.cpp \
    ../src/AMSSDemodulation.cpp \
    ../src/chanest/ChanEstTime.cpp \
    ../src/chanest/ChannelEstimation.cpp \
    ../src/chanest/IdealChannelEstimation.cpp \
    ../src/chanest/TimeLinear.cpp \
    ../src/chanest/TimeWiener.cpp \
    ../src/datadecoding/DABMOT.cpp \
    ../src/datadecoding/DataDecoder.cpp \
    ../src/datadecoding/DataEncoder.cpp \
    ../src/datadecoding/epgutil.cpp \
    ../src/datadecoding/Experiment.cpp \
    ../src/datadecoding/Journaline.cpp \
    ../src/datadecoding/journaline/crc_8_16.c \
    ../src/datadecoding/journaline/dabdgdec_impl.c \
    ../src/datadecoding/journaline/log.c \
    ../src/datadecoding/journaline/newsobject.cpp \
    ../src/datadecoding/journaline/newssvcdec_impl.cpp \
    ../src/datadecoding/journaline/NML.cpp \
    ../src/datadecoding/journaline/Splitter.cpp \
    ../src/datadecoding/MOTSlideShow.cpp \
    ../src/DataIO.cpp \
    ../src/drmchannel/ChannelSimulation.cpp \
    ../src/DrmReceiver.cpp \
    ../src/DRMSignalIO.cpp \
    ../src/DrmSimulation.cpp \
    ../src/DrmTransmitter.cpp \
    ../src/FAC/FAC.cpp \
    ../src/InputResample.cpp \
    ../src/interleaver/BlockInterleaver.cpp \
    ../src/interleaver/SymbolInterleaver.cpp \
    ../src/IQInputFilter.cpp \
    ../src/matlib/MatlibSigProToolbox.cpp \
    ../src/matlib/MatlibStdToolbox.cpp \
    ../src/MDI/AFPacketGenerator.cpp \
    ../src/MDI/MDIDecode.cpp \
    ../src/MDI/MDIInBuffer.cpp \
    ../src/MDI/MDIRSCI.cpp \
    ../src/MDI/MDITagItemDecoders.cpp \
    ../src/MDI/MDITagItems.cpp \
    ../src/MDI/PacketSinkFile.cpp \
    ../src/MDI/PacketSocket.cpp \
    ../src/MDI/PacketSourceFile.cpp \
    ../src/MDI/Pft.cpp \
    ../src/MDI/RCITagItems.cpp \
    ../src/MDI/RSCITagItemDecoders.cpp \
    ../src/MDI/RSISubscriber.cpp \
    ../src/MDI/TagPacketDecoder.cpp \
    ../src/MDI/TagPacketDecoderMDI.cpp \
    ../src/MDI/TagPacketDecoderRSCIControl.cpp \
    ../src/MDI/TagPacketGenerator.cpp \
    ../src/mlc/BitInterleaver.cpp \
    ../src/mlc/ChannelCode.cpp \
    ../src/mlc/ConvEncoder.cpp \
    ../src/mlc/EnergyDispersal.cpp \
    ../src/mlc/Metric.cpp \
    ../src/mlc/MLC.cpp \
    ../src/mlc/QAMMapping.cpp \
    ../src/mlc/TrellisUpdateMMX.cpp \
    ../src/mlc/TrellisUpdateSSE2.cpp \
    ../src/mlc/ViterbiDecoder.cpp \
    ../src/MSCMultiplexer.cpp \
    ../src/ofdmcellmapping/CellMappingTable.cpp \
    ../src/ofdmcellmapping/OFDMCellMapping.cpp \
    ../src/OFDM.cpp \
    ../src/Parameter.cpp \
    ../src/PlotManager.cpp \
    ../src/ReceptLog.cpp \
    ../src/resample/Resample.cpp \
    ../src/resample/ResampleFilter.cpp \
    ../src/Scheduler.cpp \
    ../src/SDC/SDCReceive.cpp \
    ../src/SDC/SDCTransmit.cpp \
    ../src/SDC/audioparam.cpp \
    ../src/ServiceInformation.cpp \
    ../src/SimulationParameters.cpp \
    ../src/sound/audiofilein.cpp \
    ../src/sourcedecoders/aac_codec.cpp \
    ../src/sourcedecoders/AudioCodec.cpp \
    ../src/sourcedecoders/AudioSourceDecoder.cpp \
    ../src/sourcedecoders/AudioSourceEncoder.cpp \
    ../src/sourcedecoders/null_codec.cpp \
    ../src/sourcedecoders/opus_codec.cpp \
    ../src/sync/FreqSyncAcq.cpp \
    ../src/sync/SyncUsingPil.cpp \
    ../src/sync/TimeSync.cpp \
    ../src/sync/TimeSyncFilter.cpp \
    ../src/sync/TimeSyncTrack.cpp \
    ../src/tables/TableCarMap.cpp \
    ../src/tables/TableFAC.cpp \
    ../src/tables/TableStations.cpp \
    ../src/TextMessage.cpp \
    ../src/util/CRC.cpp \
    ../src/util/FileTyper.cpp \
    ../src/util/LogPrint.cpp \
    ../src/util/Reassemble.cpp \
    ../src/util/Settings.cpp \
    ../src/util/Utilities.cpp \
    ../src/Version.cpp \
    ../src/sound/soundnull.cpp \
    ../src/DrmTransceiver.cpp \
    ../src/sound/soundinterface.cpp \
    ../src/sound/selectioninterface.cpp \
    ../src/MSC/logicalframe.cpp \
    ../src/MSC/audiosuperframe.cpp \
    ../src/MSC/aacsuperframe.cpp \
    ../src/MSC/xheaacsuperframe.cpp \
    ../src/MSC/frameborderdescription.cpp \
    ../src/resample/speexresampler.cpp \
    ../src/resample/cspectrumresample.cpp \
    ../src/resample/caudioresample.cpp \
    ../src/sourcedecoders/reverb.cpp \
    ../src/sourcedecoders/caudioreverb.cpp
