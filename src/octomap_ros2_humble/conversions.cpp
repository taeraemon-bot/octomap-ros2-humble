/**
 * OctoMap ROS 2 Humble Integration - Conversion Utilities Implementation
 * 
 * Based on original octomap_ros by A. Hornung, University of Freiburg
 * Ported to ROS 2 Humble with modern C++ practices
 * 
 * @author Mashiro (Digital Maid Assistant)
 * @license BSD
 */

#include "octomap_ros2_humble/conversions.hpp"
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <stdexcept>
#include <algorithm>

namespace octomap_ros2_humble {

void pointsOctomapToPointCloud2(const octomap::point3d_list& points, 
                                 sensor_msgs::msg::PointCloud2& cloud) {
    // Initialize point cloud fields
    cloud.fields.clear();
    cloud.fields.push_back(sensor_msgs::msg::PointField());
    cloud.fields[0].name = "x";
    cloud.fields[0].offset = 0;
    cloud.fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[0].count = 1;

    cloud.fields.push_back(sensor_msgs::msg::PointField());
    cloud.fields[1].name = "y";
    cloud.fields[1].offset = 4;
    cloud.fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[1].count = 1;

    cloud.fields.push_back(sensor_msgs::msg::PointField());
    cloud.fields[2].name = "z";
    cloud.fields[2].offset = 8;
    cloud.fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[2].count = 1;

    cloud.point_step = 12; // 3 floats * 4 bytes
    cloud.row_step = 0; // Point cloud, not organized
    cloud.height = 1; // Point cloud
    cloud.width = static_cast<uint32_t>(points.size());
    cloud.is_bigendian = false;
    cloud.is_dense = true;

    // Allocate memory
    cloud.data.resize(cloud.width * cloud.point_step);

    // Fill point data
    sensor_msgs::PointCloud2Iterator<float> iter_x(cloud, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(cloud, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(cloud, "z");

    for (const auto& pt : points) {
        *iter_x = static_cast<float>(pt.x());
        *iter_y = static_cast<float>(pt.y());
        *iter_z = static_cast<float>(pt.z());
        ++iter_x;
        ++iter_y;
        ++iter_z;
    }
}

void pointCloud2ToOctomap(const sensor_msgs::msg::PointCloud2& cloud,
                          octomap::Pointcloud& octomapCloud) {
    // Validate fields
    bool has_x = false, has_y = false, has_z = false;
    for (const auto& field : cloud.fields) {
        if (field.name == "x" || field.name == "X") has_x = true;
        else if (field.name == "y" || field.name == "Y") has_y = true;
        else if (field.name == "z" || field.name == "Z") has_z = true;
    }

    if (!has_x || !has_y || !has_z) {
        throw std::runtime_error("PointCloud2 must contain x, y, z fields");
    }

    // Estimate size for reserve
    const auto point_count = cloud.height == 0 ? cloud.width : cloud.width * cloud.height;
    octomapCloud.reserve(point_count);

    // Iterate and convert
    sensor_msgs::PointCloud2ConstIterator<float> iter_x(cloud, "x");
    sensor_msgs::PointCloud2ConstIterator<float> iter_y(cloud, "y");
    sensor_msgs::PointCloud2ConstIterator<float> iter_z(cloud, "z");
    const auto end = iter_x + point_count;

    for (; iter_x != end; ++iter_x, ++iter_y, ++iter_z) {
        if (!std::isnan(*iter_x) && !std::isnan(*iter_y) && !std::isnan(*iter_z)) {
            octomapCloud.push_back(
                static_cast<double>(*iter_x),
                static_cast<double>(*iter_y),
                static_cast<double>(*iter_z)
            );
        }
    }
}

} // namespace octomap_ros2_humble
