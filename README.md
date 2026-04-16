# OctoMap ROS 2 Humble Port

This is a standalone port of the OctoMap library for ROS 2 Humble. It provides a server to generate 3D occupancy maps from point clouds and a client to receive and deserialize these maps.

## Architecture

- **OctomapServer**: 
  - Subscribes to `/cloud_in` (`sensor_msgs/msg/PointCloud2`).
  - Uses `tf2_ros` to transform points from the sensor frame to the `world` frame.
  - Maintains an `octomap::OcFoM` (Occupancy Mapping) model.
  - Publishes the binary map to `/octomap_binary` (`octomap_msgs/msg/Octomap`).
- **OctomapClient**:
  - Subscribes to `/octomap_binary`.
  - Deserializes the binary stream into an `octomap::OcTree` for processing.
- **Smoke Test Node**:
  - A utility node that broadcasts a static transform (`world` -> `sensor_frame`) and publishes dummy point cloud data to verify the end-to-end pipeline.

## Implementation Details

- **Independence**: The implementation uses `rclcpp` and `tf2_ros` natively, avoiding any ROS 1 wrapper or bridge.
- **Memory Management**: Uses `std::shared_ptr` for ROS 2 nodes and OctoMap objects to ensure safe memory handling.
- **Performance**: Implements `sensor_msgs::PointCloud2Iterator` for efficient point cloud traversal.

## Usage

### Build
```bash
colcon build --packages-select octomap_ros2_humble
source install/setup.bash
```

### Running the System
1. **Start the Server**:
   ```bash
   ros2 run octomap_ros2_humble octomap_server_node
   ```
2. **Start the Client**:
   ```bash
   ros2 run octomap_ros2_humble octomap_client_node
   ```
3. **Run the Smoke Test**:
   ```bash
   ros2 run octomap_ros2_humble octomap_smoke_test_node
   ```

## Verification
The system is verified by running the smoke test node, which sends a point at (0,0,0) in the sensor frame. With a transform of (1,1,1), the server should insert a node at world coordinates (1,1,1), and the client should report the increase in the number of OctoMap nodes.
