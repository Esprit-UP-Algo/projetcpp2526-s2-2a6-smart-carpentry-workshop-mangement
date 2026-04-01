cmake_minimum_required(VERSION 3.16)

project(FINALE VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt modules nécessaires (sans Network)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql PrintSupport)

# Liste des sources
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    connexion.cpp
    connexion.h
    personnel.cpp
    personnel.h
    bois.cpp
    bois.h
    modele.cpp
    modele.h
    login.cpp
    login.h
    login.ui
)

# Ajouter ressources.qrc s'il existe
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ressources.qrc)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ressources.qrc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FINALE
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(FINALE SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(FINALE
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Lier les bibliothèques (sans Network)
target_link_libraries(FINALE PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

# Configuration pour iOS/macOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FINALE)
endif()
set_target_properties(FINALE PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FINALE
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FINALE)
endif()
