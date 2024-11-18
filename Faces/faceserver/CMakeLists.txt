cmake_minimum_required(VERSION 3.16)

project(faceserver VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# 添加Dlib头文件目录
include_directories("D:/dlib/include")

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Sql Widgets)

# 查找OpenCV模块
set(OpenCV_DIR "D:/OpenCV/build")
find_package(OpenCV REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appfaceserver
    main.cpp
)

qt_add_qml_module(appfaceserver
    URI faceserver
    VERSION 1.0
    QML_FILES
        Main.qml
        RESOURCES resource.qrc
        SOURCES connectionpool.cpp
        SOURCES connectionpool.h
        SOURCES servercpp.cpp
        SOURCES servercpp.h
        SOURCES checkfacethread.h checkfacethread.cpp
        SOURCES clienthandler.h clienthandler.cpp
)

set_target_properties(appfaceserver PROPERTIES
    # MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appfaceserver
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appfaceserver
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Sql
    PRIVATE Qt6::Widgets
    PRIVATE ${OpenCV_LIBS}  # OpenCV库
    PRIVATE "D:/dlib/lib/libdlib.a"  # Dlib库
)

include(GNUInstallDirs)
install(TARGETS appfaceserver
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
