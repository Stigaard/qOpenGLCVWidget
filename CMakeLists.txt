cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtGui QtCore QtOpenGL)
find_package(OpenGL REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(qOpenGLCVWidget_SRCS qOpenGLCVWidget.cc)

qt4_automoc(${qOpenGLCVWidget_SRCS})

ADD_LIBRARY(qOpenGLCVWidget STATIC ${qOpenGLCVWidget_SRCS})
	   
TARGET_LINK_LIBRARIES(qOpenGLCVWidget ${QT_QTOPENGL_LIBRARY} ${OPENGL_glu_LIBRARY} GL)