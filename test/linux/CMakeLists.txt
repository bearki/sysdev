# 编译为可执行文件
add_executable(sysdev_test ${CMAKE_CURRENT_SOURCE_DIR}/../common/sysdev.cpp)

# 指定需要链接的库
# 链接到静态库（注意：库名需要填写cmake add_library函数的第一个参数name，否则找不到库）
target_link_libraries(sysdev_test sysdev_linux)

# 指定make install后静态库，动态库，可执行文件存放目录
set(INSTALL_PATH libsysdev_${BUILD_OS}_${BUILD_ARCH})
# ARCHIVE-静态库，LIBRATY-动态库，RUNTIME-可执行二进制文件
install(TARGETS sysdev_test RUNTIME DESTINATION ${INSTALL_PATH}/bin)