cmake_minimum_required(VERSION 3.28)

project(BtScreenLocker VERSION 1.3.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(SERVICE_NAME="com.github.brookiestein.BluetoothListener")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)

set(TS_FILES BtScreenLocker_es_US.ts)
list(TRANSFORM TS_FILES PREPEND "translations/")
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

set(PROJECT_SOURCES
    bluetoothlistener.hpp
    bluetoothlistener.cpp
    devicechooser.hpp
    devicechooser.cpp
    devicechooser.ui
    logger.hpp
    logger.cpp
    main.cpp
    screenlocker.hpp
    screenlocker.cpp
    # Because we prepend src/ to this CMake variable, but they're in the project root directory.
    ../resources.qrc
)

list(TRANSFORM PROJECT_SOURCES PREPEND "src/")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BtScreenLocker
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BtScreenLocker APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BtScreenLocker SHARED
            ${PROJECT_SOURCES}
            ${QM_FILES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BtScreenLocker
            ${PROJECT_SOURCES}
            ${QM_FILES}
        )
    endif()
endif()

target_link_libraries(BtScreenLocker PRIVATE
    Qt${QT_VERSION_MAJOR}::Bluetooth
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(BtScreenLocker PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BtScreenLocker
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES resources/BtScreenLocker.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/resources/translations)
install(DIRECTORY resources/assets DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/resources)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(BtScreenLocker)
endif()
