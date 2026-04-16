# Design Document: OctoMap ROS 2 Humble Port

## Goals
The goal of this project is to provide a standalone, high-performance ROS 2 Humble implementation of the OctoMap server and client, removing dependencies on ROS 1.

## System Design

### 1. OctomapServer
The server acts as the primary producer of the 3D map.
- **Inputs**: 
  - PointCloud2 messages from sensors.
  - TF transforms to align sensor data with the world frame.
- **Internal Process**:
  - Points are iterated using `PointCloud2Iterator`.
  - Points are transformed using `tf2_ros::Buffer`.
  - Points are inserted into an `octomap::OcFoM` instance.
- **Outputs**:
  - Binary serialized OctoMap messages.

### 2. OctomapClient
The client allows other nodes to consume the map.
- **Inputs**: Binary OctoMap messages.
- **Process**: Deserializes the `std::vector<uint8_t>` data into an `octomap::OcTree`.

### 3. Smoke Test
The smoke test validates the integration.
- **Mechanism**:
  - Broadcaster: `world` $\rightarrow$ `sensor_frame` (translation: 1,1,1).
  - Publisher: Point at (0,0,0) in `sensor_frame`.
  - Expectation: Server records point at (1,1,1) in `world`.

## Dependencies
- `rclcpp`: ROS 2 C++ Client Library.
- `octomap`: The core OctoMap library.
- `octomap_msgs`: ROS 2 messages for OctoMap.
- `tf2_ros` / `tf2_geometry_msgs`: Transform management.
- `sensor_msgs`: PointCloud2 data.
