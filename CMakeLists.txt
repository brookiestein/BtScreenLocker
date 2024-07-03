cmake_minimum_required(VERSION 3.14)

project(BtScreenLocker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets)

set(TS_FILES BtScreenLocker_es_US.ts)

add_executable(BtScreenLocker
  main.cpp
  ${TS_FILES}
)

target_link_libraries(BtScreenLocker
    Qt${QT_VERSION_MAJOR}::Bluetooth
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS BtScreenLocker
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
