cmake_minimum_required(VERSION 3.10)
project(ICP_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

add_executable(PetriNetApp src/main.cpp src/tools.cpp src/labels.cpp src/globals.cpp src/help.cpp 
src/object_list.cpp src/serializer.cpp src/edit_config.cpp src/parser.cpp src/engine.cpp src/generator.cpp src/loader.cpp)

add_custom_target(todos
    COMMAND grep -rn "TODO:" ${CMAKE_SOURCE_DIR}/src/ > ${CMAKE_SOURCE_DIR}/src/TODO
    COMMAND grep -rn "FIXME:" ${CMAKE_SOURCE_DIR}/src/ > ${CMAKE_SOURCE_DIR}/src/TODO
    COMMENT "Collecting TODOs and FIXMEs..."
)

target_link_libraries(PetriNetApp Qt5::Widgets Qt5::Network)