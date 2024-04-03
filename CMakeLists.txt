cmake_minimum_required(VERSION 3.15)

project(SillyGame)

# 舒服一点。
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLAD_VERSION 4.1-compat)
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glad-${GLAD_VERSION})
set(GLFW_VERSION 3.4)
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glfw-${GLFW_VERSION})

add_subdirectory(${GLFW_DIR})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/libuv)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/uv-cpp)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/spdlog)


function(make_windows_compiler_happy TARGET)
    if(WIN32)
        if(MSVC)
            target_link_options(${TARGET} PRIVATE "/subsystem:Windows /entry:mainCRTStartup /utf-8")
        elseif(MINGW)
            target_link_options(${TARGET} PRIVATE "-mwindows")
            target_link_libraries(${TARGET} PRIVATE -static gcc stdc++ winpthread -dynamic)
        endif()
    endif()
endfunction(make_windows_compiler_happy)


set(PROJECT_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_ROOT}/*.cpp)
add_executable(SillyGame ${PROJECT_SOURCES})
target_include_directories(SillyGame PRIVATE ${PROJECT_SOURCE_ROOT})

target_include_directories(SillyGame PRIVATE ${GLAD_DIR}/include)
target_sources(SillyGame PRIVATE ${GLAD_DIR}/src/glad.c)
target_link_libraries(SillyGame PRIVATE glfw)
target_include_directories(SillyGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glm/include/glm)
target_include_directories(SillyGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/oglwrap/include)
target_include_directories(SillyGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/stb)
target_link_libraries(SillyGame PRIVATE uv_a)
target_link_libraries(SillyGame PRIVATE uv-cpp)
target_link_libraries(SillyGame PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
if(WIN32)
    target_link_libraries(SillyGame PRIVATE imm32)
endif()

make_windows_compiler_happy(SillyGame)
