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


function(add_gl_deps TARGET)
    target_include_directories(${TARGET} PRIVATE ${GLAD_DIR}/include)
    target_sources(${TARGET} PRIVATE ${GLAD_DIR}/src/glad.c)
    target_link_libraries(${TARGET} glfw)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glm/include/glm)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/oglwrap/include)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/stb)
endfunction(add_gl_deps)

function(make_windows_compiler_happy TARGET)
    if(WIN32)
        if(MSVC)
            target_link_options(${TARGET} PRIVATE "/subsystem:Windows /entry:mainCRTStartup /utf-8")
        elseif(MINGW)
            target_link_options(${TARGET} PRIVATE "-mwindows")
            target_link_libraries(${TARGET} -static gcc stdc++ winpthread -dynamic)
        endif()
    endif()
endfunction(make_windows_compiler_happy)


file(GLOB PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(SillyGame ${PROJECT_SOURCES})
make_windows_compiler_happy(SillyGame)
add_gl_deps(SillyGame)
