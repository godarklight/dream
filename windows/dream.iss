; Dream
[Setup]
AppName=Dream
AppVersion=2.1
DefaultDirName={pf}\Dream
DefaultGroupName=Dream
UninstallDisplayIcon={app}\Dream.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
                           
[Files]
Source: "Dream.exe"; DestDir: "{app}"
Source: "libfftw3-3.dll"; DestDir: "{app}"
Source: "libhamlib-2.dll"; DestDir: "{app}"
Source: "libopus-0.dll"; DestDir: "{app}"
Source: "libsndfile-1.dll"; DestDir: "{app}"
Source: "libspeexdsp.dll"; DestDir: "{app}"
Source: "libusb0.dll"; DestDir: "{app}"
Source: "Packet.dll"; DestDir: "{app}"
Source: "QtCore4.dll"; DestDir: "{app}"
Source: "QtGui4.dll"; DestDir: "{app}"
Source: "QtNetwork4.dll"; DestDir: "{app}"
Source: "QtOpenGL4.dll"; DestDir: "{app}"
Source: "QtSql4.dll"; DestDir: "{app}"
Source: "QtSvg4.dll"; DestDir: "{app}"
Source: "QtWebKit4.dll"; DestDir: "{app}"
Source: "QtXml4.dll"; DestDir: "{app}"
Source: "qwt.dll"; DestDir: "{app}"
Source: "wpcap.dll"; DestDir: "{app}"
Source: "zlib1.dll"; DestDir: "{app}"
Source: "Readme.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\Dream"; Filename: "{app}\Dream.exe"
