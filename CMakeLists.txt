cmake_minimum_required(VERSION 3.12)
project(network)

#获取Git最后一次提交的版本描述
find_package(Git)
execute_process(COMMAND
    "${GIT_EXECUTABLE}" describe --tags --always --abbrev=40 --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_VERSION
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)
#写入变量
string(REGEX MATCH "^v([0-9]+)\\.([0-9]+)\\.([0-9]+)" GIT_VERSION_NUM "${GIT_VERSION}")
if (NOT "${GIT_VERSION_NUM}" STREQUAL "")
    set(GIT_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(GIT_VERSION_MINOR ${CMAKE_MATCH_2})
    set(GIT_VERSION_PATCH ${CMAKE_MATCH_3})
else()
    message(FATAL_ERROR "No find GIT_VERSION")
endif()

#获取Git最后一次提交的日期
execute_process(COMMAND
    "${GIT_EXECUTABLE}" log -1 --format=%cd --date=local
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_DATE
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)

#获取Git最后一次提交的主题
execute_process(COMMAND
    "${GIT_EXECUTABLE}" log -1 --format=%s
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)
#在make目录生成版本文件
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/src/Version.h" @ONLY)
#设置编译参数
message(STATUS "compiler: " ${CMAKE_CXX_COMPILER_ID})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#查找依赖包
find_package(glfw3 REQUIRED)

if (MSVC)
    # MSVC链接静态库(/MT),替换/MD为/MT来进行链接静态库,使用RUNTIME_STATIC_LINK控制链接静态库还是动态库
    if(MSVC AND RUNTIME_STATIC_LINK)
        foreach(flag
            CMAKE_C_FLAGS
            CMAKE_CXX_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
    #关闭警告
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS 
        -D_SCL_SECURE_NO_WARNINGS
        -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        -D_WIN32_WINDOWS
    )
    #使用Windows共享库,添加这个设置不用使用"__declspec(dllimport)"去导出符号
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

FILE(GLOB_RECURSE Headers  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
add_library(${PROJECT_NAME} INTERFACE Headers)
target_include_directories(${PROJECT_NAME} INTERFACE include)

