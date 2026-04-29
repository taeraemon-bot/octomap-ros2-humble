# OctoMap ROS 2 Humble

A complete, modern ROS 2 Humble port of the OctoMap library for 3D occupancy mapping. This package provides server and client nodes for generating, publishing, and analyzing 3D occupancy grid maps from point cloud data.

## 🌟 Features

- **OctoMapServer**: Subscribes to point clouds, transforms them to world frame, and generates 3D occupancy maps
- **OctoMapClient**: Receives serialized OctoMap data and provides detailed statistics
- **SmokeTest**: End-to-end testing utility with configurable parameters
- **Conversion Utilities**: Reusable functions for converting between OctoMap and ROS 2 types
- **Modern C++17**: Clean, efficient code with smart pointers and error handling
- **Full ROS 2 Humble Support**: Native implementation without ROS 1 dependencies

## 📋 Requirements

- **ROS 2**: Humble Hawksbill (recommended) or Iron
- **C++**: Version 17 or higher
- **Dependencies**:
  - `octomap` (OctoMap library)
  - `sensor_msgs`
  - `octomap_msgs`
  - `tf2_ros`
  - `tf2_geometry_msgs`

## 🛠️ Installation

### 1. Install ROS 2 Humble (if not already installed)

```bash
# Ubuntu 22.04
sudo apt update
sudo apt install -y ros-humble-desktop
source /opt/ros/humble/setup.bash
```

### 2. Install OctoMap Library

```bash
# Ubuntu with ROS 2 Humble
sudo apt install -y ros-humble-octomap ros-humble-octomap-msgs
```

### 3. Clone and Build

```bash
# Create ROS 2 workspace
mkdir -p ~/octomap_ws/src
cd ~/octomap_ws/src

# Clone this repository
git clone https://github.com/taeraemon-bot/octomap-ros2-humble.git
cd ..

# Build
source /opt/ros/humble/setup.bash
colcon build --packages-select octomap_ros2_humble

# Source the workspace
source install/setup.bash
```

## 🚀 Usage

### Quick Start (Smoke Test)

Run all three nodes to test the complete pipeline:

```bash
# Terminal 1: Start the server
ros2 run octomap_ros2_humble octomap_server_node

# Terminal 2: Start the client
ros2 run octomap_ros2_humble octomap_client_node

# Terminal 3: Run the smoke test
ros2 run octomap_ros2_humble octomap_smoke_test_node
```

### With Real Point Cloud Data

The server subscribes to `/cloud_in` (`sensor_msgs::msg::PointCloud2`):

```bash
# Terminal 1: Start the server
ros2 run octomap_ros2_humble octomap_server_node

# Terminal 2: Start the client
ros2 run octomap_ros2_humble octomap_client_node

# Terminal 3: Publish your point cloud data
ros2 bag play your_pointcloud.bag --remap /points:=/cloud_in
```

## 📊 Parameters

### OctoMap Server

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `resolution` | double | 0.1 | OctoMap resolution in meters |
| `max_range` | double | 10.0 | Maximum range from origin (meters) |
| `max_nodes` | int | 100000 | Maximum number of nodes |
| `publish_free_space` | bool | false | Publish free space information |

### OctoMap Client

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualize_statistics` | bool | true | Print detailed statistics |
| `max_updates` | int | -1 | Max messages to process (-1 = unlimited) |

### Smoke Test

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `points_per_cloud` | int | 10 | Number of points per message |
| `number_of_messages` | int | 5 | Messages to publish (-1 = unlimited) |
| `publish_frequency` | double | 1.0 | Publishing frequency (Hz) |
| `random_seed` | int | 42 | Random seed for reproducibility |

### Custom Parameters

```bash
# Example with custom parameters
ros2 run octomap_ros2_humble octomap_server_node \
  --ros-args -p resolution:=0.05 -p max_range:=20.0

ros2 run octomap_ros2_humble octomap_smoke_test_node \
  --ros-args -p points_per_cloud:=50 -p number_of_messages:=10
```

## 🔌 Topics

### Subscribers

- **`/cloud_in`** (`sensor_msgs::msg::PointCloud2`): Input point cloud data

### Publishers

- **`/octomap_binary`** (`octomap_msgs::msg::Octomap`): Serialized OctoMap

### TF Requirements

- Requires transform from `world` frame to point cloud's frame_id
- Transform is looked up with 1 second timeout

## 📈 Output Example

### OctoMap Client Output

```
[INFO] [octomap_client]: === Received Octomap #1 ===
[INFO] [octomap_client]:   Resolution: 0.1000 m
[INFO] [octomap_client]:   Tree depth: 16
[INFO] [octomap_client]:   Total nodes: 127
[INFO] [octomap_client]:   Occupied nodes: 27
[INFO] [octomap_client]:   Memory usage: 0.01 MB
[INFO] [octomap_client]:   Bounds: [0.90, 0.90, 0.90] to [1.10, 1.10, 1.10]
[INFO] [octomap_client]:   Occupancy rate: 21.26%
```

## 🔍 Architecture

```
┌─────────────────┐
│ Point Cloud     │
│ (sensor_frame)  │
└────────┬────────┘
         │ /cloud_in
         ▼
┌─────────────────┐
│  OctoMap Server │ ◄─── TF: sensor_frame → world
│  (transforms &  │
│   builds map)   │
└────────┬────────┘
         │ /octomap_binary
         ▼
┌─────────────────┐
│  OctoMap Client │
│  (analyzes map) │
└─────────────────┘
```

## 🧪 Development

### Build from Source

```bash
cd ~/octomap_ws
colcon build --packages-select octomap_ros2_humble --cmake-args -DCMAKE_BUILD_TYPE=Debug
source install/setup.bash
```

### Run Tests

```bash
colcon test --packages-select octomap_ros2_humble
colcon test-result --verbose
```

### Linting

```bash
colcon lint --packages-select octomap_ros2_humble
```

## 📝 Conversion Utilities

The package includes a header-only library for common conversions:

```cpp
#include <octomap_ros2_humble/conversions.hpp>

// Point conversions
octomap::point3d pt = octomap_ros2_humble::pointMsgToOctomap(msg_pt);
geometry_msgs::msg::Point msg_pt = octomap_ros2_humble::pointOctomapToMsg(pt);

// Point cloud conversions
octomap::Pointcloud octomap_cloud;
octomap_ros2_humble::pointCloud2ToOctomap(cloud_msg, octomap_cloud);
```

## 🤝 Credits

- **Original OctoMap**: Aron Hornung, University of Freiburg
- **ROS 1 Implementation**: `octomap_ros` package
- **ROS 2 Port**: taeraemon-bot
- **Assistant**: Mashiro (Digital Maid) 🙇‍♀️

## 📄 License

BSD License - See LICENSE file for details.

## 🐛 Troubleshooting

### TF Lookup Failed

```
[WARN] [octomap_server]: TF lookup failed: ...
```

**Solution**: Ensure you're broadcasting the transform from `world` to your sensor frame:

```bash
ros2 run tf2_ros static_transform_publisher --x 1.0 --y 1.0 --z 1.0 --qx 0.0 --qy 0.0 --qz 0.0 --qw 1.0 world sensor_frame
```

### No Points in Point Cloud

Check that your point cloud has valid XYZ fields and is not empty.

## 📚 References

- [OctoMap Repository](https://github.com/OctoMap/octomap)
- [OctoMap ROS (ROS 1)](https://github.com/OctoMap/octomap_ros)
- [ROS 2 Documentation](https://docs.ros.org/en/humble/)

---

**Made with ❤️ by taeraemon-bot and Mashiro**
