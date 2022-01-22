#!/bin/sh
mkdir -p dream
rm -rf dream/dream.app
cp -r dream.app dream
cd dream
mkdir dream.app/Contents/Frameworks
cp /usr/local/lib/libhamlib.2.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libportaudio.2.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libsndfile.1.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libFLAC.8.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libvorbis.0.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libvorbisenc.2.dylib dream.app/Contents/Frameworks/
cp /opt/local/lib/libogg.0.dylib dream.app/Contents/Frameworks/

macdeployqt dream.app

install_name_tool -change /usr/local/lib/libhamlib.2.dylib @executable_path/../Frameworks/libhamlib.2.dylib dream.app/Contents/MacOS/dream 
install_name_tool -change /opt/local/lib/libportaudio.2.dylib @executable_path/../Frameworks/libportaudio.2.dylib dream.app/Contents/MacOS/dream 
install_name_tool -change /opt/local/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib dream.app/Contents/MacOS/dream 

install_name_tool -change /opt/local/lib/libogg.0.dylib @executable_path/../Frameworks/libogg.0.dylib dream.app/Contents/Frameworks/libsndfile.1.dylib 
install_name_tool -change /opt/local/lib/libvorbis.0.dylib @executable_path/../Frameworks/libvorbis.0.dylib dream.app/Contents/Frameworks/libsndfile.1.dylib 
install_name_tool -change /opt/local/lib/libvorbisenc.2.dylib @executable_path/../Frameworks/libvorbisenc.2.dylib dream.app/Contents/Frameworks/libsndfile.1.dylib 
install_name_tool -change /opt/local/lib/libFLAC.8.dylib @executable_path/../Frameworks/libFLAC.8.dylib dream.app/Contents/Frameworks/libsndfile.1.dylib 
install_name_tool -change /opt/local/lib/libogg.0.dylib @executable_path/../Frameworks/libogg.0.dylib dream.app/Contents/Frameworks/libvorbisenc.2.dylib 
install_name_tool -change /opt/local/lib/libvorbis.0.dylib @executable_path/../Frameworks/libvorbis.0.dylib dream.app/Contents/Frameworks/libvorbisenc.2.dylib 

cd ..
hdiutil create dream.dmg -srcfolder dream/ -ov
