cmake_minimum_required(VERSION 3.20)
project(kestrel_aim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    message(FATAL_ERROR "MSVC required (Windows x64).")
endif()

add_executable(kestrel_aim
    src/main.cpp
    src/Config.cpp
    src/GameCapture.cpp
    src/ColorDetection.cpp
    src/AimLogic.cpp
    src/BackendLoop.cpp
)

target_include_directories(kestrel_aim PRIVATE include)

target_compile_definitions(kestrel_aim PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(kestrel_aim PRIVATE /W4 /O2 /EHsc /permissive-)

target_link_libraries(kestrel_aim PRIVATE d3d11 dxgi)
