cmake_minimum_required(VERSION 3.16)
project(HelloQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets UiTools)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# add_executable(Caculator
#     src/calculator/Caculator.cpp
#     src/calculator/WindowClass.h
#     src/calculator/WindowClass.cpp
#     src/calculator/CalculatorEngine.h
#     src/calculator/CalculatorEngine.cpp
# )

# qt_add_resources(Caculator "resources"
#     PREFIX "/"
#     BASE "src/calculator"
#     FILES src/calculator/Caculator.ui
# )

add_executable(opencv
    src/opencv.cpp
    src/MyTimer.h
    src/MyTimer.cpp
)

target_link_libraries(opencv PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::UiTools
    Qt6::Network
    ${OpenCV_LIBS}
)
