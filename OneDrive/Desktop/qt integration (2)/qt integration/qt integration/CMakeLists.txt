cmake_minimum_required(VERSION 3.16)

project(FINALE VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Détecte Qt6 ou Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Core Gui)

# Fichiers sources communs
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    login.cpp
    login.h
    login.ui
    ajout_fab.cpp
    ajout_fab.h
    ajout_fab.ui
    modifier_fab.cpp
    modifier_fab.h
    modifier_fab.ui
    supprimer_fab.cpp
    supprimer_fab.h
    supprimer_fab.ui
    connexion.cpp
    connexion.h
    fab_controller.cpp
    fab_controller.h
    ressources.qrc
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(FINALE
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(FINALE
        ${PROJECT_SOURCES}
    )
endif()

# Lien avec Qt
target_link_libraries(FINALE PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
)

# Propriétés macOS / Windows
if(QT_VERSION VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FINALE)
endif()

set_target_properties(FINALE PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation
include(GNUInstallDirs)
install(TARGETS FINALE
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalisation pour Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FINALE)
endif()
