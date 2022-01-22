#!/bin/sh
cd ..
mkdir dream-qt4 dream-qt3 dream-qt2
cd dream-qt4
mkdir moc obj
qmake-qt4 ../dream/dream.pro
cd ../dream-qt3
mkdir moc obj
qmake-qt3 ../dream/dream.pro
cd ../dream-qt2
mkdir moc obj
cp ../dream-qt3/Makefile .
sed -i -f - Makefile <<EOT
/^CC/i QTDIR = /usr/src/qt-2.3.2
s:/usr/share/qt3:\$(QTDIR):
/^Makefile/,+3d
s/dream-qt3/dream-qt2/g
s:\.\./\.\./\.\./\.\./include/qt3:\$(QTDIR)/include:g
s:/usr/lib/libqwt.so.4:/usr/lib/qt2/libqwt.so.4:
EOT
cd ../dream
