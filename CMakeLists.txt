cmake_minimum_required(VERSION 3.16)
project(airline_management LANGUAGES CXX)

add_executable(airline_management main.cpp)
set_target_properties(airline_management PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

option(AIRLINE_BUILD_QT "Build the Qt Widgets booking interface" OFF)
if(AIRLINE_BUILD_QT)
  find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

  add_executable(airline_booking qt_booking.cpp)
  target_link_libraries(airline_booking PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
  set_target_properties(airline_booking PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
  )
endif()
