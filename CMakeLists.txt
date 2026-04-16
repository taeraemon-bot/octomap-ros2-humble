cmake_minimum_required(VERSION 3.8)
project(octomap_ros2_humble)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(octomap REQUIRED)
find_package(octomap_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

include_directories(
  include
)

# Core Node
add_executable(octomap_server_node src/octomap_ros2_humble/octomap_server.cpp)
target_link_libraries(octomap_server_node
  rclcpp::rclcpp
  sensor_msgs::sensor_msgs
  octomap::octomap
  octomap_msgs::octomap_msgs
  tf2_ros::tf2_ros
  tf2_geometry_msgs::tf2_geometry_msgs
)

install(TARGETS
  octomap_server_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
