cmake_minimum_required(VERSION 3.29)
project(QRenderer2D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        OpenGLWidgets
        Multimedia
        MultimediaWidgets
        REQUIRED)

include_directories(include)

add_library(QRenderer2D
        src/QRenderer2D.cpp
        src/SpiritBatch.cpp
)
target_include_directories(QRenderer2D INTERFACE include)
target_link_libraries(QRenderer2D
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
        Qt::OpenGLWidgets
        Qt::Multimedia
        Qt::MultimediaWidgets
)

if (CMAKE_HOST_WIN32)
    target_link_libraries(QRenderer2D
            opengl32
            glu32
    )
endif ()
