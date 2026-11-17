cmake_minimum_required(VERSION 3.8)
project(hardware_unitree_mujoco)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

# Generate symbols for IDE indexer
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THIS_PACKAGE_INCLUDE_DEPENDS
        hardware_interface
        pluginlib
        rclcpp
        rclcpp_lifecycle
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(backward_ros REQUIRED)

# Unitree SDK2 is vendored inside this package (thirdparty/unitree_sdk2) so the
# workspace builds and runs with no external SDK install, no /tmp paths, and no
# extra CMAKE_PREFIX_PATH. To use a system SDK instead, pass
#   -DUNITREE_SDK2_PREFER_SYSTEM=ON
set(UNITREE_SDK2_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/unitree_sdk2")
option(UNITREE_SDK2_PREFER_SYSTEM "Use a system-installed Unitree SDK2 if found" OFF)
if (NOT UNITREE_SDK2_PREFER_SYSTEM)
    list(PREPEND CMAKE_PREFIX_PATH "${UNITREE_SDK2_ROOT}")
    set(unitree_sdk2_DIR "${UNITREE_SDK2_ROOT}/lib/cmake/unitree_sdk2" CACHE PATH "" FORCE)
endif ()
find_package(unitree_sdk2 REQUIRED)

foreach (Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${Dependency} REQUIRED)
endforeach ()
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

include(GNUInstallDirs)
add_library(
        ${PROJECT_NAME}
        SHARED
        src/HardwareUnitree.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
        PRIVATE src
)
target_link_libraries(${PROJECT_NAME} unitree_sdk2)

# The SDK2 static library needs libddsc/libddscxx at run time. Ship the two
# shared objects next to the plugin and resolve them through $ORIGIN, so the
# plugin loads with no LD_LIBRARY_PATH.
# the .so files are the real objects; the .so.0 entries in the SDK tree are
# symlinks, and install(FILES) would copy them as broken links
set(UNITREE_SDK2_RUNTIME_LIBS
        "${UNITREE_SDK2_ROOT}/lib/libddsc.so"
        "${UNITREE_SDK2_ROOT}/lib/libddscxx.so")
set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN")
ament_target_dependencies(
        ${PROJECT_NAME}
        ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

pluginlib_export_plugin_description_file(hardware_interface HardwareUnitree.xml)

install(
        DIRECTORY include/
        DESTINATION include/${PROJECT_NAME}
)

# install under the SONAME the plugin actually links against, as real files
install(FILES "${UNITREE_SDK2_ROOT}/lib/libddsc.so" DESTINATION lib RENAME libddsc.so.0)
install(FILES "${UNITREE_SDK2_ROOT}/lib/libddscxx.so" DESTINATION lib RENAME libddscxx.so.0)

install(
        TARGETS ${PROJECT_NAME}
        EXPORT export_${PROJECT_NAME}
        ARCHIVE DESTINATION ib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

if (BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    # the following line skips the linter which checks for copyrights
    # comment the line when a copyright and license is added to all source files
    set(ament_cmake_copyright_FOUND TRUE)
    # the following line skips cpplint (only works in a git repo)
    # comment the line when this package is in a git repo and when
    # a copyright and license is added to all source files
    set(ament_cmake_cpplint_FOUND TRUE)
    ament_lint_auto_find_test_dependencies()
endif ()

ament_export_dependencies(${dependencies})
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

install(
        DIRECTORY launch
        DESTINATION share/${PROJECT_NAME}/
)

ament_package()
