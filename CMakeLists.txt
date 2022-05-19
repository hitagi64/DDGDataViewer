cmake_minimum_required(VERSION 3.5)

project(DDGDataViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        inspector.cpp
        inspector.h
        inspector.ui
        DDG/DDGContent.h DDG/DDGContent.cpp
        DDG/DDGDat.h DDG/DDGDat.cpp
        DDG/DDGMemoryBuffer.h DDG/DDGMemoryBuffer.cpp
        DDG/DDGTxm.h DDG/DDGTxm.cpp
        DDG/DDGPdb.h DDG/DDGPdb.cpp
        ContentTreeItem.h ContentTreeItem.cpp
        ContentPreviewer.h ContentPreviewer.cpp
        Res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DDGDataViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DDGDataViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DDGDataViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DDGDataViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DDGDataViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(DDGDataViewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DDGDataViewer)
endif()
