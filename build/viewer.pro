win32 { TEMPLATE = vcapp }
else  { TEMPLATE = app }

unix:!macx:!ios:!android:TARGET = inscoreviewer
else { TARGET = INScoreViewer }
OBJECTS_DIR = tmp
MOC_DIR		= tmp
RCC_DIR		= tmp

VERSION = 1.17

QT += core gui widgets svg printsupport multimedia multimediawidgets qml websockets

############################## 
# locations
############################## 
ROOT = $$PWD/..
SRC  = $$ROOT/src
APPL = $$SRC/inscoreviewer
LIB  = $$SRC/inscore

CONFIG += c++11

############################## 
# source and headers
############################## 
NOVIEW { SOURCES += $$APPL/INScoreNoView.cpp } 
else   { SOURCES += $$APPL/INScoreAppl.cpp }
HEADERS 	+= $$APPL/INScoreAppl.h
INCLUDEPATH += $$APPL $$LIB/interface

############### Resources
RESOURCES += $$ROOT/rsrc/inscore.qrc
ICON = $$ROOT/rsrc/INScoreViewer.icns

############################## 
# macos x support
############################## 
macx {
	QMAKE_LFLAGS += -F$$PWD/macos/Release
	LIBS += -framework INScore
	QMAKE_INFO_PLIST = $$PWD/Info.plist
	RSRC.files 	= $$ROOT/rsrc/INScoreViewer.icns
	RSRC.path 	= Contents/Resources
	QMAKE_BUNDLE_DATA += RSRC
	ICON 		= $$ROOT/rsrc/INScoreViewer.icns
}

##############################
# ios support
##############################
ios {
        LIBS += -L. -lINScore $$ROOT/lib/GuidoEngine/ios/libGUIDOEngine.a
        QMAKE_INFO_PLIST = $$PWD/Info.plist
        ios_icon.files = $$files($$ROOT/rsrc/ios/*.png)
        QMAKE_BUNDLE_DATA += ios_icon
        ICON 		= $$ROOT/rsrc/INScoreViewer.icns
        RESOURCES += $$ROOT/rsrc/inscoremobile.qrc
        OBJECTIVE_SOURCES += $$SRC/mobile/ios/QtAppDelegate.mm
        INCLUDEPATH += $$SRC/mobile/ios
        HEADERS += $$files($$SRC/mobile/ios/*.h)
}

############################## 
# windows support
# assumes environment is MSVC
############################## 
win32 {
	VERSION = ""
	CONFIG(debug,debug|release) { LIBS   += debug/INScore.lib }
	else { LIBS   += release/INScore.lib }
	RC_FILE = $$ROOT/win32/Viewer/INScoreViewer.rc
}

############################## 
# linux support
############################## 
unix:!android:!macx:!ios:LIBS += -L. -lINScore -lGUIDOEngine -lguidoar -lmicrohttpd

############################## 
# android support
############################## 
android {
        LIBS += -L. -lINScore
        ANDROID_EXTRA_LIBS = $$ROOT/lib/GuidoEngine/android/libGUIDOEngine.so $$ROOT/lib/GuidoAR/android/libguidoar.so libINScore.so
        DISTFILES +=  $$ROOT/rsrc/android/AndroidManifest.xml
        ANDROID_PACKAGE_SOURCE_DIR = $$ROOT/rsrc/android
        QT += androidextras
        RESOURCES += $$ROOT/rsrc/inscoremobile.qrc
}

