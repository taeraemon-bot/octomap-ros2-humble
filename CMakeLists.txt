cmake_minimum_required(VERSION 3.8)
project(octomap_ros2_humble)

# C++ standard
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(octomap REQUIRED)
find_package(octomap_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# Include directories
include_directories(
  include
)

# Library: conversions
add_library(octomap_ros2_humble_conversions 
  src/octomap_ros2_humble/conversions.cpp
)
target_include_directories(octomap_ros2_humble_conversions PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(octomap_ros2_humble_conversions
  rclcpp::rclcpp
  sensor_msgs::sensor_msgs
  octomap::octomap
  octomap_msgs::octomap_msgs
  tf2_ros::tf2_ros
  tf2_geometry_msgs::tf2_geometry_msgs
)

# Executable: OctoMap Server Node
add_executable(octomap_server_node 
  src/octomap_ros2_humble/octomap_server.cpp
)
target_link_libraries(octomap_server_node
  octomap_ros2_humble_conversions
  rclcpp::rclcpp
  sensor_msgs::sensor_msgs
  octomap::octomap
  octomap_msgs::octomap_msgs
  tf2_ros::tf2_ros
  tf2_geometry_msgs::tf2_geometry_msgs
)

# Executable: OctoMap Client Node
add_executable(octomap_client_node 
  src/octomap_ros2_humble/octomap_client.cpp
)
target_link_libraries(octomap_client_node
  octomap_ros2_humble_conversions
  rclcpp::rclcpp
  octomap::octomap
  octomap_msgs::octomap_msgs
)

# Executable: Smoke Test Node
add_executable(octomap_smoke_test_node 
  src/octomap_ros2_humble/smoke_test.cpp
)
target_link_libraries(octomap_smoke_test_node
  octomap_ros2_humble_conversions
  rclcpp::rclcpp
  sensor_msgs::sensor_msgs
  tf2_ros::tf2_ros
  tf2_geometry_msgs::tf2_geometry_msgs
)

# Installation targets
install(TARGETS
  octomap_server_node
  octomap_client_node
  octomap_smoke_test_node
  octomap_ros2_humble_conversions
  DESTINATION lib/${PROJECT_NAME}
)

# Installation: headers
install(DIRECTORY include/
  DESTINATION include
)

# Export dependencies for other packages
ament_export_targets(export HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp
  sensor_msgs
  octomap
  octomap_msgs
  tf2_ros
  tf2_geometry_msgs
)

# Ament package
ament_package()
