QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    btform.cpp \
    commonpage.cpp \
    downpage.cpp \
    listitembox.cpp \
    lrcpage.cpp \
    main.cpp \
    kugoumusic.cpp \
    music.cpp \
    musiclist.cpp \
    musicslider.cpp \
    recbox.cpp \
    recboxitem.cpp \
    recentpage.cpp \
    volumetool.cpp

HEADERS += \
    btform.h \
    commonpage.h \
    downpage.h \
    kugoumusic.h \
    listitembox.h \
    lrcpage.h \
    music.h \
    musiclist.h \
    musicslider.h \
    recbox.h \
    recboxitem.h \
    recentpage.h \
    volumetool.h

FORMS += \
    btform.ui \
    commonpage.ui \
    downpage.ui \
    kugoumusic.ui \
    listitembox.ui \
    lrcpage.ui \
    musicslider.ui \
    recbox.ui \
    recboxitem.ui \
    recentpage.ui \
    volumetool.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
