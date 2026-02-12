QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = WoodPilot_Etapes

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Etape.cpp \
    DatabaseManager.cpp

HEADERS += \
    MainWindow.h \
    Etape.h \
    DatabaseManager.h \
    StyleSheet.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
