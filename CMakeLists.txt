cmake_minimum_required(VERSION 2.8.8)

project(iph-eco)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(UNIX)
	set(CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.4.0/lib/cmake/Qt5Widgets)
endif()

find_package(Qt5Widgets REQUIRED)
if(MSVC)
    find_package(Qt5Sql REQUIRED)
endif()
find_package(VTK REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(GeographicLib REQUIRED)

if(NOT MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include(${VTK_USE_FILE})
include(${CGAL_USE_FILE})

file(GLOB_RECURSE SOURCES src/main.cpp src/*.cpp include/*.h icons.qrc)

add_executable(iph-eco MACOSX_BUNDLE ${SOURCES})

if(MSVC)
	target_link_libraries(iph-eco ${VTK_LIBRARIES} ${CGAL_LIBRARIES} ${GeographicLib_LIBRARIES} Qt5::Widgets Qt5::Sql)
else()
	target_link_libraries(iph-eco ${VTK_LIBRARIES} ${CGAL_LIBRARIES} ${GeographicLib_LIBRARIES} Qt5::Widgets Qt5::Sql gmp mpfr boost_system boost_thread-mt)
endif()