# 配置相关源文件
set(SOURCES sysdev.cpp NetworkCard.cpp)

# # 配置静态库生成参数
add_library(sysdev_linux STATIC ${SOURCES})
set_target_properties(sysdev_linux PROPERTIES OUTPUT_NAME "sysdev")

# 指定make install后头文件、静态库，动态库，可执行文件存放目录
set(INSTALL_PATH libsysdev_${BUILD_OS}_${BUILD_ARCH})
# 头文件
install(FILES ${CMAKE_SOURCE_DIR}/include/sysdev/sysdev.h DESTINATION ${INSTALL_PATH}/include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sysdev_${TOOLCHAIN_VENDER}.pc DESTINATION ${INSTALL_PATH} RENAME "sysdev.pc")
# ARCHIVE-静态库，LIBRATY-动态库，RUNTIME-可执行二进制文件
install(TARGETS sysdev_linux ARCHIVE DESTINATION ${INSTALL_PATH}/lib)
