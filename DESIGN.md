# 🎀 OctoMap ROS 2 Humble Porting Design Document

## 1. Goal
To create a standalone ROS 2 Humble wrapper for the OctoMap library, ensuring independence from ROS 1 legacy code and maximizing efficiency.

## 2. Architecture
The project will be divided into a core library dependency and a ROS 2 interface layer.

### 2.1 Dependency Layer
- **OctoMap (C++ Library):** Use the official `octomap` library for 3D occupancy grid mapping.
- **ROS 2 Humble:** Target distribution.
- **rclcpp:** ROS 2 C++ client library.
- **sensor_msgs:** For `PointCloud2` input.
- **octomap_msgs:** For Octomap data transmission.

### 2.2 Key Components to Implement
- **`OctomapServerNode`**: 
    - Subscribes to `sensor_msgs/msg/PointCloud2`.
    - Updates the OctoMap model.
    - Provides services to save/load maps.
    - Publishes the map to `octomap_msgs/msg/Octomap`.
- **`OctomapClientNode`**:
    - Receives OctoMap data and provides a simplified interface for other nodes.
- **`Tf2 Integration`**:
    - Handle coordinate transformations using `tf2_ros` to ensure accurate map alignment.

## 3. Porting Strategy (ROS 1 $\rightarrow$ ROS 2)
| Feature | ROS 1 (Official) | ROS 2 Humble (Port) |
| :--- | :--- | :--- |
| **Node Base** | `ros::NodeHandle` | `rclcpp::Node` |
| **Build System** | `catkin` | `ament_cmake` |
| **Comm. Model** | Synchronous/Asynchronous | Managed by Executors / Callbacks |
| **TF** | `tf` | `tf2_ros` |
| **Parameters** | `ros::NodeHandle::getParam` | `this->get_parameter()` |

## 4. Implementation Roadmap
1. **Phase 1: Skeleton** - Create package structure, `package.xml`, `CMakeLists.txt`.
2. **Phase 2: Core Integration** - Link `octomap` library and verify basic build.
3. **Phase 3: Server Implementation** - Implement PointCloud $\rightarrow$ Octomap update logic.
4. **Phase 4: TF2 & Messaging** - Integrate `tf2_ros` and `octomap_msgs`.
5. **Phase 5: Validation** - Test with sample data in RViz2.
