project(video_annotator)
cmake_minimum_required(VERSION 2.8)
# Debug, Release, RelWithDebInfo and MinSizeRe
set(CMAKE_BUILD_TYPE RelWithDebInfo)
SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # add extra warnings

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/video_annotator_path.h.in"
               "${PROJECT_BINARY_DIR}/video_annotator_path.h")


ADD_LIBRARY(tinyxml
            tinyxml/tinystr.cpp  tinyxml/tinyxml.cpp  tinyxml/tinyxmlerror.cpp  tinyxml/tinyxmlparser.cpp  #tinyxml/xmltest.cpp
            )
FIND_PACKAGE(OpenCV)
ADD_EXECUTABLE(test.exe
               test.cpp GUI.cpp Interpolator.cpp XmlFile.cpp)
TARGET_LINK_LIBRARIES(test.exe ${OpenCV_LIBS} tinyxml)
