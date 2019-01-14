cmake_minimum_required(VERSION 3.4.1)

project(iph-eco)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(WITH_UNSTRUCTURED_MESH OFF)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(VTK REQUIRED)
find_package(GeographicLib REQUIRED)
find_package(Pugixml REQUIRED)

if(MSVC)
    find_package(Qt5Sql REQUIRED)
endif()

if(WITH_UNSTRUCTURED_MESH)
	find_package(CGAL REQUIRED COMPONENTS Core)
endif()

find_library(IPH_ECO_MODEL_LIBRARY NAMES iph-eco-model PATHS ${IPH_ECO_MODEL_DIR} NO_DEFAULT_PATH)

include(${VTK_USE_FILE})

file(GLOB_RECURSE SOURCES src/main.cpp src/*.cpp include/*.h resources/resources.qrc resources/iph-eco.rc)

if(WITH_UNSTRUCTURED_MESH)
	include(${CGAL_USE_FILE})
else()
	add_definitions(-DWITHOUT_UNSTRUCTURED_MESH)
	list(REMOVE_ITEM SOURCES
		C:/dev/iph-eco/include/utility/cgal_definitions.h
		C:/dev/iph-eco/include/domain/delaunay_triangulation_face.h
		C:/dev/iph-eco/src/domain/delaunay_triangulation_face.cpp
		C:/dev/iph-eco/include/domain/unstructured_mesh.h
		C:/dev/iph-eco/src/domain/unstructured_mesh.cpp
		C:/dev/iph-eco/include/ui/unstructured_mesh_dialog.h
		C:/dev/iph-eco/src/ui/unstructured_mesh_dialog.cpp
	)
endif()

add_executable(iph-eco MACOSX_BUNDLE ${SOURCES})
target_include_directories(iph-eco PUBLIC include)

if(WITH_UNSTRUCTURED_MESH)
	target_link_libraries(iph-eco ${IPH_ECO_MODEL_LIBRARY} ${VTK_LIBRARIES} ${CGAL_LIBRARIES} ${GeographicLib_LIBRARIES} pugixml Qt5::Widgets Qt5::Sql Qt5::WebKitWidgets)
else()
	target_link_libraries(iph-eco ${IPH_ECO_MODEL_LIBRARY} ${VTK_LIBRARIES} ${GeographicLib_LIBRARIES} pugixml Qt5::Widgets Qt5::Sql Qt5::WebKitWidgets)
endif()

if(MSVC)
	set_target_properties(iph-eco PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:wmainCRTStartup")
endif()