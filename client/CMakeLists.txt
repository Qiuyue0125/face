cmake_minimum_required(VERSION 3.16)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

# 添加Dlib头文件目录
include_directories("D:/dlib/include")

# 设置OpenCV
set(OpenCV_DIR "D:/OpenCV/build")

find_package(OpenCV REQUIRED)

qt_add_executable(appclient
    main.cpp
)

qt_add_qml_module(appclient
    URI client
    VERSION 1.0
    QML_FILES
    Main.qml
    SOURCES facedetector.h
    SOURCES facedetector.cpp
    RESOURCES resouces.qrc
    SOURCES imageprovider.h imageprovider.cpp
    SOURCES threads.h threads.cpp
)

set_target_properties(appclient PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appclient
    PRIVATE Qt6::Quick
    PRIVATE ${OpenCV_LIBS}  # 添加 OpenCV 库
    PRIVATE "D:/dlib/lib/libdlib.a"  # Dlib库
)

target_include_directories(appclient
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}  # 包含当前源文件目录
)

include(GNUInstallDirs)
install(TARGETS appclient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

