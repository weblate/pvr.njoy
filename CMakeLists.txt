project(pvr.njoy)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

option(PACKAGE_ZIP "Package Zip file?" OFF)

find_package(TinyXML REQUIRED)
find_package(Kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${kodiplatform_INCLUDE_DIRS}
                    ${p8-platform_INCLUDE_DIRS}
                    ${TINYXML_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR})

set(NJOY_SOURCES src/client.cpp
                 src/N7Xml.cpp)

set(DEPLIBS ${kodiplatform_LIBRARIES}
            ${p8-platform_LIBRARIES}
            ${TINYXML_LIBRARIES})

build_addon(pvr.njoy NJOY DEPLIBS)

include(CPack)
