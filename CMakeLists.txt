cmake_minimum_required(VERSION 3.19)
project(eHaz-Game-Engine)

# ---------------------------------------
# Compiler must be set BEFORE project()
# ---------------------------------------
# Normally you set this via CLI, but if you want:
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------
# Build Types
# ---------------------------------------
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug
            -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Debug
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release
            -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Release
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Release
)

add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=RelWithDebInfo
            -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/Dist
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Dist
)

# ---------------------------------------
# Dependencies
# ---------------------------------------
add_subdirectory(dependencies/EnvHaz-Graphics/lib)

find_package(Boost REQUIRED)

file(GLOB_RECURSE EHAZVIEWER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
)

add_executable(eHazEngine ${EHAZVIEWER_SOURCES})

target_include_directories(eHazEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGui
    SYSTEM PUBLIC  # suppress warnings from boost
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(eHazEngine
    PUBLIC EnvHazGraphics
)

# ---------------------------------------
# Warning settings (your code ONLY)
# ---------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(eHazEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -fdiagnostics-color=always
    )
endif()

# ---------------------------------------
# Project defines
# ---------------------------------------
target_compile_definitions(eHazEngine PRIVATE
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

