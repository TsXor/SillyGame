cmake_minimum_required(VERSION 3.15)

project(SillyGame)

# 舒服一点。
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/libuv)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/uv-cpp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/coutils)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/sf_boost_deps)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/spdlog)


function(compiler_static_lib TARGET)
    if(CMAKE_COMPILER_IS_GNUCC)
        target_link_libraries(${TARGET} PRIVATE -static-libgcc)
        if(CMAKE_COMPILER_IS_GNUCXX)
            target_link_libraries(${TARGET} PRIVATE -static-libstdc++)
            if (MINGW)
                target_link_libraries(${TARGET} PRIVATE -static winpthread -dynamic)
            endif()
        endif()
    endif()
endfunction(compiler_static_lib)

function(make_windows_compiler_happy TARGET)
    if(WIN32)
        if(MSVC)
            target_link_options(${TARGET} PRIVATE "/subsystem:Windows /entry:mainCRTStartup /utf-8")
        elseif(MINGW)
            target_link_options(${TARGET} PRIVATE "-mwindows")
        endif()
    endif()
endfunction(make_windows_compiler_happy)


set(SILLY_FRAMEWORK_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/silly_framework)
set(SILLY_FRAMEWORK_HEADER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/silly_framework)
file(GLOB_RECURSE SILLY_FRAMEWORK_SOURCES ${SILLY_FRAMEWORK_SOURCE_ROOT}/*.cpp ${SILLY_FRAMEWORK_SOURCE_ROOT}/*.c)
add_library(SillyFramework SHARED ${SILLY_FRAMEWORK_SOURCES})
compiler_static_lib(SillyFramework)
target_compile_definitions(SillyFramework PRIVATE SF_BUILD)
target_include_directories(SillyFramework PRIVATE ${SILLY_FRAMEWORK_SOURCE_ROOT}/..)
target_include_directories(SillyFramework PUBLIC  ${SILLY_FRAMEWORK_HEADER_ROOT}/..)

target_link_libraries(SillyFramework PUBLIC glfw)
target_include_directories(SillyFramework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glm/include/glm)
target_include_directories(SillyFramework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/oglwrap/include)
target_link_libraries(SillyFramework PUBLIC coutils)
target_link_libraries(SillyFramework PUBLIC sf_boost_deps)
target_include_directories(SillyFramework PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/stb)
target_link_libraries(SillyFramework PRIVATE uv_a)
target_link_libraries(SillyFramework PRIVATE uv-cpp)
target_link_libraries(SillyFramework PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
if(WIN32)
    target_link_libraries(SillyFramework PRIVATE imm32)
endif()


set(SILLY_GAME_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/main)
file(GLOB_RECURSE SILLY_GAME_SOURCES ${SILLY_GAME_SOURCE_ROOT}/*.cpp)
add_executable(SillyGame ${SILLY_GAME_SOURCES})
compiler_static_lib(SillyGame)
target_include_directories(SillyGame PRIVATE ${SILLY_GAME_SOURCE_ROOT})
target_link_libraries(SillyGame PRIVATE SillyFramework)
make_windows_compiler_happy(SillyGame)
add_custom_command(TARGET SillyGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:SillyGame>/res)
