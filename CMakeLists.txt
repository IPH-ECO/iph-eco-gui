cmake_minimum_required(VERSION 2.8.8)

project(iph-eco)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.4.0/lib/cmake/Qt5Widgets)

find_package(Qt5Widgets REQUIRED)
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

file(GLOB_RECURSE SOURCES src/main.cpp src/*.cpp include/*.h icons.qrc)

add_executable(iph-eco ${SOURCES})
target_link_libraries(iph-eco ${VTK_LIBRARIES} Qt5::Widgets Qt5::Sql CGAL Geographic gmp mpfr boost_system boost_thread-mt)
