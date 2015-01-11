cmake_minimum_required(VERSION 2.8)
project(video_annotator)
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
