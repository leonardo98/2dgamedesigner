cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME 2DGameDesigner)

set(Qt5Gui_DIR "~/Qt/5.13.0/gcc_64/lib/cmake/Qt5Gui")
set(Qt5Core_DIR "~/Qt/5.13.0/gcc_64/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "~/Qt/5.13.0/gcc_64/lib/cmake/Qt5Widgets")
set(Qt5OpenGL_DIR "~/Qt/5.13.0/gcc_64/lib/cmake/Qt5OpenGL")

project(${PROJECT_NAME})

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

#OpenGL
find_package(OpenGL REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h src/*.c)


file(COPY gfx DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#set(RESOURCE ${CMAKE_SOURCE_DIR}/resources.qrc)
set(RESOURCE ${CMAKE_CURRENT_BINARY_DIR}/gfx/resources.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCE_ADDED})

include_directories("src/qtpropertybrowser/src" "src/Core")

qt5_use_modules(${PROJECT_NAME}
    Core
    Gui
    Widgets
    OpenGL)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Gui Qt5::Core Qt5::OpenGL ${OPENGL_LIBRARIES})
