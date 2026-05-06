#-------------------------------------------------
# Projet Intégré : Suivi de Fabrication, Bois & Arduino
#-------------------------------------------------

# Ajout de tous les modules nécessaires :
# 'network' pour l'IA, 'serialport' pour l'Arduino, et 'charts' pour les graphiques
QT += core gui sql printsupport network serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Fichiers Sources (.cpp)
SOURCES += \
    SmtpClient-for-Qt/src/emailaddress.cpp \
    SmtpClient-for-Qt/src/mimeattachment.cpp \
    SmtpClient-for-Qt/src/mimebase64encoder.cpp \
    SmtpClient-for-Qt/src/mimebase64formatter.cpp \
    SmtpClient-for-Qt/src/mimebytearrayattachment.cpp \
    SmtpClient-for-Qt/src/mimecontentencoder.cpp \
    SmtpClient-for-Qt/src/mimecontentformatter.cpp \
    SmtpClient-for-Qt/src/mimefile.cpp \
    SmtpClient-for-Qt/src/mimehtml.cpp \
    SmtpClient-for-Qt/src/mimeinlinefile.cpp \
    SmtpClient-for-Qt/src/mimemessage.cpp \
    SmtpClient-for-Qt/src/mimemultipart.cpp \
    SmtpClient-for-Qt/src/mimepart.cpp \
    SmtpClient-for-Qt/src/mimeqpencoder.cpp \
    SmtpClient-for-Qt/src/mimeqpformatter.cpp \
    SmtpClient-for-Qt/src/mimetext.cpp \
    SmtpClient-for-Qt/src/quotedprintable.cpp \
    SmtpClient-for-Qt/src/smtpclient.cpp \
    arduino.cpp \
    bois.cpp \
    connexion.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    modele.cpp \
    personnel.cpp

# Fichiers d'en-tête (.h)
HEADERS += \
    SmtpClient-for-Qt/src/emailaddress.h \
    SmtpClient-for-Qt/src/mimeattachment.h \
    SmtpClient-for-Qt/src/mimebase64encoder.h \
    SmtpClient-for-Qt/src/mimebase64formatter.h \
    SmtpClient-for-Qt/src/mimebytearrayattachment.h \
    SmtpClient-for-Qt/src/mimecontentencoder.h \
    SmtpClient-for-Qt/src/mimecontentformatter.h \
    SmtpClient-for-Qt/src/mimefile.h \
    SmtpClient-for-Qt/src/mimehtml.h \
    SmtpClient-for-Qt/src/mimeinlinefile.h \
    SmtpClient-for-Qt/src/mimemessage.h \
    SmtpClient-for-Qt/src/mimemultipart.h \
    SmtpClient-for-Qt/src/mimepart.h \
    SmtpClient-for-Qt/src/mimeqpencoder.h \
    SmtpClient-for-Qt/src/mimeqpformatter.h \
    SmtpClient-for-Qt/src/mimetext.h \
    SmtpClient-for-Qt/src/quotedprintable.h \
    SmtpClient-for-Qt/src/smtpclient.h \
    SmtpClient-for-Qt/src/smtpmime_global.h \
    arduino.h \
    bois.h \
    connexion.h \
    login.h \
    mainwindow.h \
    modele.h \
    personnel.h

# Interfaces Graphiques (.ui)
FORMS += \
    login.ui \
    mainwindow.ui

# Ressources (Images, etc.)
RESOURCES += \
    ressources.qrc

# Configuration par défaut pour le déploiement
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += $$PWD/SmtpClient-for-Qt/src
