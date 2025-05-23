# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.10)

# 配置项目名
project(SysDev)

#告知当前使用的是交叉编译方式，必须配置
if (NOT CMAKE_SYSTEM_NAME)
    # 设置目标操作系统名称
    set(CMAKE_SYSTEM_NAME windows)
    # 设置目标操作系统架构
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
endif()

# 修复MSVC编译警告
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 设置编译后的静态库文件的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
# 设置编译后的动态库文件的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
# 设置编译后的可执行文件的输出成目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
# 设定编译后生成的制品文件安装目录前缀
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)

# 配置引用头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/src)
# 配置引用库链接目录（静态库或动态库都行）
link_directories(${ARCHIVE_OUTPUT_DIRECTORY}/lib
                 ${LIBRARY_OUTPUT_DIRECTORY}/lib)

# 转换一下大小写方便打包
string(TOLOWER "${CMAKE_SYSTEM_NAME}" BUILD_OS)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" BUILD_ARCH)

# 打印一下系统类型
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
# 根据条件添加子目录
if(CMAKE_SYSTEM_NAME MATCHES "Windows") # windows环境下需要编译的内容
    # 增加子目录
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/windows)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test/windows)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux") # linux环境下需要编译的内容
    # 增加子目录
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/linux)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test/linux)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin") # mac环境下需要编译的内容
     # 增加子目录
     add_subdirectory(${CMAKE_SOURCE_DIR}/src/darwin)
     add_subdirectory(${CMAKE_SOURCE_DIR}/test/darwin)
else()
    message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}, 暂无匹配该OS的动作")
endif()