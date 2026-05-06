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
    arduino_notifier.cpp \
    bois.cpp \
    connexion.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    modele.cpp \
    personnel.cpp

# Fichiers d'en-tête (.h)
HEADERS += \
    arduino_notifier.h \
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
