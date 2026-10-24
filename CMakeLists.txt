cmake_minimum_required(VERSION 3.16)
project(caixeiro_qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(caixeiro_qt
    main_qt.cpp
    janela.cpp
    janela.hpp
    widgetrota.cpp
    widgetrota.hpp
    cenarios.cpp
    cenarios.hpp
    genetico.cpp
    genetico.hpp
    ciclo.cpp
    ciclo.hpp
)

target_link_libraries(caixeiro_qt PRIVATE Qt6::Widgets)