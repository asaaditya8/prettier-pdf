cmake_minimum_required(VERSION 3.17)
project(wash)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)


if(WIN32)
    add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp mainwindow.h opencvhelper.cpp opencvhelper.h)
else()
    add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp mainwindow.h opencvhelper.cpp opencvhelper.h)
    set(OpenCV_DIR /usr/local/include/opencv4)
    include_directories("${CMAKE_CURRENT_LIST_DIR}")
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    include_directories("/usr/local/include/opencv4")
endif()

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()


find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED} ${OpenCV_LIBS})
