cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtGui QtCore QtOpenGL)
find_package(OpenGL REQUIRED)
find_package(X11)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${X11_INCLUDE_DIRS})
add_definitions(${QT_DEFINITIONS})

set(qOpenGLCVWidget_SRCS qOpenGLCVWidget.cc)

qt4_automoc(${qOpenGLCVWidget_SRCS})

ADD_LIBRARY(qOpenGLCVWidget STATIC ${qOpenGLCVWidget_SRCS})
	   
TARGET_LINK_LIBRARIES(qOpenGLCVWidget ${QT_QTOPENGL_LIBRARY} ${OPENGL_glu_LIBRARY} GL ${X11_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})